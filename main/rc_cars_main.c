/* RC CARS

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "ps3.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

// Define GPIO
#define GPIO_OUTPUT_SERVO       CONFIG_CONFIG_GPIO_OUTPUT_15_SERVO
#define GPIO_OUTPUT_MOTOR_UP    CONFIG_CONFIG_GPIO_OUTPUT_12_MOTOR_EN1
#define GPIO_OUTPUT_MOTOR_DN    CONFIG_CONFIG_GPIO_OUTPUT_13_MOTOR_EN2
#define GPIO_OUTPUT_PIN_SEL (                               \
                                (1ULL<<CONFIG_CONFIG_GPIO_OUTPUT_15_SERVO)     | \
                                (1ULL<<CONFIG_CONFIG_GPIO_OUTPUT_12_MOTOR_EN1) | \
                                (1ULL<<CONFIG_CONFIG_GPIO_OUTPUT_13_MOTOR_EN2)   \
                            )

#define LEDC_MODE               LEDC_LOW_SPEED_MODE

#define LEDC_TIMER_MOTOR        LEDC_TIMER_0
#define LEDC_DUTY_RES_MOTOR     LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_FREQUENCY_MOTOR    40000 // Frequency in Hertz. Set frequency at 40 kHz

#define LEDC_TIMER_SERVO        LEDC_TIMER_1
#define LEDC_DUTY_RES_SERVO     LEDC_TIMER_16_BIT // Set duty resolution to 16 bits
#define LEDC_FREQUENCY_SERVO    50 // Frequency in Hertz. Set frequency at 50 Hz

#define MIN_TIME_SERVO          3863 // 1639
#define MAX_TIME_SERVO          6100 // 8196
#define AVG_TIME_SERVO          4918 // 4918 ~ 1500 us

#define DELTA_MAX_AVG_SERVO     (MAX_TIME_SERVO - AVG_TIME_SERVO)
#define DELTA_AVG_MIN_SERVO     (AVG_TIME_SERVO - MIN_TIME_SERVO)

#define STEP_MAX_AVG_SERVO      DELTA_MAX_AVG_SERVO / 255.0
#define STEP_AVG_MIN_SERVO      DELTA_AVG_MIN_SERVO / 255.0

#define ESP_INTR_FLAG_DEFAULT 0

#define WDT_TIMEOUT 3

void controller_event_cb(ps3_t ps3, ps3_event_t event)
{
    uint32_t duty_0 = 0;
    uint32_t duty_1 = 0;
    float    duty_c = 0;

    vTaskDelay(10 / portTICK_PERIOD_MS);
    // printf("(%3d,%3d) \r",ps3.analog.stick.lx,ps3.analog.stick.ly);
    // printf("(%3d,%3d,%3d) \r",ps3.analog.button.l2,ps3.analog.button.r2,ps3.analog.stick.lx);

    // MOTOR
    duty_0 = ps3.analog.button.l2;
    duty_1 = ps3.analog.button.r2;

    if (duty_0 > 5 && duty_1 <= 5){
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_1, duty_0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));
    } else if (duty_1 > 5 && duty_0 <= 5) {
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_0, duty_1));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0));
    } else {
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_0, 0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0));
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_1, 0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));
    }

    // SERVO
    duty_1 = abs(ps3.analog.stick.lx) * 2;
    if (duty_1 > 255) duty_1 = 255;

    if (ps3.analog.stick.lx < -5){
        // min
        duty_c = AVG_TIME_SERVO - duty_1 * STEP_AVG_MIN_SERVO;
        printf("(%f) \n",duty_c);
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_2, (uint32_t) duty_c));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));

    } else if (ps3.analog.stick.lx > 5) {
        // max
        duty_c = AVG_TIME_SERVO + duty_1 * STEP_MAX_AVG_SERVO;
        printf("(%f) \n",duty_c);
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_2, (uint32_t) duty_c));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
    } else {
        // neutral 
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_2, AVG_TIME_SERVO));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
    }


    // duty = abs(ps3.analog.stick.lx) * 2;
    // if (duty > 255) duty = 255;

    // if (ps3.analog.stick.lx <= 0){
    //     ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_0, duty));
    //     ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0));
    // } else {
    //     ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_1, duty));
    //     ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));
    // }
}

static void pwm_init(void)
{
    // MOTOR PWM timer configuration
    ledc_timer_config_t ledc_timer_motor = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES_MOTOR,
        .timer_num        = LEDC_TIMER_MOTOR,
        .freq_hz          = LEDC_FREQUENCY_MOTOR,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_motor));

    ledc_channel_config_t ledc_channel0 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_MOTOR,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_MOTOR_UP,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel0));

    ledc_channel_config_t ledc_channel1 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER_MOTOR,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_MOTOR_DN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));

    // SERVO PWM channel configuration
    ledc_timer_config_t ledc_timer_servo = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES_SERVO,
        .timer_num        = LEDC_TIMER_SERVO,
        .freq_hz          = LEDC_FREQUENCY_SERVO,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_servo));

    ledc_channel_config_t ledc_channel2 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER_SERVO,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_SERVO,
        .duty           = AVG_TIME_SERVO,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));
}

void app_main(void)
{
    // Watchdog cfg
    esp_task_wdt_deinit();

    // PWM init
    pwm_init();

    // PS3 controller init
    ESP_ERROR_CHECK(nvs_flash_init());
    ps3SetEventCallback(controller_event_cb);
    uint8_t mac[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    ps3SetBluetoothMacAddress(mac);
    ps3Init();

    // Wait connection
    while (!ps3IsConnected()) {
        // Prevent the Task Watchdog from triggering
        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf("waiting\r");
    }
    printf("\n");

    // Forever loop
    while (1) {
    }
}

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
#define GPIO_OUTPUT_LEFT    CONFIG_GPIO_OUTPUT_2_LEFT
#define GPIO_OUTPUT_RIGHT   CONFIG_GPIO_OUTPUT_13_RIGHT
#define GPIO_OUTPUT_UP      CONFIG_GPIO_OUTPUT_15_UP
#define GPIO_OUTPUT_DOWN    CONFIG_GPIO_OUTPUT_14_DOWN
#define GPIO_OUTPUT_PIN_SEL (                               \
                                (1ULL<<GPIO_OUTPUT_LEFT)  | \
                                (1ULL<<GPIO_OUTPUT_RIGHT) | \
                                (1ULL<<GPIO_OUTPUT_UP)    | \
                                (1ULL<<GPIO_OUTPUT_DOWN)    \
                            )

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES       LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_FREQUENCY      (40000) // Frequency in Hertz. Set frequency at 4 kHz


#define ESP_INTR_FLAG_DEFAULT 0

#define WDT_TIMEOUT 3

void controller_event_cb(ps3_t ps3, ps3_event_t event)
{
    uint32_t duty = 0;

    vTaskDelay(10 / portTICK_PERIOD_MS);
    printf("(%3d,%3d) \r",ps3.analog.stick.lx,ps3.analog.stick.ly);

    duty = abs(ps3.analog.stick.lx) * 2;
    if (duty > 255) duty = 255;

    if (ps3.analog.stick.lx <= 0){
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0));
        
        // if (ps3.analog.stick.lx == -128){
        //     gpio_set_level(GPIO_OUTPUT_LEFT, 1);
        // } else {
        //     gpio_set_level(GPIO_OUTPUT_LEFT, 0);
        // }
    } else {
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));

        // if (ps3.analog.stick.lx == 127){
        //     gpio_set_level(GPIO_OUTPUT_RIGHT, 1);
        // } else {
        //     gpio_set_level(GPIO_OUTPUT_RIGHT, 0);
        // }
    }

    duty = abs(ps3.analog.stick.ly) * 2;
    if (duty > 255) duty = 255;

    if (ps3.analog.stick.ly <= 0){
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));

        // if (ps3.analog.stick.ly == -128){
        //     gpio_set_level(GPIO_OUTPUT_UP, 1);
        // } else {
        //     gpio_set_level(GPIO_OUTPUT_UP, 0);
        // }
    } else {
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_3, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_3));

        // if (ps3.analog.stick.ly == 127){
        //     gpio_set_level(GPIO_OUTPUT_DOWN, 1);
        // } else {
        //     gpio_set_level(GPIO_OUTPUT_DOWN, 0);
        // }
    }
}

// static void gpio_init(void)
// {
//     // GPIO cfg
//     gpio_config_t io_conf = {};                 // zero-initialize the config structure.
//     io_conf.intr_type = GPIO_INTR_DISABLE;      // disable interrupt
//     io_conf.mode = GPIO_MODE_OUTPUT;            // set as output mode
//     io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // bit mask of the pins that you want to set,e.g.GPIO18/19
//     io_conf.pull_down_en = 0;                   // disable pull-down mode
//     io_conf.pull_up_en = 0;                     // disable pull-up mode
//     gpio_config(&io_conf);                      // configure GPIO with the given settings
// }

static void pwm_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel0 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_LEFT,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel0));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_RIGHT,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel2 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_UP,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel3 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_3,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_OUTPUT_DOWN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel3));
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

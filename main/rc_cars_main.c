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
#include <rom/ets_sys.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "led_effect.h"
#include "led_strip.h"

#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "ps3.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

// Define GPIO
#define GPIO_OUTPUT_LED         CONFIG_GPIO_OUTPUT_2_LED
#define GPIO_OUTPUT_LED2        CONFIG_GPIO_OUTPUT_14_LED
#define GPIO_OUTPUT_SERVO       CONFIG_GPIO_OUTPUT_15_SERVO
#define GPIO_OUTPUT_MOTOR_UP    CONFIG_GPIO_OUTPUT_12_MOTOR_EN1
#define GPIO_OUTPUT_MOTOR_DN    CONFIG_GPIO_OUTPUT_13_MOTOR_EN2
#define GPIO_OUTPUT_PIN_SEL     (1ULL<<CONFIG_GPIO_OUTPUT_2_LED | \
                                 1ULL<<CONFIG_GPIO_OUTPUT_14_LED)

// Define LEDC
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_MOTOR        LEDC_TIMER_0
#define LEDC_DUTY_RES_MOTOR     LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_FREQUENCY_MOTOR    40000 // Frequency in Hertz. Set frequency at 40 kHz
#define LEDC_TIMER_SERVO        LEDC_TIMER_1
#define LEDC_DUTY_RES_SERVO     LEDC_TIMER_16_BIT // Set duty resolution to 16 bits
#define LEDC_FREQUENCY_SERVO    50 // Frequency in Hertz. Set frequency at 50 Hz

// Define SERVO
#define MIN_TIME_SERVO          3863 // 1639
#define MAX_TIME_SERVO          6100 // 8196
#define AVG_TIME_SERVO          4918 // 4918 ~ 1500 us
#define DELTA_MAX_AVG_SERVO     (MAX_TIME_SERVO - AVG_TIME_SERVO)
#define DELTA_AVG_MIN_SERVO     (AVG_TIME_SERVO - MIN_TIME_SERVO)
#define STEP_MAX_AVG_SERVO      DELTA_MAX_AVG_SERVO / 255.0
#define STEP_AVG_MIN_SERVO      DELTA_AVG_MIN_SERVO / 255.0

// Define LED
#define LED_NUMBERS             18
#define DELAY_50_MS             pdMS_TO_TICKS( 50)
#define DELAY_100_MS            pdMS_TO_TICKS(100)
#define DELAY_200_MS            pdMS_TO_TICKS(200)
#define DELAY_500_MS            pdMS_TO_TICKS(500)

// Global variable
enum   led_mode_enum{
                                DISABLE,
                                BLINK,
                                POLICE,
                                POLICE2,
                                DRAG_R,
                                DRAG_G,
                                DRAG_B,
                                RANDOM_R,
                                RANDOM_G,
                                RANDOM_B,
                                SHADOW_R,
                                SHADOW_G,
                                SHADOW_B
};
enum led_mode_enum led_mode = DISABLE;

//###############################################################
//
//   ######  ######## ########  ##         ######## ##     ## ######## ##    ## ######## 
//  ##    ##    ##    ##     ## ##         ##       ##     ## ##       ###   ##    ##    
//  ##          ##    ##     ## ##         ##       ##     ## ##       ####  ##    ##    
//  ##          ##    ########  ##         ######   ##     ## ######   ## ## ##    ##    
//  ##          ##    ##   ##   ##         ##        ##   ##  ##       ##  ####    ##    
//  ##    ##    ##    ##    ##  ##         ##         ## ##   ##       ##   ###    ##    
//   ######     ##    ##     ## ########   ########    ###    ######## ##    ##    ## 
//
//###############################################################
void controller_event_cb(ps3_t ps3, ps3_event_t event)
{
    uint32_t duty_0 = 0;
    uint32_t duty_1 = 0;
    float    duty_c = 0;

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
        // printf("(%f) \n",duty_c);
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_2, (uint32_t) duty_c));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));

    } else if (ps3.analog.stick.lx > 5) {
        // max
        duty_c = AVG_TIME_SERVO + duty_1 * STEP_MAX_AVG_SERVO;
        // printf("(%f) \n",duty_c);
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_2, (uint32_t) duty_c));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
    } else {
        // neutral 
        ESP_ERROR_CHECK(ledc_set_duty   (LEDC_MODE, LEDC_CHANNEL_2, AVG_TIME_SERVO));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
    }

    // LED
    if        (ps3.button.triangle  == 0x1 ){
        led_mode = SHADOW_G;
    } else if (ps3.button.circle    == 0x1 ){
        led_mode = DRAG_G;
    } else if (ps3.button.cross     == 0x1 ){
        led_mode = SHADOW_B;
    } else if (ps3.button.square    == 0x1 ){
        led_mode = DRAG_B;
    } else if (ps3.button.l1        == 0x1){
        led_mode = DISABLE;
    } else if (ps3.button.r1        == 0x1){
        led_mode = POLICE2;
    }
}

//###############################################################
//
//  ##       ######## ########     #### ##    ## #### ######## 
//  ##       ##       ##     ##     ##  ###   ##  ##     ##    
//  ##       ##       ##     ##     ##  ####  ##  ##     ##    
//  ##       ######   ##     ##     ##  ## ## ##  ##     ##    
//  ##       ##       ##     ##     ##  ##  ####  ##     ##    
//  ##       ##       ##     ##     ##  ##   ###  ##     ##    
//  ######## ######## ########     #### ##    ## ####    ##  
//
//###############################################################
static void led_init(ledstrip_channel_handle_t *led_channel_handle)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = GPIO_OUTPUT_LED,
        .max_leds       = LED_NUMBERS,
        .led_model      = LED_MODEL_WS2812,
    };
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &(led_channel_handle->channel0) ));

    led_strip_config_t strip_config2 = {
        .strip_gpio_num = GPIO_OUTPUT_LED2,
        .max_leds       = LED_NUMBERS,
        .led_model      = LED_MODEL_WS2812,
    };
    led_strip_spi_config_t spi_config2 = {
        .spi_bus = SPI3_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config2, &spi_config2, &(led_channel_handle->channel1) ));
}

//###############################################################
//
//   ########  ##      ## ##     ##    #### ##    ## #### ######## 
//   ##     ## ##  ##  ## ###   ###     ##  ###   ##  ##     ##    
//   ##     ## ##  ##  ## #### ####     ##  ####  ##  ##     ##    
//   ########  ##  ##  ## ## ### ##     ##  ## ## ##  ##     ##    
//   ##        ##  ##  ## ##     ##     ##  ##  ####  ##     ##    
//   ##        ##  ##  ## ##     ##     ##  ##   ###  ##     ##    
//   ##         ###  ###  ##     ##    #### ##    ## ####    ##    
//
//###############################################################
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

//###############################################################
//
//  ##     ##    ###    #### ##    ## 
//  ###   ###   ## ##    ##  ###   ## 
//  #### ####  ##   ##   ##  ####  ## 
//  ## ### ## ##     ##  ##  ## ## ## 
//  ##     ## #########  ##  ##  #### 
//  ##     ## ##     ##  ##  ##   ### 
//  ##     ## ##     ## #### ##    ## 
//
//###############################################################
void app_main(void)
{
    uint8_t led_effect_idx = 0;
    ledstrip_channel_handle_t led_channel_handle;

    // Watchdog cfg
    esp_task_wdt_deinit();

    // Led init
    led_init(&led_channel_handle);

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
        led_effect_blink(&led_channel_handle,led_effect_idx);
        vTaskDelay(DELAY_500_MS);
    }
    printf("\n");

    // Forever loop
    while (1) {
        switch ( led_mode ){
            case DISABLE:
                led_effect_disable(&led_channel_handle);
                vTaskDelay(DELAY_500_MS);
                break;
            // case BLINK:
            //     led_effect_blink(&led_channel_handle,led_effect_idx);
            //     vTaskDelay(DELAY_500_MS);
            //     break;
            // case POLICE:
            //     led_effect_police(&led_channel_handle,led_effect_idx);
            //     vTaskDelay(DELAY_500_MS);
            //     break;
            case POLICE2:
                led_effect_police2(&led_channel_handle,led_effect_idx);
                vTaskDelay(DELAY_500_MS);
                break;
            case RANDOM_R:
                led_effect_random(&led_channel_handle,led_effect_idx,40,0,0);
                vTaskDelay(DELAY_50_MS);
                break;
            case RANDOM_G:
                led_effect_random(&led_channel_handle,led_effect_idx,0,40,0);
                vTaskDelay(DELAY_50_MS);
                break;
            case RANDOM_B:
                led_effect_random(&led_channel_handle,led_effect_idx,0,0,40);
                vTaskDelay(DELAY_50_MS);
                break;
            case DRAG_R:
                led_effect_drag(&led_channel_handle,led_effect_idx, 80, 80, 0, 0);
                vTaskDelay(DELAY_50_MS);
                if (led_effect_idx >= 18){
                    led_effect_idx = 0;
                    continue;
                }
                break;
            case DRAG_G:
                led_effect_drag(&led_channel_handle,led_effect_idx, 80, 0, 80, 0);
                vTaskDelay(DELAY_50_MS);
                if (led_effect_idx >= 18){
                    led_effect_idx = 0;
                    continue;
                }
                break;
            case DRAG_B:
                led_effect_drag(&led_channel_handle,led_effect_idx, 80, 0, 0, 80);
                vTaskDelay(DELAY_50_MS);
                if (led_effect_idx >= 18){
                    led_effect_idx = 0;
                    continue;
                }
                break;
            case SHADOW_R:
                led_effect_shadow(&led_channel_handle,led_effect_idx,64,80,0,0);
                vTaskDelay(DELAY_50_MS);
                break;
            case SHADOW_G:
                led_effect_shadow(&led_channel_handle,led_effect_idx,64,0,80,0);
                vTaskDelay(DELAY_50_MS);
                break;
            case SHADOW_B:
                led_effect_shadow(&led_channel_handle,led_effect_idx,64,0,0,80);
                vTaskDelay(DELAY_50_MS);
                break;
            default:
                led_effect_disable(&led_channel_handle);
                vTaskDelay(DELAY_500_MS);
        }

        led_effect_idx += 1;
    }
}

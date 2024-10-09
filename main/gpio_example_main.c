/* GPIO Example

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

#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "ps3.h"
#include "nvs_flash.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output (ESP32C2/ESP32H2 uses GPIO8 as the second output pin)
 * GPIO19: output (ESP32C2/ESP32H2 uses GPIO9 as the second output pin)
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Note. These are the default GPIO pins to be used in the example. You can
 * change IO pins in menuconfig.
 *
 * Test:
 * Connect GPIO18(8) with GPIO4
 * Connect GPIO19(9) with GPIO5
 * Generate pulses on GPIO18(8)/19(9), that triggers interrupt on GPIO4/5
 *
 */

#define GPIO_OUTPUT_IO_0    CONFIG_GPIO_OUTPUT_0
#define GPIO_OUTPUT_IO_1    CONFIG_GPIO_OUTPUT_1
#define GPIO_OUTPUT_IO_2    CONFIG_GPIO_OUTPUT_2
#define GPIO_OUTPUT_IO_3    CONFIG_GPIO_OUTPUT_3
#define GPIO_OUTPUT_IO_4    CONFIG_GPIO_OUTPUT_4
#define GPIO_OUTPUT_PIN_SEL (                              \
                                (1ULL<<GPIO_OUTPUT_IO_0) | \
                                (1ULL<<GPIO_OUTPUT_IO_1) | \
                                (1ULL<<GPIO_OUTPUT_IO_2) | \
                                (1ULL<<GPIO_OUTPUT_IO_3) | \
                                (1ULL<<GPIO_OUTPUT_IO_4)   \
                            )
/*
 * Let's say, GPIO_OUTPUT_IO_0=18, GPIO_OUTPUT_IO_1=19
 * In binary representation,
 * 1ULL<<GPIO_OUTPUT_IO_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<GPIO_OUTPUT_IO_1 is equal to 0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL                0000000000000000000011000000000000000000
 * */
#define GPIO_INPUT_IO_0     CONFIG_GPIO_INPUT_0
#define GPIO_INPUT_IO_1     CONFIG_GPIO_INPUT_1
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
/*
 * Let's say, GPIO_INPUT_IO_0=4, GPIO_INPUT_IO_1=5
 * In binary representation,
 * 1ULL<<GPIO_INPUT_IO_0 is equal to 0000000000000000000000000000000000010000 and
 * 1ULL<<GPIO_INPUT_IO_1 is equal to 0000000000000000000000000000000000100000
 * GPIO_INPUT_PIN_SEL                0000000000000000000000000000000000110000
 * */
#define ESP_INTR_FLAG_DEFAULT 0

// static QueueHandle_t gpio_evt_queue = NULL;

// static void IRAM_ATTR gpio_isr_handler(void* arg)
// {
//     uint32_t gpio_num = (uint32_t) arg;
//     xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
// }

// static void gpio_task_example(void* arg)
// {
//     uint32_t io_num;
//     for (;;) {
//         if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
//             printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
//         }
//     }
// }

void controller_event_cb(ps3_t ps3, ps3_event_t event)
{
    if (ps3.status.battery >= ps3_status_battery_high)
        printf("batt ok\n");

    if (ps3.status.charging)
        printf("Controller is charging\n");

    if (ps3.button.triangle)
        printf("Currently pressing the trangle button\n");

    if (ps3.analog.stick.lx < 0)
        printf("stick left\n");

    if (event.button_down.cross)
        printf("The user started pressing the X button\n");

    if (event.button_up.cross)
        printf("The user released the X button\n");

    if (event.analog_changed.stick.lx)
        printf("The user has moved the left stick sideways\n");
}

void app_main(void)
{
    // CONFIGURE GPIO

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    // io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    // io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    // io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    // io_conf.pull_up_en = 1;
    // gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    // gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    // gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    // xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    // gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin
    // gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    //remove isr handler for gpio number.
    // gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin again
    // gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    // printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());


    printf("PS3!\n");

    ESP_ERROR_CHECK(nvs_flash_init());
    ps3SetEventCallback(controller_event_cb);
    // uint8_t mac[6] = { 0x44, 0xd8, 0x32, 0xbe, 0x1a, 0xc6 };
    // 04:76:6E:D6:FA:61
    // 00:23:06:9A:7D:87
    // 48:22:33:44:55:66
    // uint8_t mac[6] = { 0x04, 0x76, 0x6E, 0xD6, 0xFA, 0x61 };
    // uint8_t mac[6] = { 0x00, 0x23, 0x06, 0x9A, 0x7D, 0x87 };
    uint8_t mac[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    ps3SetBluetoothMacAddress(mac);
    ps3Init();

    while (!ps3IsConnected()) {
        // Prevent the Task Watchdog from triggering
        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf("waiting\n");
    }

    int cnt = 0;
    while (1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, 0);

        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_1, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_1, 0);
        
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_2, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_2, 0);

        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_3, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_3, 0);

        // vTaskDelay(250 / portTICK_PERIOD_MS);
        // gpio_set_level(GPIO_OUTPUT_IO_4, 1);
        // vTaskDelay(250 / portTICK_PERIOD_MS);
        // gpio_set_level(GPIO_OUTPUT_IO_4, 0);
    }
}

/* LED EFFECT

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "led_effect.h"
#include "led_strip.h"

//###############################################################
//
//  #       ######## ########     ######## ######## ######## ########  ######  ######## 
//  #       ##       ##     ##    ##       ##       ##       ##       ##    ##    ##    
//  #       ##       ##     ##    ##       ##       ##       ##       ##          ##    
//  #       ######   ##     ##    ######   ######   ######   ######   ##          ##    
//  #       ##       ##     ##    ##       ##       ##       ##       ##          ##    
//  #       ##       ##     ##    ##       ##       ##       ##       ##    ##    ##    
//  ####### ######## ########     ######## ##       ##       ########  ######     ##  
//
//###############################################################
extern void led_effect_disable(ledstrip_channel_handle_t *led_channel_handle){
    led_strip_clear(led_channel_handle->channel0);
    led_strip_clear(led_channel_handle->channel1);
}

extern void led_effect_blink(ledstrip_channel_handle_t *led_channel_handle, uint8_t index)
{
    if ((index & 0x1) == 0x1) {
        for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
        {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 80, 0, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 80, 0, 0); // red, green, blue
        }
        led_strip_refresh(led_channel_handle->channel0);
        led_strip_refresh(led_channel_handle->channel1);
    } else {
        led_strip_clear(led_channel_handle->channel0);
        led_strip_clear(led_channel_handle->channel1);
    }
}

extern void led_effect_shadow(ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t nb_step, uint8_t max_value)
{
    // Set intensity
    float   step_value = max_value / nb_step;
    uint8_t tmp_value  = (index % nb_step);

    if (tmp_value > (nb_step/2)){
        tmp_value = nb_step - tmp_value;
    }
    tmp_value = tmp_value * step_value;

    for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
    {
        led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 0, tmp_value); // red, green, blue
        led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 0, tmp_value); // red, green, blue
    }
    led_strip_refresh(led_channel_handle->channel0);
    led_strip_refresh(led_channel_handle->channel1);
}

extern void led_effect_police  (ledstrip_channel_handle_t *led_channel_handle, uint8_t index){
    if ((index & 0x1) == 0x1) {
        for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
        {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 80, 0, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 80, 0, 0); // red, green, blue
        }
        led_strip_refresh(led_channel_handle->channel0);
        led_strip_refresh(led_channel_handle->channel1);
    } else {
        for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
        {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 0, 80); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 0, 80); // red, green, blue
        }
        led_strip_refresh(led_channel_handle->channel0);
        led_strip_refresh(led_channel_handle->channel1);
    }
}

extern void led_effect_police2 (ledstrip_channel_handle_t *led_channel_handle, uint8_t index){
    if ((index & 0x1) == 0x1) {
        for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
        {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 80, 0,  0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx,  0, 0, 80); // red, green, blue
        }
        led_strip_refresh(led_channel_handle->channel0);
        led_strip_refresh(led_channel_handle->channel1);
    } else {
        for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
        {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx,  0, 0, 80); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 80, 0,  0); // red, green, blue
        }
        led_strip_refresh(led_channel_handle->channel0);
        led_strip_refresh(led_channel_handle->channel1);
    }
}

extern void led_effect_random  (ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t max_value){
    uint8_t tmp_index;
    for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
    {
        tmp_index = rand() % max_value;
        led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, tmp_index, 0); // red, green, blue
        tmp_index = rand() % max_value;
        led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, tmp_index, 0); // red, green, blue
    }
    led_strip_refresh(led_channel_handle->channel0);
    led_strip_refresh(led_channel_handle->channel1);
}

extern void led_effect_drag    (ledstrip_channel_handle_t *led_channel_handle, uint8_t index){
    uint8_t tmp_index = 17-index;

    for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
    {
        if        (tmp_index == pixel_idx){
            led_strip_set_pixel(led_channel_handle->channel0, tmp_index, 80, 80, 80); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, tmp_index, 80, 80, 80); // red, green, blue
        } else if (tmp_index == (pixel_idx-1)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 80, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 80, 0); // red, green, blue
        } else if (tmp_index == (pixel_idx-2)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 40, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 40, 0); // red, green, blue
        } else if (tmp_index == (pixel_idx-3)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 10, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 10, 0); // red, green, blue
        } else if (tmp_index == (pixel_idx-4)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 5, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 5, 0); // red, green, blue
        } else {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 0, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 0, 0); // red, green, blue
        }
    }
    led_strip_refresh(led_channel_handle->channel0);
    led_strip_refresh(led_channel_handle->channel1);
}

// heart beat

// K2000

// drag / trainee


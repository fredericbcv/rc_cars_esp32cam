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

extern void led_effect_random  (ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t max_value_r, uint8_t max_value_g, uint8_t max_value_b){
    uint8_t tmp_value_r, tmp_value_g, tmp_value_b;
    for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
    {
        if (max_value_r > 0){
            tmp_value_r = rand() % max_value_r;
        } else {
            tmp_value_r = 0;
        }
        if (max_value_g > 0){
            tmp_value_g = rand() % max_value_g;
        } else {
            tmp_value_g = 0;
        }
        if (max_value_r > 0){
            tmp_value_b = rand() % max_value_b;
        } else {
            tmp_value_b = 0;
        }
        led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, tmp_value_r, tmp_value_g, tmp_value_b); // red, green, blue
        
        if (max_value_r > 0){
            tmp_value_r = rand() % max_value_r;
        } else {
            tmp_value_r = 0;
        }
        if (max_value_g > 0){
            tmp_value_g = rand() % max_value_g;
        } else {
            tmp_value_g = 0;
        }
        if (max_value_r > 0){
            tmp_value_b = rand() % max_value_b;
        } else {
            tmp_value_b = 0;
        }
        led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, tmp_value_r, tmp_value_g, tmp_value_b); // red, green, blue
    }
    led_strip_refresh(led_channel_handle->channel0);
    led_strip_refresh(led_channel_handle->channel1);
}

extern void led_effect_drag    (ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t w, uint8_t r, uint8_t g, uint8_t b){
    uint8_t tmp_index = LED_NUMBERS-1-index;

    for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
    {
        if        (tmp_index == pixel_idx){
            led_strip_set_pixel(led_channel_handle->channel0, tmp_index, w, w, w); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, tmp_index, w, w, w); // red, green, blue
        } else if (tmp_index == (pixel_idx-1)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, r, g, b); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, r, g, b); // red, green, blue
        } else if (tmp_index == (pixel_idx-2)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, r/2, g/2, b/2); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, r/2, g/2, b/2); // red, green, blue
        } else if (tmp_index == (pixel_idx-3)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, r/4, g/4, b/4); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, r/4, g/4, b/4); // red, green, blue
        } else if (tmp_index == (pixel_idx-4)){
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, r/8, g/8, b/8); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, r/8, g/8, b/8); // red, green, blue
        } else {
            led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, 0, 0, 0); // red, green, blue
            led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, 0, 0, 0); // red, green, blue
        }
    }
    led_strip_refresh(led_channel_handle->channel0);
    led_strip_refresh(led_channel_handle->channel1);
}

extern void led_effect_shadow(ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t nb_step, uint8_t max_value_r, uint8_t max_value_g, uint8_t max_value_b)
{
    // Set intensity
    float   step_value_r, step_value_g, step_value_b;
    uint8_t tmp_value_r,  tmp_value_g,  tmp_value_b;

    if (max_value_r > 0)
    {
        step_value_r = max_value_r / nb_step;
        tmp_value_r  = (index % nb_step);
        if (tmp_value_r > (nb_step/2)){
            tmp_value_r = nb_step - tmp_value_r;
        }
        tmp_value_r = tmp_value_r * step_value_r;
    } else {
        tmp_value_r = 0;
    }

    if (max_value_g > 0)
    {
        step_value_g = max_value_g / nb_step;
        tmp_value_g  = (index % nb_step);
        if (tmp_value_g > (nb_step/2)){
            tmp_value_g = nb_step - tmp_value_g;
        }
        tmp_value_g = tmp_value_g * step_value_g;
    } else {
        tmp_value_g = 0;
    }

    if (max_value_b > 0)
    {
        step_value_b = max_value_b / nb_step;
        tmp_value_b  = (index % nb_step);
        if (tmp_value_b > (nb_step/2)){
            tmp_value_b = nb_step - tmp_value_b;
        }
        tmp_value_b = tmp_value_b * step_value_b;
    } else {
        tmp_value_b = 0;
    }

    for (int pixel_idx = 0; pixel_idx < LED_NUMBERS; ++pixel_idx)
    {
        led_strip_set_pixel(led_channel_handle->channel0, pixel_idx, tmp_value_r, tmp_value_g, tmp_value_b); // red, green, blue
        led_strip_set_pixel(led_channel_handle->channel1, pixel_idx, tmp_value_r, tmp_value_g, tmp_value_b); // red, green, blue
    }
    led_strip_refresh(led_channel_handle->channel0);
    led_strip_refresh(led_channel_handle->channel1);
}

// heartbeat
// K2000

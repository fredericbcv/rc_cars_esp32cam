#include <stdint.h>
#include "led_strip.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_NUMBERS 18

typedef struct {
    led_strip_handle_t channel0;
    led_strip_handle_t channel1;
} ledstrip_channel_handle_t;

extern void led_effect_disable (ledstrip_channel_handle_t *led_channel_handle);
extern void led_effect_blink   (ledstrip_channel_handle_t *led_channel_handle, uint8_t index);
extern void led_effect_shadow  (ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t nb_step, uint8_t max_value_r, uint8_t max_value_g, uint8_t max_value_b);
extern void led_effect_police  (ledstrip_channel_handle_t *led_channel_handle, uint8_t index);
extern void led_effect_police2 (ledstrip_channel_handle_t *led_channel_handle, uint8_t index);
extern void led_effect_random  (ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t max_value_r, uint8_t max_value_g, uint8_t max_value_b);
extern void led_effect_drag    (ledstrip_channel_handle_t *led_channel_handle, uint8_t index, uint8_t w, uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

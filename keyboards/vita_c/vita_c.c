/**
 * @file vita_c.c
 * @author astro
 * 
 * @copyright Copyright(c) 2023
*/

#include "vita_c.h"

#include "led.h"

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
    {0, 4, 4, 4},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 0, 5},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 1, 4},
    {1, 0, 1},
};

#define CAPS_LED_INDEX 0

void rgb_led_pre_flush(void)
{
    if (host_keyboard_led_state().caps_lock) {
        rgb_linear_set_rgb(1, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
    } else {
        rgb_linear_set_rgb(1, CAPS_LED_INDEX, 0, 0, 0);
    }
}
#endif

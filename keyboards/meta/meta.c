/*
 * @file meta.c
 */

#include "meta.h"
#include "led.h"
#include "amk_printf.h"

#ifdef RGB_LINEAR_ENABLE
#include "rgb_common.h"
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "is31fl3731.h"
#include "is31fl3236.h"
#include "is31fl3729.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    #if 1
    {0,0,0,0},
    // 16 leds
    {0,1,1,1},
    {0,2,2,2},
    {0,3,3,3},
    {0,4,4,4},
    {0,5,5,5},
    {0,6,6,6},
    {0,7,7,7},
    {0,8,8,8},
    {0,9,9,9},
    {0,10,10,10},
    {0,11,11,11},
    {0,12,12,12},
    {0,13,13,13},
    {0,14,14,14},
    {0,15,15,15},
    {0,16,16,16},
    #endif
    #if 0
    // left
    {0, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {0, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {0, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {0, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {0, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {0, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {0, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {0, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {0, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {0, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {0, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {0, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {0, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {0, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {0, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {0, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {0, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {0, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {0, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {0, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {0, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {0, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {0, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {0, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {0, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {0, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {0, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {0,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {0,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {0,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {0,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {0,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {0,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {0,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {0,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {0,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    {0,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {0,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {0,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {0,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {0,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {0,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {0,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {0,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {0,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},
    #endif

    #if 0
    // right
    {1, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {1, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {1, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {1, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {1, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {1, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {1, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {1, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {1, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {1, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {1, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {1, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {1, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {1, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {1, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {1, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {1, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {1, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {1, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {1, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {1, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {1, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {1, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {1, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {1, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {1, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {1, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {1,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {1,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {1,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {1,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {1,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {1,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {1,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {1,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {1,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {1,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {1,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {1,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {1,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {1,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {1,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {1,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {1,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {1,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},
    #endif
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812,     0, 0, 0, 17},
    //{RGB_DRIVER_IS31FL3729, 0x68, 0, 0, 45},
    //{RGB_DRIVER_IS31FL3729, 0x6E, 1, 45, 45},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0,  0, 17},
    //{0,  0, 45},
    //{1,  45, 45},
};

#define CAPS_LED_INDEX    0
void rgb_led_pre_flush(void)
{
    uint8_t led = host_keyboard_leds();
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
        amk_printf("turn caps on\n");
    } else {
        rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0, 0, 0);
        amk_printf("turn caps off\n");
    }
}

#endif

void indicator_led_set(uint8_t led)
{
}

#ifdef RGB_MATRIX_ENABLE
#include "rgb_common.h"
#include "is31fl3729.h"
#include "is31fl3731.h"
#include "rgb_driver.h"

#define META_RGB_V1 0

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // left
#if META_RGB_V1
    {0, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {0, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {0, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {0, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {0, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {0, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {0, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {0, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {0, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {0, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {0, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {0, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {0, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {0, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {0, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {0, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {0, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {0, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {0, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {0, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {0, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {0, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {0, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {0, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {0, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {0, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {0, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {0,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {0,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {0,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {0,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {0,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {0,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {0,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {0,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {0,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    {0,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {0,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {0,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {0,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {0,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {0,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {0,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {0,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {0,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},
#else
    {0,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {0,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {0,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {0,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {0,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {0,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {0,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {0,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {0,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},

    {0,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {0,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {0,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {0,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {0,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {0,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {0,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {0,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {0,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {0, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {0, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {0, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {0, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {0, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {0, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {0, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {0, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {0, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {0, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {0, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {0, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {0, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {0, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {0, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {0, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {0, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {0, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {0, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {0, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {0, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {0, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {0, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {0, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {0, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {0, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {0, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

#endif

    // right
    {1, CS14_SW1_29, CS13_SW1_29, CS15_SW1_29},
    {1, CS14_SW2_29, CS13_SW2_29, CS15_SW2_29},
    {1, CS14_SW3_29, CS13_SW3_29, CS15_SW3_29},
    {1, CS14_SW4_29, CS13_SW4_29, CS15_SW4_29},
    {1, CS14_SW5_29, CS13_SW5_29, CS15_SW5_29},
    {1, CS14_SW6_29, CS13_SW6_29, CS15_SW6_29},
    {1, CS14_SW7_29, CS13_SW7_29, CS15_SW7_29},
    {1, CS14_SW8_29, CS13_SW8_29, CS15_SW8_29},
    {1, CS14_SW9_29, CS13_SW9_29, CS15_SW9_29},

    {1, CS11_SW1_29, CS10_SW1_29, CS12_SW1_29},
    {1, CS11_SW2_29, CS10_SW2_29, CS12_SW2_29},
    {1, CS11_SW3_29, CS10_SW3_29, CS12_SW3_29},
    {1, CS11_SW4_29, CS10_SW4_29, CS12_SW4_29},
    {1, CS11_SW5_29, CS10_SW5_29, CS12_SW5_29},
    {1, CS11_SW6_29, CS10_SW6_29, CS12_SW6_29},
    {1, CS11_SW7_29, CS10_SW7_29, CS12_SW7_29},
    {1, CS11_SW8_29, CS10_SW8_29, CS12_SW8_29},
    {1, CS11_SW9_29, CS10_SW9_29, CS12_SW9_29},

    {1, CS8_SW1_29, CS7_SW1_29, CS9_SW1_29},
    {1, CS8_SW2_29, CS7_SW2_29, CS9_SW2_29},
    {1, CS8_SW3_29, CS7_SW3_29, CS9_SW3_29},
    {1, CS8_SW4_29, CS7_SW4_29, CS9_SW4_29},
    {1, CS8_SW5_29, CS7_SW5_29, CS9_SW5_29},
    {1, CS8_SW6_29, CS7_SW6_29, CS9_SW6_29},
    {1, CS8_SW7_29, CS7_SW7_29, CS9_SW7_29},
    {1, CS8_SW8_29, CS7_SW8_29, CS9_SW8_29},
    {1, CS8_SW9_29, CS7_SW9_29, CS9_SW9_29},

    {1,  CS5_SW1_29,  CS4_SW1_29,  CS6_SW1_29},
    {1,  CS5_SW2_29,  CS4_SW2_29,  CS6_SW2_29},
    {1,  CS5_SW3_29,  CS4_SW3_29,  CS6_SW3_29},
    {1,  CS5_SW4_29,  CS4_SW4_29,  CS6_SW4_29},
    {1,  CS5_SW5_29,  CS4_SW5_29,  CS6_SW5_29},
    {1,  CS5_SW6_29,  CS4_SW6_29,  CS6_SW6_29},
    {1,  CS5_SW7_29,  CS4_SW7_29,  CS6_SW7_29},
    {1,  CS5_SW8_29,  CS4_SW8_29,  CS6_SW8_29},
    {1,  CS5_SW9_29,  CS4_SW9_29,  CS6_SW9_29},

    {1,  CS2_SW1_29,  CS1_SW1_29,  CS3_SW1_29},
    {1,  CS2_SW2_29,  CS1_SW2_29,  CS3_SW2_29},
    {1,  CS2_SW3_29,  CS1_SW3_29,  CS3_SW3_29},
    {1,  CS2_SW4_29,  CS1_SW4_29,  CS3_SW4_29},
    {1,  CS2_SW5_29,  CS1_SW5_29,  CS3_SW5_29},
    {1,  CS2_SW6_29,  CS1_SW6_29,  CS3_SW6_29},
    {1,  CS2_SW7_29,  CS1_SW7_29,  CS3_SW7_29},
    {1,  CS2_SW8_29,  CS1_SW8_29,  CS3_SW8_29},
    {1,  CS2_SW9_29,  CS1_SW9_29,  CS3_SW9_29},
    
    #if 0
    // 16 leds
    {2, C1_1,   C3_2,   C4_2},
    {2, C1_2,   C2_2,   C4_3},
    {2, C1_3,   C2_3,   C3_3},
    {2, C1_4,   C2_4,   C3_4},
    {2, C1_5,   C2_5,   C3_5},
    {2, C1_6,   C2_6,   C3_6},
    {2, C1_7,   C2_7,   C3_7},
    {2, C1_8,   C2_8,   C3_8},

    {2, C9_1,   C8_1,   C7_1},
    {2, C9_2,   C8_2,   C7_2},
    {2, C9_3,   C8_3,   C7_3},
    {2, C9_4,   C8_4,   C7_4},
    {2, C9_5,   C8_5,   C7_5},
    {2, C9_6,   C8_6,   C7_6},
    {2, C9_7,   C8_7,   C6_6},
    {2, C9_8,   C7_7,   C6_7},
    #endif
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3729, 0x68, 0, 0, 45},
    {RGB_DRIVER_IS31FL3729, 0x6E, 1, 45, 45},
    //{RGB_DRIVER_IS31FL3731, 0xE8, 0, 90, 16},
};

rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM] = {
    {0, 0, 90},
};

//rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
//    {1,  90, 16},
//};

#include "rgb_matrix_stub.h"

led_config_t g_led_config = {
#if META_RGB_V1
    {
        {45,    46, 47,    48,      49,     50, 51,    52,      53,     54,     55, 56,     57, NO_LED, NO_LED, 58},
        { 0,     1,  2,     3,       4,      5,  6,     7,       8,     59,     60, 61,     62,     77,     70, 71},
        { 9,    10, 11,     12,     13,     14, 15,     16,     17,     63,     64, 65,     66,     67,     68, 69},
        {18,    19, 20,     21,     22,     23, 24,     25,     26,     72,     73, 74, NO_LED,     76,     77, 78},
        {27,NO_LED, 29,     30,     31,     32, 33,     34,     35,     81,     82, 83,     84, NO_LED,     87, 80},
        {36,    37, 38, NO_LED, NO_LED, NO_LED, 40, NO_LED, NO_LED, NO_LED, NO_LED, 42,     43,     86,     88, 89},
    },
    {
        {0,16},{14,16},{28,16},{42,16},{56,16}, {70,16}, {84,16}, {98,16},{112,16},
        {4,28},{21,28},{35,28},{49,28},{63,28}, {77,28}, {91,28},{105,28},{119,28},
        {4,40},{25,40},{39,40},{53,40},{67,40}, {81,40}, {95,40},{109,40},{123,40},
        {9,52},{18,51},{32,52},{46,52},{60,52}, {74,52}, {88,52},{102,52},{116,52},
        {2,64},{19,64},{37,64},{53,61},{95,64},{136,61},{152,64},{170,64},{177,52},

           {0,1},  {17,1},  {31,1},  {45,1},  {59,1},  {77,1},  {91,1}, {105,1}, {119,1},
         {136,1}, {150,1}, {164,1}, {178,1}, {224,0},{126,16},{140,16},{154,16},{168,16},
        {133,28},{147,28},{161,28},{175,28},{192,27},{210,28},{224,28},{210,16},{224,16},
        {137,40},{151,40},{165,40},{179,39},{185,39},{189,16},{210,40},{224,40},{224,52},
        {130,52},{144,52},{158,52},{184,52},{196,52},{196,64},{210,52},{210,64},{224,64},
    },
    {
        4, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 4, 4, 4, 1, 1, 4,

        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 1, 1, 1, 1,
        4, 4, 4, 4, 1, 1, 1, 1, 1,
        4, 4, 4, 4, 4, 1, 1, 1, 1,
    },
#else
    /*{
        {45,  46, 47,    48,      49,     50, 51,    52,      53,     54,     55, 56,     57, NO_LED, NO_LED, 58},
        { 0,   1,  2,     3,       4,      5,  6,     7,       8,     59,     60, 61,     62,     77,     70, 71},
        { 9,  10, 11,     12,     13,     14, 15,     16,     17,     63,     64, 65,     66,     67,     68, 69},
        {18,  19, 20,     21,     22,     23, 24,     25,     26,     72,     73, 74, NO_LED,     76,     77, 78},
        {27,  28, 29,     30,     31,     32, 33,     34,     81,     82,     83, 84,     85, NO_LED,     87, 80},
        {35,  36, 37, NO_LED, NO_LED, NO_LED, 39, NO_LED, NO_LED, NO_LED, NO_LED, 41,     42,     86,     88, 89},
    },
    */
    {
        {45,  46, 47,    48,      49,     50, 51,    52,      53,     54,     55, 56,     57, NO_LED, NO_LED, 58},
        { 0,   1,  2,     3,       4,      5,  6,     7,       8,     59,     60, 61,     62,     77,     70, 71},
        { 9,  10, 11,     12,     13,     14, 15,     16,     17,     63,     64, 65,     66,     67,     68, 69},
        {18,  19, 20,     21,     22,     23, 24,     25,     26,     72,     73, 74, NO_LED,     76,     78, 79},
        {27,  28, 29,     30,     31,     32, 33,     34,     35,     81,     82, 83,     84,     NO_LED, 87, 80},
        {36,  37, 38, NO_LED, NO_LED, NO_LED, 40, NO_LED, NO_LED, NO_LED, NO_LED, 42,     43,     86,     88, 89},
    },
    {
        {0,16},{14,16},{28,16},{42,16},{56,16}, {70,16}, {84,16}, {98,16},{112,16},
        {4,28},{21,28},{35,28},{49,28},{63,28}, {77,28}, {91,28},{105,28},{119,28},
        {4,40},{25,40},{39,40},{53,40},{67,40}, {81,40}, {95,40},{109,40},{123,40},
        {9,52},{18,51},{32,52},{46,52},{60,52}, {74,52}, {88,52},{102,52},{116,52},
        {2,64},{19,64},{37,64},{53,61},{95,64},{136,61},{152,64},{170,64},{177,52},

           {0,1},  {17,1},  {31,1},  {45,1},  {59,1},  {77,1},  {91,1}, {105,1}, {119,1},
         {136,1}, {150,1}, {164,1}, {178,1}, {224,0},{126,16},{140,16},{154,16},{168,16},
        {133,28},{147,28},{161,28},{175,28},{192,27},{210,28},{224,28},{210,16},{224,16},
        {137,40},{151,40},{165,40},{179,39},{185,39},{189,16},{210,40},{224,40},{224,52},
        {130,52},{144,52},{158,52},{184,52},{196,52},{196,64},{210,52},{210,64},{224,64},
    },
    {
        4, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 4, 4, 4, 1, 1, 4,

        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 1, 1, 1, 1,
        4, 4, 4, 4, 1, 1, 1, 1, 1,
        4, 4, 4, 4, 4, 1, 1, 1, 1,
    },
#endif
};

#include "host.h"
#include "led.h"
#include "rgb_matrix.h"

#define CAPS_LED_INDEX 18

void rgb_led_pre_flush(void)
{
    if (!rgb_matrix_is_enabled()) {
        if (host_keyboard_led_state().caps_lock) {
            rgb_matrix_set_rgb_stub(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
        } else {
            rgb_matrix_set_rgb_stub(0, CAPS_LED_INDEX, 0, 0, 0);
        }
    }
}

#endif

/**
 * @file me_ec.c
 */

#include "me_ec.h"
#include "amk_printf.h"
#include "ec_matrix.h"

ec_matrix_t ec_matrix = {
    {
        { 
            {56,1844,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1907,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {59,1542,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1594,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1669,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {59,1668,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1581,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {59,1636,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1737,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*60*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1673,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1458,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1737,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1587,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1850,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {58,1658,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1927,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1785,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1848,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1921,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1766,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1864,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1734,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1867,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1818,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1728,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1682,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1758,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1638,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1802,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1784,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {59,1798,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1952,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1828,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1925,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1744,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1932,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1812,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,2158,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1978,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1933,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1609,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1618,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1609,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,1198,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1639,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1632,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {57,1660,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1819,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1934,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1911,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1795,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1948,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1779,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1826,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1880,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1938,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1755,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1604,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1767,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1759,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1535,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1732,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {58,2017,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1946,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,2060,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,2178,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,2152,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1544,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1928,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1801,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,2125,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {58,1984,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1907,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1829,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {54,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1626,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1694,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1813,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {57,1875,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1453,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {54,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1320,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1725,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*60*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {52,317,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*60*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {55,EC_INVALID_MAX/*61*/,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1619,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {56,1760,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {57,1687,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
    },
    //EC_AUTO_MODE,
    //EC_CALIBRATE_MODE,
    EC_DEFAULT_MODE,
};

void matrix_init_kb(void)
{
    ec_matrix_init(&ec_matrix);

    #ifdef RGB_EN_PIN
    gpio_set_output_pushpull(RGB_EN_PIN);
    gpio_write_pin(RGB_EN_PIN, 1);
    #endif

    //power on switch board
    #ifdef POWER_PIN
    gpio_set_output_pushpull(POWER_PIN);
    gpio_write_pin(POWER_PIN, 1);

    wait_ms(100);
    #endif

    // initialize opamp and capacitor discharge
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 1);
}

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "is31fl3236.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
    {1, OUT_34, OUT_35, OUT_36},
    {1, OUT_31, OUT_32, OUT_33},
    {1, OUT_28, OUT_29, OUT_30},
    {1, OUT_25, OUT_26, OUT_27},
    {1, OUT_22, OUT_23, OUT_24},
    {1, OUT_19, OUT_20, OUT_21},
    {1, OUT_16, OUT_17, OUT_18},
    {1, OUT_13, OUT_14, OUT_15},
    {1, OUT_10, OUT_11, OUT_12},
    {1, OUT_7, OUT_8, OUT_9},
    {1, OUT_4, OUT_5, OUT_6},
    {1, OUT_1, OUT_2, OUT_3},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 0, 4},
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 4, 12},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 4},
    {1, 4, 12},
};
#endif

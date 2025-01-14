/**
 * @file rgb_driver.c
 * @breif rgb led driver implementation
 */

#include <stddef.h>
#include "rgb_driver.h"
#include "wait.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#include "ws2812.h"
#include "aw9523b.h"
#include "aw9106b.h"
#include "is31fl3731.h"
#include "is31fl3236.h"
#include "is31fl3733.h"
#include "is31fl3741.h"
#include "is31fl3746.h"
#include "is31fl3729.h"
#include "is31fl3193.h"

#ifndef RGB_DRIVER_DEBUG
#define RGB_DRIVER_DEBUG 1
#endif

#if RGB_DRIVER_DEBUG
#define rgb_driver_debug  amk_printf
#else
#define rgb_driver_debug(...)
#endif

rgb_driver_t rgb_drivers[RGB_DEVICE_NUM];

#define RGB_DRIVER_DEF(name) \
static void rd_init##name(rgb_driver_t *driver) \
{ \
    driver->data = name##_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num); \
} \
\
static void rd_uninit##name(rgb_driver_t *driver) \
{ \
    i2c_led_t *is31 = (i2c_led_t *)driver->data; \
    name##_uninit(is31);\
} \
\
static void rd_set_color_rgb##name(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue) \
{ \
    i2c_led_t *is31 = (i2c_led_t *)driver->data; \
    name##_set_color(is31, index, red, green, blue); \
} \
\
static void rd_set_color##name(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val) \
{ \
    amk_hsv_t hsv = {hue, sat, val}; \
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv); \
    i2c_led_t *is31 = (i2c_led_t *)driver->data; \
    name##_set_color(is31, index, rgb.r, rgb.g, rgb.b); \
} \
\
static void rd_set_color_all_rgb##name(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue) \
{ \
    i2c_led_t *is31 = (i2c_led_t *)driver->data; \
    name##_set_color_all(is31, red, green, blue); \
} \
\
static void rd_set_color_all##name(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val) \
{ \
    amk_hsv_t hsv = {hue, sat, val}; \
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv); \
    i2c_led_t *is31 = (i2c_led_t *)driver->data; \
    name##_set_color_all(is31, rgb.r, rgb.g, rgb.b); \
} \
\
static bool rd_flush##name(rgb_driver_t *driver) \
{ \
    i2c_led_t *is31 = (i2c_led_t *)driver->data; \
    return name##_update_buffers(is31); \
} \

#define RGB_DRIVER_INIT(name) \
            driver->init = rd_init##name; \
            driver->set_color = rd_set_color##name; \
            driver->set_color_all = rd_set_color_all##name; \
            driver->set_color_rgb = rd_set_color_rgb##name; \
            driver->set_color_all_rgb = rd_set_color_all_rgb##name; \
            driver->flush = rd_flush##name; \
            driver->uninit = rd_uninit##name;

#ifdef USE_WS2812
static void rd_ws2812_init(rgb_driver_t *driver)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif

#ifdef WS2812_LED_PIN
    pin_t p = WS2812_LED_PIN;
    ws2812_init(p);
    driver->data = NULL;
#endif
}

static void rd_ws2812_uninit(rgb_driver_t *driver)
{
#ifdef WS2812_LED_PIN
    pin_t p = WS2812_LED_PIN;
    ws2812_uninit(p);
#endif
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
}

static void rd_ws2812_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef WS2812_LED_PIN
    ws2812_set_color(index, red, green, blue);
#endif
}
static void rd_ws2812_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
#ifdef WS2812_LED_PIN
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    ws2812_set_color(index, rgb.r, rgb.g, rgb.b);
#endif
}

static void rd_ws2812_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef WS2812_LED_PIN
    ws2812_set_color_all(red, green, blue);
#endif
}

static void rd_ws2812_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
#ifdef WS2812_LED_PIN
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    ws2812_set_color_all(rgb.r, rgb.g, rgb.b);
#endif
}

static bool rd_ws2812_flush(rgb_driver_t *driver)
{
#ifdef WS2812_LED_PIN
    pin_t p = WS2812_LED_PIN;
    ws2812_update_buffers(p);
#endif
    return true;
}
#endif

#ifdef USE_AW9523B
static void rd_aw9523b_init(rgb_driver_t *driver)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    driver->data = aw9523b_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
    rgb_driver_debug("AW9523B init\n");
}

static void rd_aw9523b_uninit(rgb_driver_t *driver)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9523b_uninit(awinic);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
    rgb_driver_debug("AW9523B uninit\n");
}

static void rd_aw9523b_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9523b_set_color(awinic, index, red, green, blue);
}

static void rd_aw9523b_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9523b_set_color(awinic, index, rgb.r, rgb.g, rgb.b);
}

static void rd_aw9523b_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9523b_set_color_all(awinic, red, green, blue);
}

static void rd_aw9523b_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9523b_set_color_all(awinic, rgb.r, rgb.g, rgb.b);
}

static bool rd_aw9523b_flush(rgb_driver_t *driver)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9523b_update_buffers(awinic);
    return true;
}
#endif

#ifdef USE_AW9106B
static void rd_aw9106b_init(rgb_driver_t *driver)
{
#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    driver->data = aw9106b_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
    rgb_driver_debug("AW9106B init\n");
}

static void rd_aw9106b_uninit(rgb_driver_t *driver)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9106b_uninit(awinic);
#ifdef RGBLIGHT_EN_PIN
    gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
    rgb_driver_debug("AW106B uninit\n");
}

static void rd_aw9106b_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;

    aw9106b_set_color(awinic, index, red, green, blue);
}

static void rd_aw9106b_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *awinic = (i2c_led_t *)driver->data;

    aw9106b_set_color(awinic, index, rgb.r, rgb.g, rgb.b);
}

static void rd_aw9106b_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9106b_set_color_all(awinic, red, green, blue);
}

static void rd_aw9106b_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9106b_set_color_all(awinic, rgb.r, rgb.g, rgb.b);
}

static bool rd_aw9106b_flush(rgb_driver_t *driver)
{
    i2c_led_t *awinic = (i2c_led_t *)driver->data;
    aw9106b_update_buffers(awinic);
    return true;
}
#endif

#ifdef USE_3731
static void rd_3731_init(rgb_driver_t *driver)
{
    driver->data = is31fl3731_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
    rgb_driver_debug("IS31FL3731 inited\n");
}

static void rd_3731_uninit(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3731_uninit(is31);
}

static void rd_3731_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t blue, uint8_t green)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3731_set_color(is31, index, red, green, blue);
}

static void rd_3731_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3731_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3731_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t blue, uint8_t green)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3731_set_color_all(is31, red, green, blue);
}

static void rd_3731_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3731_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static bool rd_3731_flush(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    return is31fl3731_update_buffers(is31);
}
#endif

#ifdef USE_3733
static void rd_3733_init(rgb_driver_t *driver)
{
    driver->data = is31fl3733_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
}

static void rd_3733_uninit(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3733_uninit(is31);
}

static void rd_3733_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t blue, uint8_t green)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3733_set_color(is31, index, red, green, blue);
}

static void rd_3733_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3733_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3733_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3733_set_color_all(is31, red, green, blue);
}

static void rd_3733_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3733_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static bool rd_3733_flush(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3733_update_buffers(is31);
    return true;
}
#endif

#ifdef USE_3236

RGB_DRIVER_DEF(is31fl3236)

#if 0
static void rd_3236_init(rgb_driver_t *driver)
{
    driver->data = is31fl3236_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
}

static void rd_3236_uninit(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3236_uninit(is31);
}

static void rd_3236_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3236_set_color(is31, index, red, green, blue);
}

static void rd_3236_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3236_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3236_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3236_set_color_all(is31, red, green, blue);
}

static void rd_3236_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3236_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static bool rd_3236_flush(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    return is31fl3236_update_buffers(is31);
}
#endif
#endif

#ifdef USE_3741
static void rd_3741_init(rgb_driver_t *driver)
{
    driver->data = is31fl3741_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
}

static void rd_3741_uninit(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3741_uninit(is31);
}

static void rd_3741_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3741_set_color(is31, index, red, green, blue);
}

static void rd_3741_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3741_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3741_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3741_set_color_all(is31, red, green, blue);
}

static void rd_3741_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3741_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static bool rd_3741_flush(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3741_update_buffers(is31);
    return true;
}
#endif

#ifdef USE_3746
static void rd_3746_init(rgb_driver_t *driver)
{
    driver->data = is31fl3746_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
}

static void rd_3746_uninit(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3746_uninit(is31);
}

static void rd_3746_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3746_set_color(is31, index, red, green, blue);
}

static void rd_3746_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3746_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3746_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3746_set_color_all(is31, red, green, blue);
}

static void rd_3746_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3746_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static bool rd_3746_flush(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3746_update_buffers(is31);
    return true;
}
#endif

#ifdef USE_3729
static void rd_3729_init(rgb_driver_t *driver)
{
    driver->data = is31fl3729_init(driver->device->addr, driver->device->index, driver->device->led_start, driver->device->led_num);
}

static void rd_3729_uninit(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3729_uninit(is31);
}

static void rd_3729_set_color_rgb(rgb_driver_t *driver, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3729_set_color(is31, index, red, green, blue);
}

static void rd_3729_set_color(rgb_driver_t *driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3729_set_color(is31, index, rgb.r, rgb.g, rgb.b);
}

static void rd_3729_set_color_all_rgb(rgb_driver_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3729_set_color_all(is31, red, green, blue);
}

static void rd_3729_set_color_all(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val)
{
    amk_hsv_t hsv = {hue, sat, val};
    amk_rgb_t rgb = hsv_to_rgb_stub(hsv);
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    is31fl3729_set_color_all(is31, rgb.r, rgb.g, rgb.b);
}

static bool rd_3729_flush(rgb_driver_t *driver)
{
    i2c_led_t *is31 = (i2c_led_t *)driver->data;
    return is31fl3729_update_buffers(is31);
}
#endif

#ifdef USE_3193
RGB_DRIVER_DEF(is31fl3193)
#endif

bool rgb_driver_available(rgb_driver_type_t type)
{
    switch(type) {
        case RGB_DRIVER_WS2812:
            return true;    // always available
#ifdef USE_AW9523B
        case RGB_DRIVER_AW9523B:
            return aw9523b_available(AW9523B_ADDR);
#endif
        case RGB_DRIVER_AW9106B:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3731:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3733:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3236:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3741:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3746:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3729:
            return true;    // TODO
        case RGB_DRIVER_IS31FL3193:
            return true;    // TODO
        default:
            break;
    }
    return true;
}

rgb_driver_t* rgb_driver_create(rgb_device_t *device, uint8_t index)
{
    rgb_driver_t *driver = &rgb_drivers[index];
    driver->device = device;
    driver->data = NULL;
    switch(device->type) {
#ifdef USE_WS2812
        case RGB_DRIVER_WS2812:
            driver->init = rd_ws2812_init;
            driver->set_color = rd_ws2812_set_color;
            driver->set_color_all = rd_ws2812_set_color_all;
            driver->set_color_rgb = rd_ws2812_set_color_rgb;
            driver->set_color_all_rgb = rd_ws2812_set_color_all_rgb;
            driver->flush = rd_ws2812_flush;
            driver->uninit = rd_ws2812_uninit;
            break;
#endif
#ifdef USE_AW9523B
        case RGB_DRIVER_AW9523B:
            driver->init = rd_aw9523b_init;
            driver->set_color = rd_aw9523b_set_color;
            driver->set_color_all = rd_aw9523b_set_color_all;
            driver->set_color_rgb = rd_aw9523b_set_color_rgb;
            driver->set_color_all_rgb = rd_aw9523b_set_color_all_rgb;
            driver->flush = rd_aw9523b_flush;
            driver->uninit = rd_aw9523b_uninit;
            break;
#endif
#ifdef USE_AW9106B
        case RGB_DRIVER_AW9106B:
            driver->init = rd_aw9106b_init;
            driver->set_color = rd_aw9106b_set_color;
            driver->set_color_all = rd_aw9106b_set_color_all;
            driver->set_color_rgb = rd_aw9106b_set_color_rgb;
            driver->set_color_all_rgb = rd_aw9106b_set_color_all_rgb;
            driver->flush = rd_aw9106b_flush;
            driver->uninit = rd_aw9106b_uninit;
            break;
#endif
#ifdef USE_3731
        case RGB_DRIVER_IS31FL3731:
            driver->init = rd_3731_init;
            driver->set_color = rd_3731_set_color;
            driver->set_color_all = rd_3731_set_color_all;
            driver->set_color_rgb = rd_3731_set_color_rgb;
            driver->set_color_all_rgb = rd_3731_set_color_all_rgb;
            driver->flush = rd_3731_flush;
            driver->uninit = rd_3731_uninit;
            break;
#endif
#ifdef USE_3733
        case RGB_DRIVER_IS31FL3733:
            driver->init = rd_3733_init;
            driver->set_color = rd_3733_set_color;
            driver->set_color_all = rd_3733_set_color_all;
            driver->set_color_rgb = rd_3733_set_color_rgb;
            driver->set_color_all_rgb = rd_3733_set_color_all_rgb;
            driver->flush = rd_3733_flush;
            driver->uninit = rd_3733_uninit;
            break;
#endif
#ifdef USE_3236
        case RGB_DRIVER_IS31FL3236:
        RGB_DRIVER_INIT(is31fl3236)
        #if 0
            driver->init = rd_3236_init;
            driver->set_color = rd_3236_set_color;
            driver->set_color_all = rd_3236_set_color_all;
            driver->set_color_rgb = rd_3236_set_color_rgb;
            driver->set_color_all_rgb = rd_3236_set_color_all_rgb;
            driver->flush = rd_3236_flush;
            driver->uninit = rd_3236_uninit;
        #endif
            break;
#endif
#ifdef USE_3193
        case RGB_DRIVER_IS31FL3193:
        RGB_DRIVER_INIT(is31fl3193)
            break;
#endif
#ifdef USE_3741
        case RGB_DRIVER_IS31FL3741:
            driver->init = rd_3741_init;
            driver->set_color = rd_3741_set_color;
            driver->set_color_all = rd_3741_set_color_all;
            driver->set_color_rgb = rd_3741_set_color_rgb;
            driver->set_color_all_rgb = rd_3741_set_color_all_rgb;
            driver->flush = rd_3741_flush;
            driver->uninit = rd_3741_uninit;
            break;
#endif
#ifdef USE_3746
        case RGB_DRIVER_IS31FL3746:
            driver->init = rd_3746_init;
            driver->set_color = rd_3746_set_color;
            driver->set_color_all = rd_3746_set_color_all;
            driver->set_color_rgb = rd_3746_set_color_rgb;
            driver->set_color_all_rgb = rd_3746_set_color_all_rgb;
            driver->flush = rd_3746_flush;
            driver->uninit = rd_3746_uninit;
            break;
#endif
#ifdef USE_3729
        case RGB_DRIVER_IS31FL3729:
            driver->init = rd_3729_init;
            driver->set_color = rd_3729_set_color;
            driver->set_color_all = rd_3729_set_color_all;
            driver->set_color_rgb = rd_3729_set_color_rgb;
            driver->set_color_all_rgb = rd_3729_set_color_all_rgb;
            driver->flush = rd_3729_flush;
            driver->uninit = rd_3729_uninit;
            break;
#endif
        default:
            return NULL;
    }

    return driver;
}

void rgb_driver_uninit(void)
{
	for (int i = 0; i < RGB_DEVICE_NUM; i++) {
        rgb_driver_t *driver = &rgb_drivers[i];
        driver->uninit(driver);
	}
}

void rgb_driver_init(void)
{
	for (int i = 0; i < RGB_DEVICE_NUM; i++) {
        rgb_driver_t *driver = rgb_driver_create(&g_rgb_devices[i], i);
        driver->init(driver);
	}
} 

rgb_driver_t *rgb_driver_get(uint8_t index)
{
    rgb_driver_t *driver = NULL;
    if (index < RGB_DEVICE_NUM) {
        driver = &rgb_drivers[index];
    }
    return driver;
}

void rgb_driver_prepare_sleep(void)
{
    //TODO
}
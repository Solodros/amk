/**
 * @file ws2812.c
 */

#ifdef WS2812_LED_NUM
#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "ws2812_nrf52.c"
#else
#ifndef WS2812_USE_GPIO
#   include "ws2812_stm32_pwm.c"
#else
#   include "ws2812_stm32_gpio.c"
#endif
#endif
#endif

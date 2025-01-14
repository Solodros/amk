/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x4D45      // ME
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         ME EC

#define MATRIX_ROWS     6
#define MATRIX_COLS     16
#define L_MASK          0x08
#define R_MASK          0x10
#define MATRIX_ROW_PINS {B6, B5, B4, A15, B15, B1}
#define MATRIX_COL_PINS {L_MASK|3, L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|4, L_MASK|6, L_MASK|7, L_MASK|5, R_MASK|3, R_MASK|0, R_MASK|1, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7, R_MASK|5}

#define DISCHARGE_WAIT_PRE  10
#define DISCHARGE_WAIT_POST 10
#define SCAN_DELAY          100

#define CHARGE_WAIT     2
#define EC_TH_LOW       600
#define EC_TH_HIGH      800

#define LEFT_EN_PIN     B10
#define RIGHT_EN_PIN    B2
#define COL_A_PIN       B14
#define COL_B_PIN       B13
#define COL_C_PIN       B12

#define OPA_EN_PIN      A5
#define DISCHARGE_PIN   A6

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_2
#define KEY_IN_PIN      GPIO_PIN_2
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        1024

#define ADC_CHANNEL_COUNT   1
#define ADC_CHANNELS        {KEY_IN_CHANNEL}
#define ADC_PINS            {A2}

#define CAPS_LED_PIN    A8
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  B7


#if 0
#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_1
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC1
#define PWM_DMA_INSTANCE    DMA1_Stream0
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream0_IRQHandler
#endif

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2
#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler
#define PWM_PIN             GPIO_PIN_7

#define RGB_LED_NUM     (4+12)
#define RGB_DEVICE_NUM  2
#define RGB_SEGMENT_NUM 2

#define USE_I2C1
#define USE_WS2812
#define USE_3236

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        5

#define VECT_TAB_OFFSET 0x20000
#define DWT_DELAY
#define SUSPEND_RESET
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VIAL_KEYBOARD_UID {0x64, 0x20, 0x09, 0xF6, 0x28, 0x2C, 0x68, 0xD5}

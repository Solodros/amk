/**
 * @file config.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x4A4B   // JK
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Faukwaa

// key matrix
#define MATRIX_ROWS         5
#define MATRIX_COLS         16
#define MATRIX_ROW_PINS     {A7, A6, B13, B12, B0}
#define MATRIX_COL_PINS     {C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15}

#define USE_I2C1
#define I2C1_SCL_PIN    B6
#define I2C1_SDA_PIN    B7

#define USE_3729
#define IS31FL3729_NUM          2
#define IS31FL3729_SCALE_DEFAULT    0x7F

#define USE_3236
#define IS31FL3236_NUM      4
#define IS31FL3236_LED_NUM  19
#define RGB_INDICATOR_NUM   1
//#define IS31FL3236_LED_NUM  21

#define RGB_MATRIX_LED_NUM      74
#define RGB_MATRIX_LED_COUNT    RGB_MATRIX_LED_NUM

#define RGB_LED_NUM         (IS31FL3236_LED_NUM+RGB_MATRIX_LED_NUM)
#define RGB_DEVICE_NUM      6
#define RGB_SEGMENT_NUM     5
#define RGB_MATRIX_NUM      1

#define DEBOUNCE            5
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000
#define SUSPEND_RESET
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

// vial 
#define VIAL_KEYBOARD_UID {0xA3, 0xE5, 0xC9, 0x6E, 0x96, 0xBA, 0x27, 0x49}

#define USE_HS_USB
//#define RGB_FLUSH_ASYNC

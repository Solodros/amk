/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x6501   // M65 with screen
#define DEVICE_VER 0x0001
#define MANUFACTURER Matrix Lab
#define PRODUCT Vita

// key matrix
#define MATRIX_ROWS     5
#define MATRIX_COLS     15

#define MATRIX_ROW_PINS { A1, C2, C1, C4, B10}
#define MATRIX_COL_PINS { C9, A8, C8, C7, C6, B4, D2, C12, C11, C10, A15, B11, B2, B1, B0}

#define CAPS_LED_PIN    A10

#define USE_SPI1
#define SCREEN_NUM      1
#define SCREEN_0_PWR    C0
#define SCREEN_0_PWR_EN 0
#define SCREEN_0_CS     A2
#define SCREEN_0_RESET  A3
#define SCREEN_0_DC     A4
#define ST7735_SPI_ID   SPI_INSTANCE_1

#define USE_SPI2
#define FLASH_CS        B12
#define W25QXX_SPI_ID   SPI_INSTANCE_2

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VECT_TAB_OFFSET 0x20000

#define USE_I2C1
#define I2C1_SCL_PIN    B8
#define I2C1_SDA_PIN    B9

#define USE_WS2812
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  A0
#define PWM_TIM_PERIOD  134
#define PWM_TIM htim2
#define PWM_TIM_CHANNEL TIM_CHANNEL_1

#define RGB_LED_NUM     4
#define RGB_DEVICE_NUM  1
#define RGB_SEGMENT_NUM 1

#define VIAL_KEYBOARD_UID {0x01, 0x90, 0x17, 0x3A, 0xEC, 0xEE, 0x2B, 0x85}

#define DWT_DELAY
#define SUSPEND_RESET
//#define WDT_ENABLE
//#define FAULT_BREAK
//#define TYPING_SPEED

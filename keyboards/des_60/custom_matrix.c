/**
 * @file custom_matrix.c
 */

#include "matrix.h"
#include "ec_matrix.h"
#include "wait.h"

#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef CUSTOM_MATRIX_DEBUG
#define CUSTOM_MATRIX_DEBUG 0
#endif

#if CUSTOM_MATRIX_DEBUG
#define custom_matrix_debug  amk_printf
#else
#define custom_matrix_debug(...)
#endif

#define CLEANUP_DELAY  200
#define ROW_WAIT       50

//#define COL_A_MASK  0x01
//#define COL_B_MASK  0x02
//#define COL_C_MASK  0x04
//#define L_MASK      0x08
//#define R_MASK      0x10

static void AdcConfig(void);
static void AdcClockConfig(void);
static void AdcInitConfig(void);
static void AdcChannelConfig(void);

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
//static pin_t custom_col_pins[] = MATRIX_COL_PINS;

void matrix_init_custom(void)
{
    #ifdef CAPS_LED_PIN
    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, 0);
    #endif

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

    // initialize row pins
    gpio_set_output_pushpull(ROW_1_PIN);
    gpio_write_pin(ROW_1_PIN, 0);
    gpio_set_output_pushpull(ROW_2_PIN);
    gpio_write_pin(ROW_2_PIN, 0);
    gpio_set_output_pushpull(ROW_3_PIN);
    gpio_write_pin(ROW_3_PIN, 0);
    gpio_set_output_pushpull(ROW_4_PIN);
    gpio_write_pin(ROW_4_PIN, 0);

    // initialize col pins
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 1);
    gpio_set_output_pushpull(RIGHT_EN_PIN);
    gpio_write_pin(RIGHT_EN_PIN, 1);

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);

    // initialize opamp and capacitor discharge
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);

    AdcConfig();
}

//extern ADC_HandleTypeDef hadc1;

static uint32_t adc_read(void)
{
    static uint16_t data = 0;
    if (Ok != ADC_PollingSa(M4_ADC1, &data, 1, 10)) {
        custom_matrix_debug("key sense failed\n");
    } else {
    }

#if 0
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, 1);
        if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
        {
            data = HAL_ADC_GetValue(&hadc1);
        } else {
        }
        HAL_ADC_Stop(&hadc1);
    }
#endif

    return data;

}

static bool sense_key(pin_t row)
{
    bool key_down = false;
    //gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_set_input_floating(DISCHARGE_PIN);

    wait_us(2);
    gpio_write_pin(row, 1);
    wait_us(2);
    uint32_t data = adc_read();
    if (data > 512) {
        key_down = true;
        //custom_matrix_debug("key down: 0x%lx, data=%d\n", row, data);
        amk_printf("key down: 0x%lx, data=%d\n", row, data);
    } else {
        //custom_matrix_debug("read key: data=%d\n", data);
        amk_printf("read key: data=%d\n", data);
    }

    // clean up
    gpio_write_pin(row, 0);
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(CLEANUP_DELAY);
    return key_down;
}

#define SCAN_ONE 1
#if SCAN_ONE
bool scan_one(matrix_row_t *raw)
{
    bool changed = false;
    uint8_t col = 0;

    gpio_write_pin(RIGHT_EN_PIN, 1);
    gpio_write_pin(COL_C_PIN, 0);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_write_pin(LEFT_EN_PIN,  0);

    for (int row = 0; row < 1 /*MATRIX_ROWS*/; row++) {
        matrix_row_t last_row_value    = raw[row];
        matrix_row_t current_row_value = last_row_value;

        if (sense_key(custom_row_pins[row])) {
            current_row_value |= (1 << col);
        } else {
            current_row_value &= ~(1 << col);
        }

        if (last_row_value != current_row_value) {
            raw[row] = current_row_value;
            changed = true;
        }
    }

    return changed;
}
#endif

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
#if SCAN_ONE
    changed = scan_one(raw);
#else
    for (int col = 0; col < MATRIX_COLS; col++) {

        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);

        if (custom_col_pins[col]&L_MASK) {
            gpio_write_pin(LEFT_EN_PIN,  0);
        }
        if (custom_col_pins[col]&R_MASK) {
            gpio_write_pin(RIGHT_EN_PIN, 0);
        }

        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            if (sense_key(custom_row_pins[row])) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
        }
        gpio_write_pin(LEFT_EN_PIN,  1);
        gpio_write_pin(RIGHT_EN_PIN, 1);
    }
#endif

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}

static void AdcConfig(void)
{
    AdcClockConfig();
    AdcInitConfig();
    AdcChannelConfig();
}

static void AdcClockConfig(void)
{
    CLK_SetPeriClkSource(ClkPeriSrcPclk);
}

static void AdcInitConfig(void)
{
    stc_adc_init_t stcAdcInit;

    MEM_ZERO_STRUCT(stcAdcInit);

    stcAdcInit.enResolution = AdcResolution_10Bit;
    stcAdcInit.enDataAlign  = AdcDataAlign_Right;
    stcAdcInit.enAutoClear  = AdcClren_Disable;
    stcAdcInit.enScanMode   = AdcMode_SAOnce;
    /* 1. Enable ADC1. */
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1, Enable);
    /* 2. Initialize ADC1. */
    ADC_Init(M4_ADC1, &stcAdcInit);
}

static void AdcChannelConfig(void)
{
    stc_adc_ch_cfg_t stcChCfg;
    uint8_t au8Adc1SaSampTime[1] = {0x30};

    MEM_ZERO_STRUCT(stcChCfg);

    stcChCfg.u32Channel  = ADC1_CH0;
    stcChCfg.u8Sequence  = ADC_SEQ_A;
    stcChCfg.pu8SampTime = au8Adc1SaSampTime;

    /* 1. Set the ADC pin to analog mode. */
    stc_port_init_t stcPortInit;
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Ana;
    stcPortInit.enPullUp  = Disable;
    PORT_Init(KEY_IN_PORT, KEY_IN_PIN, &stcPortInit);

    /* 2. Add ADC channel. */
    ADC_AddAdcChannel(M4_ADC1, &stcChCfg);
}

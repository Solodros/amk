/**
 * @file ec_matrix.c
 */

#include "quantum.h"

#include "amk_gpio.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifndef CUSTOM_MATRIX_DEBUG
#define CUSTOM_MATRIX_DEBUG 1
#endif

#if CUSTOM_MATRIX_DEBUG
#define custom_matrix_debug  amk_printf
#else
#define custom_matrix_debug(...)
#endif

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04

/*
#define L_MASK      0x08
#define R_MASK      0x10
*/

#if 0
struct ec_mode_t{
    uint32_t low;
    uint32_t high;
};

#define EC_MODE_MAX 3
struct ec_mode_t ec_modes[EC_MODE_MAX] = {
    {1024, 1280},
    {864, 1024},
    {1280, 1444},
};

static uint32_t ec_mode_current = 0;

void ec_mode_iter(void)
{
    ec_mode_current = (ec_mode_current+1) % EC_MODE_MAX;
    custom_matrix_debug("ec mode changed to:%d\n", ec_mode_current);
    eeconfig_update_kb(ec_mode_current);
}
#endif

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS;

void matrix_init_custom(void)
{
    //ec_mode_current = eeconfig_read_kb();

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
    for (int i = 0; i < AMK_ARRAY_SIZE(custom_row_pins); i++) {
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 0);
    }

    // initialize col pins
#ifdef LEFT_EN_PIN
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 1);
#endif

#ifdef RIGHT_EN_PIN
    gpio_set_output_pushpull(RIGHT_EN_PIN);
    gpio_write_pin(RIGHT_EN_PIN, 1);
#endif

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);

    // initialize opamp and capacity discharge
#ifdef OPA_EN_PIN
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
#endif
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
}

extern ADC_HandleTypeDef hadc1;

static uint32_t adc_read(void)
{
    uint32_t data = 0;
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, 1);
        if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
        {
            data = HAL_ADC_GetValue(&hadc1);
        } else {
            custom_matrix_debug("key sense failed\n");
        }
        HAL_ADC_Stop(&hadc1);
    }

    return data;
}

static bool sense_key(pin_t row, bool on, bool* key)
{
    //bool key_down = false;
    gpio_set_input_floating(DISCHARGE_PIN);
    gpio_write_pin(row, 1);
    uint32_t data = adc_read();
    //if (data < 100 || data > 2800) return false;
    // press to release
    if (on) {
        if (data > EC_TH_LOW) {
//        if (data > ec_modes[ec_mode_current].low) {
            //key_down = true;
            *key = true;
        }
    } else {
        if (data > EC_TH_HIGH) {
 //       if (data > ec_modes[ec_mode_current].high) {
            //key_down = true;
            *key = true;
        }
    }
    if (*key) {
        custom_matrix_debug("key down: 0x%lx, data=%d\n", row, data);
        //amk_printf("key down: 0x%lx, data=%d\n", row, data);
    }

    // clean up
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(row, 0);
    wait_us(DISCHARGE_WAIT_POST);

    return true;
    //key_down;
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
#if SCAN_ONE
    changed = scan_one(raw);
#else
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(300);
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

            bool key = false;
            if (sense_key(custom_row_pins[row], (last_row_value&col), &key)) {
            //if (sense_key(custom_row_pins[row], (last_row_value&col))) {
                if (key) {
                    current_row_value |= (1 << col);
                } else {
                    current_row_value &= ~(1 << col);
                }
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

/*
 * pad_ec_keymap.c
 */

#include "pad_ec.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
         MO(1),  KC_ESC, KC_PSCR, KC_BSPC,
         KC_NUM, KC_PSLS, KC_PAST, KC_PMNS,
          KC_P7,   KC_P8,   KC_P9,
          KC_P4,   KC_P5,   KC_P6, KC_PPLS,
          KC_P1,   KC_P2,   KC_P3, 
          KC_P0,          KC_PDOT, KC_PENT),

    [1] = LAYOUT_default(
        QK_BOOT, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, 
        _______, _______, _______, _______,
        _______, _______, _______, 
        _______,          _______, _______),
};

/**
 * @file amk_store.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "amk_hal.h"
#include "eeprom.h"
#include "amk_store.h"
#include "amk_printf.h"

#define AMK_STORE_APC_DEFAULT   16
#define AMK_STORE_RT_DEFAULT    0 

void amk_store_set_apc(uint8_t row, uint8_t col, uint16_t apc)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*2 + AMK_STORE_APC_START);
    eeprom_write_byte(addr, apc >> 8);
    eeprom_write_byte(addr+1, apc & 0xFF);
}

uint16_t amk_store_get_apc(uint8_t row, uint8_t col)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*2 + AMK_STORE_APC_START);
    uint16_t high = eeprom_read_byte(addr);
    uint16_t low = eeprom_read_byte(addr+1);
    uint16_t apc = (high << 8) | low;
    if (apc == 0) {
        apc = AMK_STORE_APC_DEFAULT;
    }
    return apc;
}

void amk_store_set_rt(uint8_t row, uint8_t col, uint16_t rt)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*2 + AMK_STORE_RT_START);
    eeprom_write_byte(addr, rt >> 8);
    eeprom_write_byte(addr+1, rt& 0xFF);
}

uint16_t amk_store_get_rt(uint8_t row, uint8_t col)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*2 + AMK_STORE_RT_START);
    uint16_t high = eeprom_read_byte(addr);
    uint16_t low = eeprom_read_byte(addr+1);

    return (high << 8) | low;
}

void amk_store_set_dks(uint8_t row, uint8_t col, void* dks)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*sizeof(struct amk_dks) + AMK_STORE_DKS_START);
    struct amk_dks* p = (struct amk_dks*)dks;
    for (int i = 0; i < AMK_DKS_EVENT_MAX; i++) {
        eeprom_write_byte(addr++, p->events[i]);
    }
    for (int j = 0; j < AMK_DKS_KEY_MAX; j++) {
        uint8_t high = (uint8_t)(p->keys[j] >> 8);
        uint8_t low = (uint8_t)(p->keys[j] & 0xFF);
        eeprom_write_byte(addr++, high);
        eeprom_write_byte(addr++, low);
    }
}

void amk_store_get_dks(uint8_t row, uint8_t col, void* dks)
{
    uint8_t *addr = (uint8_t *) ((row*MATRIX_COLS + col)*sizeof(struct amk_dks) + AMK_STORE_DKS_START);
    struct amk_dks* p = (struct amk_dks*)dks;
    for (int i = 0; i < AMK_DKS_EVENT_MAX; i++) {
        p->events[i] = eeprom_read_byte(addr++);
    }
    for (int j = 0; j < AMK_DKS_KEY_MAX; j++) {
        uint16_t high = eeprom_read_byte(addr++);
        uint16_t low = eeprom_read_byte(addr++);

        p->keys[j] = (high << 8) | low;
    }
}

void amk_store_init(void)
{
    uint8_t dks[sizeof(struct amk_dks)] = {[0 ... sizeof(struct amk_dks)-1] = 0};
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            amk_store_set_apc(row, col, AMK_STORE_APC_DEFAULT);
            amk_store_set_rt(row, col, AMK_STORE_RT_DEFAULT);
            amk_store_set_dks(row, col, dks);
        }
    }
}

void eeconfig_init_custom(void)
{
    amk_printf("AMK STORE init: apc:%d, rt:%d, dks:%d-%d\n", AMK_STORE_APC_START, AMK_STORE_RT_START, AMK_STORE_DKS_START, AMK_STORE_DKS_END);

    amk_store_init();
}
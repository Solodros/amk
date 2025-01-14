/**
 * @file is31fl3729.c
 * @brief driver implementation for is31fl3729
 */

#include <string.h>
#include "amk_hal.h"
#include "is31fl3729.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

// addr in gnd: 0x68
// addr in vcc: 0x6E

#ifndef FL3729_DEBUG
#define FL3729_DEBUG 1
#endif

#if FL3729_DEBUG
#define fl3729_debug  amk_printf
#else
#define fl3729_debug(...)
#endif

#define PWM_BUFFER_SIZE     0x90
#define SCALE_BUFFER_SIZE   0x10

// registers
#define PWM_BASE_REG        0x01    // 0x01-0x8F
#define SCALE_BASE_REG      0x90    // 0x90-0x9F

#define CONFIG_REG          0xA0
#define GLOBAL_CURRENT_REG  0xA1
#define PD_PU_REG           0xB0
#define SS_REG              0xB1
#define PWM_FREQ_REG        0xB2
// open short register B3~C4
#define RESET_REG           0xCF

// pullup and pulldown register for SWx and CSy
#define PD_OFFSET           0x04
#define PD_NO               0x00
#define PD_0_5K_SW_OFF      0x01
#define PD_1K_SW_OFF        0x02
#define PD_2K_SW_OFF        0x03
#define PD_1K_ALL           0x04
#define PD_2K_ALL           0x05
#define PD_4K_ALL           0x06
#define PD_8K_ALL           0x07

#ifndef IS31FL3729_PD_DEFAULT
    #define IS31FL3729_PD_DEFAULT PD_8K_ALL
#endif

#define PU_OFFSET           0x00
#define PU_NO               0x00
#define PU_0_5K_CS_OFF      0x01
#define PU_1K_CS_OFF        0x02
#define PU_2K_CS_OFF        0x03
#define PU_1K_ALL           0x04
#define PU_2K_ALL           0x05
#define PU_4K_ALL           0x06
#define PU_8K_ALL           0x07

#ifndef IS31FL3729_PU_DEFAULT
    #define IS31FL3729_PU_DEFAULT PU_8K
#endif

// reset command
#define RESET_COMMAND       0xAE

#ifndef IS31FL3729_NUM
    #define IS31FL3729_NUM  1
#endif

#ifndef IS31FL3729_GLOBAL_CURRENT
    #define IS31FL3729_GLOBAL_CURRENT 0xFF
#endif

#ifndef IS31FL3729_SCALE_DEFAULT
    #define IS31FL3729_SCALE_DEFAULT    0xFF
#endif

#define TIMEOUT             100

#ifndef IS31FL3729_I2C_ID
    #define IS31FL3729_I2C_ID   I2C_INSTANCE_1
#endif

#define FLUSH_COUNT     9
#define FLUSH_BATCH     9
static i2c_handle_t i2c_inst;
static bool fl3729_available = true;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE];
    uint8_t         last_index;
    bool            pwm_dirty;
    bool            ready;
    uint32_t        count;
} is31fl3729_driver_t;

static is31fl3729_driver_t is31_drivers[IS31FL3729_NUM] = {0};

#ifdef RTOS_ENABLE
#define IS31FL3729_ASYNC
#endif

#ifdef IS31FL3729_ASYNC
#include "tx_api.h"
static TX_SEMAPHORE is31_write;
static TX_SEMAPHORE* p_write = NULL;
static void is31_sema_init(void)
{
    if (TX_SUCCESS == tx_semaphore_create(&is31_write, "is31_write", 0)) {
        p_write = &is31_write;
    }
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c== i2c_inst) {
        if (p_write) {
            tx_semaphore_put(p_write);
        }
    }
}
#endif

static void init_driver(is31fl3729_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

bool is31fl3729_available(uint8_t addr)
{
    return fl3729_available;
}

i2c_led_t *is31fl3729_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3729_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    init_driver(driver);

    driver->ready = true;
    fl3729_debug("IS31FL3729: initialized\n");
    return &driver->i2c_led;
}

void is31fl3729_uninit(i2c_led_t *driver)
{   
    if (!is31fl3729_available(0)) return;

    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3729_driver_t *is31 = (is31fl3729_driver_t*)(driver->data);
    
    memset(is31, 0, sizeof(is31fl3729_driver_t));
}

void is31fl3729_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3729_driver_t *is31 = (is31fl3729_driver_t*)(driver->data);

    if (is31->pwm_buffer[led->r] != red) {
        is31->pwm_buffer[led->r] = red;
        is31->pwm_dirty = true;
    }

    if (is31->pwm_buffer[led->g] != green) {
        is31->pwm_buffer[led->g] = green;
        is31->pwm_dirty = true;
    }

    if (is31->pwm_buffer[led->b] != blue) {
        is31->pwm_buffer[led->b] = blue;
        is31->pwm_dirty = true;
    }
}

void is31fl3729_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3729_set_color(driver, i, red, green, blue);
    }
}

static bool is31fl3729_update_pwm_buffers(i2c_led_t *driver)
{
    if (!is31fl3729_available(0)) return true;

    is31fl3729_driver_t *is31 = (is31fl3729_driver_t*)(driver->data);

    if (!is31->pwm_dirty) return true;

    // write from sw1 to sw9
#ifdef RGB_FLUSH_ASYNC
    if (AMK_SUCCESS != ak_i2c_send_async(i2c_inst, driver->addr, &is31->pwm_buffer[is31->last_index*0x10], 0x10)) {
        return false;
    }
#elif defined(IS31FL3729_ASYNC)
    if (p_write == NULL) {
        is31_sema_init();
    }

    for (int i = 0; i < FLUSH_BATCH; i++) {
        ak_i2c_send_async(i2c_inst, driver->addr, &is31->pwm_buffer[i*0x10], 0x10);
        tx_semaphore_get(p_write, TX_WAIT_FOREVER);
    }
    is31->pwm_dirty = false;
    return true;
#else
    for (int i = 0; i < FLUSH_BATCH; i++) {
        ak_i2c_send(i2c_inst, driver->addr, &is31->pwm_buffer[i*0x10], 0x10, TIMEOUT);
    }
    is31->pwm_dirty = false;
    return true;
#endif

#ifdef RGB_FLUSH_ASYNC
    is31->last_index = (is31->last_index + 1) % FLUSH_BATCH;

    if (is31->last_index == 0) {
        is31->count++;
        if (is31->count % FLUSH_COUNT == 0) {
            is31->pwm_dirty = false;
        }
        return true;
    }
#endif

    return false;
}

#ifdef SDB_EN_PIN
#include "amk_gpio.h"
#endif

void init_driver(is31fl3729_driver_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(IS31FL3729_I2C_ID);
    }
#ifdef SDB_EN_PIN
    gpio_set_output_pushpull(SDB_EN_PIN);
    gpio_write_pin(SDB_EN_PIN, 1);
    wait_ms(1);
#endif

    // Reset 3729 to default state
    uint8_t data = RESET_COMMAND;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, RESET_REG, &data, 1, TIMEOUT);

    if (status != AMK_SUCCESS) {
        fl3729_debug("IS31FL3729: failed to reset: %d\n", status);
        fl3729_available = false;
        return;
    }

    wait_ms(10);

    // enable chip
    data = 0x01; // 9x15, lgc 1.4v/0v, disalbe osde
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, CONFIG_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3729_debug("IS31FL3729: failed to enable chip: %d\n", status);
    }

    // set global current
    data = IS31FL3729_GLOBAL_CURRENT;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, GLOBAL_CURRENT_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3729_debug("IS31FL3729: failed to set global current: %d\n", status);
    }


    // set pulldown and pullup
    // AS default
    //data = ((IS31FL3729_PD_DEFAULT<<PD_OFFSET) | (IS31FL3729_PU_DEFAULT<<PU_OFFSET));
    //status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, PD_PU_REG, &data, 1, TIMEOUT);

    // set scale buffer
    data = IS31FL3729_SCALE_DEFAULT;
    for (int i = 0;i < 0x0F; i++) {
        status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, SCALE_BASE_REG+i, &data, 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3729_debug("IS31FL3729: failed to set scale: index: %d, status: %d\n", i, status);
        }
    }

    // reset pwm
    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE);
    // set swx first reg address
    for (int i = 0; i < 9; i++) {
        driver->pwm_buffer[i*0x10] = i*0x10+1;
    }

    driver->pwm_dirty = true;
    driver->last_index = 0;
    driver->count = 0;
}

bool is31fl3729_update_buffers(i2c_led_t *driver)
{
    return is31fl3729_update_pwm_buffers(driver);
}

void uninit_driver(i2c_led_t *driver)
{
    // shutdonw driver
    uint8_t data = 0;
    ak_i2c_write_reg(i2c_inst, driver->addr, CONFIG_REG, &data, 1, TIMEOUT);
}

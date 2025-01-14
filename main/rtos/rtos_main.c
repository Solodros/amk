/**
 * @file rtos_main.c
 */

#include "cmsis_os2.h"
#include "amk_hal.h"
#include "board.h"
#include "usb_interface.h"
#include "rgb_led.h"
#include "amk_driver.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifdef RF_ENABLE
#include "rf_driver.h"
#endif

#ifdef MSC_ENABLE
#include "mscusb.h"
#endif

#include "FreeRTOS.h"

extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

// flags
#define FLAGS_MAIN_KEYBOARD     (1<<0)
#define FLAGS_MAIN_OTHER        (1<<1)

// thread parameters
#define USB_THREAD_STACK        512
#define USB_THREAD_PRIORITY     osPriorityHigh
#define MAIN_THREAD_STACK       1024
#define MAIN_THREAD_PRIORITY    osPriorityNormal

static osThreadId_t usb_thread_id;
static uint32_t usb_thread_cb[WORDS(sizeof(StaticTask_t))];
static uint64_t usb_thread_stack[USB_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t usb_thread_attr = {
    .name = "usb",
    .cb_mem = usb_thread_cb,
    .cb_size = sizeof(usb_thread_cb),
    .stack_mem = usb_thread_stack,
    .stack_size = sizeof(usb_thread_stack),
    .priority = USB_THREAD_PRIORITY,
};

static osThreadId_t main_thread_id;
static uint32_t main_thread_cb[WORDS(sizeof(StaticTask_t))];
static uint64_t main_thread_stack[MAIN_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t main_thread_attr = {
    .name = "main",
    .cb_mem = main_thread_cb,
    .cb_size = sizeof(main_thread_cb),
    .stack_mem = main_thread_stack,
    .stack_size = sizeof(main_thread_stack),
    .priority = MAIN_THREAD_PRIORITY,
};

#ifndef USE_HARDWARE_TIMER
static uint32_t main_timer_cb[WORDS(sizeof(StaticTimer_t))];
static const osTimerAttr_t main_timer_attr = {
    .name = "timer",
    .cb_mem = main_timer_cb,
    .cb_size = sizeof(main_timer_cb),
};
#else
__attribute__((weak))
void rtos_timer_init(void){
}
#endif

static void usb_thread(void *arg);
static void main_thread(void *arg);

void rtos_timer_task(void *arg);

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    osKernelInitialize();

    usb_thread_id = osThreadNew(usb_thread, NULL, &usb_thread_attr);
    main_thread_id = osThreadNew(main_thread, NULL, &main_thread_attr);
    osKernelStart();

    // never reach here
    for (;;) {}

    return 0;
}

void usb_thread(void *arg)
{
    usb_init();
    while (1) {
        usb_task_usb();
    }
}

void main_thread(void *arg)
{
    uint32_t flags = 0;
#ifndef USE_HARDWARE_TIMER
    osTimerId_t tmr_id = osTimerNew(rtos_timer_task, osTimerPeriodic, NULL, &main_timer_attr);
    osTimerStart(tmr_id, 1);
#else
    rtos_timer_init();
#endif
    
    amk_printf("amk_init\n");
    amk_driver_init();
    amk_printf("board_init end\n");
    while(1) {
        flags = osThreadFlagsWait(
                        FLAGS_MAIN_KEYBOARD     // keyboard flag 
                       | FLAGS_MAIN_OTHER       // other flag 
                       , osFlagsWaitAny
                       , osWaitForever);

        if (flags & FLAGS_MAIN_KEYBOARD) {
            usb_task_report();
            amk_driver_task();
        }

        if (flags & FLAGS_MAIN_OTHER) {
            // other task handler
        #ifdef MSC_ENABLE
            msc_task();
        #endif

        #ifdef AUDIO_ENABLE
            audio_task();
        #endif

        #ifdef SCREEN_ENABLE
            render_task();
        #endif

        #ifdef RGB_ENABLE
            rgb_led_task();
        #endif

        #ifdef RF_ENABLE
            rf_driver_task();
        #endif

            custom_board_task();
        }
    }
}


void board_init(void)
{
    amk_printf("system_init\n");
    system_init();
    amk_printf("custom_board_init\n");
    custom_board_init();
}

#ifndef FLAGS_OTHER_COUNT
#define FLAGS_OTHER_COUNT   10
#endif
void rtos_timer_task(void *arg)
{
    static uint32_t i = 0;
    osThreadFlagsSet(main_thread_id, FLAGS_MAIN_KEYBOARD);
    if (!(i++ % FLAGS_OTHER_COUNT)) {
        osThreadFlagsSet(main_thread_id, FLAGS_MAIN_OTHER);
    }
}

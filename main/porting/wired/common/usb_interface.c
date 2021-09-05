/**
 * usb_interface.c
 */

#include "usb_interface.h"
#include "amk_usb.h"
#include "usb_interface.h"
#include "generic_hal.h"
#include "report_queue.h"
#include "report.h"
#include "wait.h"
#include "amk_keymap.h"
#include "amk_printf.h"
#include "amk_macro.h"

#ifdef VIAL_ENABLE
#include "vial_porting.h"
#include "vial_macro.h"
#endif

static hid_report_queue_t report_queue;

#ifdef VIAL_ENABLE
static hid_report_queue_t macro_queue;
extern uint8_t amk_macro_state;
extern uint32_t amk_macro_delay;
static void process_macro(hid_report_queue_t *queue);
#endif

static void process_report_queue(hid_report_queue_t *queue);

void usb_init(void)
{
    amk_usb_init();

    hid_report_queue_init(&report_queue);

#ifdef VIAL_ENABLE
    hid_report_queue_init(&macro_queue);
#endif
}

void usb_task(void)
{
#ifdef VIAL_ENABLE
    if (!hid_report_queue_empty(&macro_queue)) {
        process_report_queue(&macro_queue);
    } else 
#endif
    if (!hid_report_queue_empty(&report_queue)) {
#ifdef VIAL_ENABLE
        hid_report_t* item = hid_report_queue_peek(&report_queue);
        if (item->type == HID_REPORT_ID_MACRO) {
            process_macro(&report_queue);
        } else
#endif
        {
            process_report_queue(&report_queue);
        }
    }

#ifdef VIAL_ENABLE
    vial_task();
#endif
    amk_usb_task();
}

static void process_report_queue(hid_report_queue_t * queue)
{
    hid_report_t* item = hid_report_queue_peek(queue);
    if (amk_usb_itf_ready(item->type)) {
        amk_printf("ITF ready, type:%d, send report\n", item->type);
        hid_report_t report;
        hid_report_queue_get(queue, &report);
        amk_usb_itf_send_report(report.type, report.data, report.size);
        if (report.type == HID_REPORT_ID_KEYBOARD) {
            report_keyboard_t *keyboard = (report_keyboard_t*)&report.data[0];
            for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
                if(keyboard->keys[i] == KC_CAPS) {
                    wait_ms(100); // fix caps lock under mac
                    amk_printf("delay 100ms after sent caps\n");
                }
            }
        }
    }
}

#ifdef VIAL_ENABLE
static void process_macro(hid_report_queue_t *queue)
{
    hid_report_t *item = hid_report_queue_peek(queue);
    amk_macro_t *macro = (amk_macro_t*)&(item->data[0]);
    macro->delay = 0;
    amk_macro_state = 1;
    if (!vial_macro_play(macro->id, &macro->offset, &macro->delay)) {
        // macro finished
        hid_report_queue_pop(queue);
    }
    if (macro->delay) {
        amk_macro_delay = macro->delay;
    }
    amk_macro_state = 0;
}
#endif

bool usb_ready(void)
{
    return amk_usb_ready();
}

bool usb_suspended(void)
{
    return amk_usb_suspended();
}

void usb_remote_wakeup(void)
{
    hid_report_queue_init(&report_queue);

    amk_usb_remote_wakeup();
}

void usb_connect(bool on)
{
    amk_usb_connect(on);
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    hid_report_t item;
    memcpy(item.data, data, size);
    item.type = report_type;
    item.size = size;
#ifdef VIAL_ENABLE
    if (amk_macro_state) {
        item.delay = amk_macro_delay;
        amk_macro_delay = 0;
    } else
#endif
    {
        item.delay = 0;
    }

#ifdef VIAL_ENABLE
    if (amk_macro_state) {
        hid_report_queue_put(&macro_queue, &item);
    } else
#endif
    {
        hid_report_queue_put(&report_queue, &item);
    }
}

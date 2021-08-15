/**
 * @file usb_usbx.c
 */

#include "generic_hal.h"
#include "ux_api.h"
#include "ux_device_class_hid.h"
#include "ux_dcd_stm32.h"
#include "usb_interface.h"
#include "usb_descriptors.h"


#define USB_BASE_ADDRESS 0x40040000

PCD_HandleTypeDef  pcd_handle;

static UX_SLAVE_CLASS_HID *get_hid_by_itf(ULONG itf);

void usb_init(void)
{
    pcd_handle.Instance = USB_OTG_FS;
    pcd_handle.Init.dev_endpoints = 4;
    pcd_handle.Init.speed = PCD_SPEED_FULL;
    pcd_handle.Init.dma_enable = DISABLE;
    pcd_handle.Init.phy_itface = PCD_PHY_EMBEDDED;
    pcd_handle.Init.Sof_enable = DISABLE;
    pcd_handle.Init.low_power_enable = DISABLE;
    pcd_handle.Init.lpm_enable = DISABLE;
    pcd_handle.Init.vbus_sensing_enable = DISABLE;
    pcd_handle.Init.use_dedicated_ep1 = DISABLE;

    if (ux_dcd_stm32_initialize(USB_BASE_ADDRESS, (ULONG)&pcd_handle) != UX_SUCCESS) {
        while(1);
    }

    if (HAL_PCD_Init(&pcd_handle) != HAL_OK) {
        while(1);
    }

    HAL_PCDEx_SetRxFiFo(&pcd_handle, 0x80);
    HAL_PCDEx_SetTxFiFo(&pcd_handle, 0, 0x20);
    HAL_PCDEx_SetTxFiFo(&pcd_handle, 1, 0x10);
    HAL_PCDEx_SetTxFiFo(&pcd_handle, 2, 0x10);
    HAL_PCDEx_SetTxFiFo(&pcd_handle, 3, 0x80);

    if (HAL_PCD_Start(&pcd_handle) != HAL_OK) {
        while(1);
    }
}

void usb_task(void)
{
}

bool usb_ready(void)
{
    UX_SLAVE_DEVICE *device = &_ux_system_slave->ux_system_slave_device;

    return (device->ux_slave_device_state == UX_DEVICE_CONFIGURED);
}

bool usb_suspended(void)
{
    UX_SLAVE_DEVICE *device = &_ux_system_slave->ux_system_slave_device;

    return (device->ux_slave_device_state == UX_DEVICE_SUSPENDED);
}

void usb_remote_wakeup(void)
{
    ux_device_stack_host_wakeup();
}

void usb_connect(bool on)
{
    if (on) {
        HAL_PCD_DevConnect(&pcd_handle);
    } else {
        HAL_PCD_DevDisconnect(&pcd_handle);
    }
}


void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    UX_SLAVE_CLASS_HID_EVENT event;
    event.ux_device_class_hid_event_report_type = UX_DEVICE_CLASS_HID_REPORT_TYPE_INPUT;
    event.ux_device_class_hid_event_length = size;
    ux_utility_memory_copy(event.ux_device_class_hid_event_buffer, (void*)data, size);
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        event.ux_device_class_hid_event_report_id = HID_REPORT_ID_KEYBOARD;
        ux_device_class_hid_event_set(get_hid_by_itf(ITF_NUM_HID_KBD), &event);
        break;
    case HID_REPORT_ID_MOUSE:
        event.ux_device_class_hid_event_report_id = HID_REPORT_ID_MOUSE;
        ux_device_class_hid_event_set(get_hid_by_itf(ITF_NUM_HID_OTHER), &event);
        break;
    case HID_REPORT_ID_SYSTEM:
        event.ux_device_class_hid_event_report_id = HID_REPORT_ID_SYSTEM;
        ux_device_class_hid_event_set(get_hid_by_itf(ITF_NUM_HID_OTHER), &event);
        break;
    case HID_REPORT_ID_CONSUMER:
        event.ux_device_class_hid_event_report_id = HID_REPORT_ID_CONSUMER;
        ux_device_class_hid_event_set(get_hid_by_itf(ITF_NUM_HID_OTHER), &event);
        break;
    }
}

static UX_SLAVE_CLASS_HID *get_hid_by_itf(ULONG itf)
{
    UX_SLAVE_DEVICE *device = &_ux_system_slave->ux_system_slave_device;
    UX_SLAVE_INTERFACE *interface = device->ux_slave_device_first_interface;

    while(interface != UX_NULL) {
        if (interface->ux_slave_interface_descriptor.bInterfaceNumber == itf) {
            return interface->ux_slave_interface_class_instance;
        }

        interface = interface->ux_slave_interface_next_interface;
    }

    return UX_NULL;
}

void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USB_OTG_FS GPIO Configuration
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&pcd_handle);
}
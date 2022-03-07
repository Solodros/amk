/**
 * @file hcd_hal_stm32.c
 * @brief 
 * 
 */

#include "generic_hal.h"
#include "host/hcd.h"
#include "amk_printf.h"

#ifndef HCD_DEBUG
#define HCD_DEBUG   1
#endif

#if HCD_DEBUG
#define hcd_debug   amk_printf
#else
#define hcd_debug(...)
#endif

#if defined(STM32F405xx)
#define HCD_MAX_CHANNELS    12
#define HCD_USB_INSTANCE    USB_OTG_HS
#define HCD_USB_IRQn        OTG_HS_IRQn
#define HCD_USB_PORT        1
#else
    #error "HAL USB HOST unsupported mcu"
#endif

typedef struct {
    uint8_t dev_addr;
    uint8_t endpoint;
    uint8_t channel;
    uint8_t used;
} hcd_usb_pipe_t;

typedef struct {
    bool    connected;
    bool    port_enabled;
    hcd_usb_pipe_t pipes[HCD_MAX_CHANNELS];
    HCD_HandleTypeDef *hcd_handle;
} hcd_usb_state_t;

HCD_HandleTypeDef hcd_usb;

hcd_usb_state_t hcd_state;

static void hcd_state_init(hcd_usb_state_t *state, HCD_HandleTypeDef *hcd)
{
    state->connected = false;
    state->port_enabled = false;
    for (uint32_t i = 0; i < HCD_MAX_CHANNELS; i++) {
        state->pipes[i].dev_addr = 0;
        state->pipes[i].endpoint = 0;
        state->pipes[i].channel = 0;
        state->pipes[i].used = 0;
    }
    state->hcd_handle = hcd;
    hcd->pData = state;
}

static hc_usb_pipe_t allocate_pipe(uint8_t addr, uint8_t ep)
{
    hc_usb_pipe_t *pipe = NULL;
    for (uint32_t i = 0; i < HCD_MAX_CHANNELS; i++) {
        if (hcd_state.pipes[i].used == 0) {
            pipe = &hcd_state.pipes[i];
            pipe->dev_addr = addr;
            pipe->endpoint = ep;
            pipe->channel = i;
            pipe->used = 1;
            break;
        } else {
            if ((pipe->dev_addr==addr) && (pipe->endpoint == ep)) {
                hcd_debug("pipe already allocated: addr=%d, ep=%d\n", addr, ep);
            }
        }
    }

    return pipe;
}

static void free_pipe(uint8_t ep)
{
    for (uint32_t i = 0; i < HCD_MAX_CHANNELS; i++) {
        if (hcd_state.pipes[i].used &&(hcd_state.pipes[i].endpoint == ep)) {
            hcd_state.pipes[i].used = 0;
            hcd_debug("free pipe: ep=%d, channel=%d\n", ep, hcd_state.pipes[i].channel);
            break;
        }
    }
}

static bool open_pipe(uint8_t pipe, uint8_t ep_num, uint8_t dev_addr, uint8_t speed, uint8_t ep_type, uint16_t packet_size)
{
    if (HAL_HCD_HC_Init(&hcd_usb, pipe, ep_num, dev_addr, speed, ep_type, packet_size) != HAL_OK) {
        hcd_debug("failed to open pipe:%d, ep:%d\n", pipe, ep_num);
        return false;
    }
    return true;
}

static void close_pipe(uint8_t pipe)
{
    if (pipe >= HCD_MAX_CHANNELS) {
        hcd_debug("invalid pipe number: %d\n", pipe);
        return;
    }

    HAL_HCD_HC_Halt(&hcd_usb, pipe);
}

static void set_pipe_toggle(uint8_t pipe, uint8_t toggle)
{
    if (pipe >= HCD_MAX_CHANNELS) {
        hcd_debug("invalid pipe number for setting toggle: %d\n", pipe);
        return;
    }

    if (hcd_usb.hc[pipe].ep_is_in) {
        hcd_usb.hc[pipe].toggle_in = toggle;
    } else {
        hcd_usb.hc[pipe].toggle_out = toggle;
    }
}

//--------------------------------------------------------------------+
// Controller API
//--------------------------------------------------------------------+

// Initialize controller to host mode
bool hcd_init(uint8_t rhport)
{
    hcd_state_init(&hcd_state, &hcd_usb);

    hcd_usb.Instance = HCD_USB_INSTANCE;
    hcd_usb.Init.Host_channels = HCD_MAX_CHANNELS;
    hcd_usb.Init.speed = HCD_SPEED_FULL;
    hcd_usb.Init.dma_enable = DISABLE;
    hcd_usb.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
    hcd_usb.Init.Sof_enable = DISABLE;
    hcd_usb.Init.low_power_enable = DISABLE;
    hcd_usb.Init.vbus_sensing_enable = DISABLE;
    hcd_usb.Init.use_external_vbus = DISABLE;

    if (HAL_HCD_Init(&hcd_usb) != HAL_OK) {
        hcd_debug("failed to initialize usb host\n");
    }

    if (HAL_HCD_Start(&hcd_usb) != HAL_OK) {
        hcd_debug("Failed to start usb host\n");
    }
}

// Interrupt Handler
void hcd_int_handler(uint8_t rhport)
{
    HAL_HCD_IRQHandler(&hcd_usb);
}

// Enable USB interrupt
void hcd_int_enable (uint8_t rhport)
{
    HAL_NVIC_EnableIRQ(HCD_USB_IRQn);
}

// Disable USB interrupt
void hcd_int_disable(uint8_t rhport)
{
    HAL_NVIC_DisableIRQ(HCD_USB_IRQn);
}

// Get frame number (1ms)
uint32_t hcd_frame_number(uint8_t rhport)
{
    HAL_HCD_GetCurrentFrame(&hcd_usb);
}

//--------------------------------------------------------------------+
// Port API
//--------------------------------------------------------------------+

// Get the current connect status of roothub port
bool hcd_port_connect_status(uint8_t rhport)
{
    return hcd_state.connected;
}

// Reset USB bus on the port
void hcd_port_reset(uint8_t rhport)
{
    HAL_HCD_ResetPort(&hcd_usb);
}

// TODO implement later
void hcd_port_reset_end(uint8_t rhport)
{
}

// Get port link speed
tusb_speed_t hcd_port_speed_get(uint8_t rhport)
{
    tusb_speed_t speed = TUSB_SPEED_INVALID;
    switch (HAL_HCD_GetCurrentSpeed(&hcd_usb)) {
        case HCD_SPEED_FULL:
            speed = TUSB_SPEED_FULL;
            break;
        case HCD_SPEED_LOW:
            speed = TUSB_SPEED_LOW;
            break;
        case HCD_SPEED_HIGH:
        default:
            break;
    }
    return speed;
}

// HCD closes all opened endpoints belong to this device
void hcd_device_close(uint8_t rhport, uint8_t dev_addr)
{
    for (uint32_t i = 0; i < HCD_MAX_CHANNELS; i++) {
        if (hcd_state.pipes[i].used) {
            close_pipe(hcd_state.pipes[i].channel);
            hcd_state.pipes[i].used = 0;
        }
    }
}

//--------------------------------------------------------------------+
// Endpoint API
//--------------------------------------------------------------------+

bool hcd_edpt_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_endpoint_t const * ep_desc)
{
    hcd_usb_pipe_t *pipe = allocate_pipe(dev_addr, ep_desc->bEndpointAddress);
    if (!pipe) {
        hcd_debug("failed to allocated pipe: addr=%d, ep=%d\n", dev_addr, ep_desc->bEndpointAddress);
        return false;
    }

    if (!open_pipe(pipe->channel, ep_desc->bEndpointAddress, dev_addr, HAL_HCD_GetCurrentSpeed(&hcd_usb), 
                    ep_desc->bDescriptorType, ep_desc->wMaxPacketSize.size)) {
        free_pipe(pipe->channel);
        return false;
    }
    if ((ep_desc->bEndpointAddress & 0x7F) != 0) {
        set_pipe_toggle(pipe->channel, 0);
    }
    return true;
}

bool hcd_edpt_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr, uint8_t * buffer, uint16_t buflen)
{
    return true;
}

bool hcd_setup_send(uint8_t rhport, uint8_t dev_addr, uint8_t const setup_packet[8])
{
    return true;
}

bool hcd_edpt_clear_stall(uint8_t dev_addr, uint8_t ep_addr)
{
    return true;
}

// HAL call back
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
}

void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
    hcd_state.connected = true;
    hcd_event_device_attach(HCD_USB_PORT, true);
}

void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
    hcd_state.connected = false;
    hcd_event_device_remove(HCD_USB_PORT, true);
}

void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
    hcd_state.port_enabled = true;
}

void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
    hcd_state.port_enabled = false;
}

void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{}
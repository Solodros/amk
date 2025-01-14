/**
 * dcd_hal.c
 */
#include "amk_hal.h"
#include "device/dcd.h"
#include "amk_printf.h"

#if defined(STM32F103xB)
    #define DCD_MAX_EP_NUM      8
    #define DCD_USB_INSTANCE    USB
    #define DCD_USB_IRQn        USB_LP_CAN1_RX0_IRQn
    #define PHY_INTERFACE       PCD_PHY_EMBEDDED
#elif defined(STM32F411xE) || defined(STM32F401xC)
    #define DCD_MAX_EP_NUM      4
    #define DCD_USB_INSTANCE    USB_OTG_FS
    #define DCD_USB_IRQn        OTG_FS_IRQn
    #define PHY_INTERFACE       PCD_PHY_EMBEDDED
#elif defined(STM32F722xx) || defined(STM32F405xx) || defined(STM32F446xx) || defined(STM32F412Rx)
    #ifdef USE_HS_USB
        #define DCD_MAX_EP_NUM      6
        #define DCD_USB_INSTANCE    USB_OTG_HS
        #define DCD_USB_IRQn        OTG_HS_IRQn
        #define PHY_INTERFACE       USB_OTG_EMBEDDED_PHY
    #else
        #define DCD_MAX_EP_NUM      6
        #define DCD_USB_INSTANCE    USB_OTG_FS
        #define DCD_USB_IRQn        OTG_FS_IRQn
        #define PHY_INTERFACE       PCD_PHY_EMBEDDED
    #endif
#elif defined(STM32L432xx)
    #define DCD_MAX_EP_NUM      8
    #define DCD_USB_INSTANCE    USB
    #define DCD_USB_IRQn        USB_FS_IRQn
    #define PHY_INTERFACE       PCD_PHY_EMBEDDED
#elif defined(STM32L072xx)
    #define DCD_MAX_EP_NUM      8
    #define DCD_USB_INSTANCE    USB
    #define DCD_USB_IRQn        USB_IRQn
    #define PHY_INTERFACE       PCD_PHY_EMBEDDED
#else
    #error "HAL USB unsupported mcu"
#endif

PCD_HandleTypeDef dcd_usb;
static uint16_t last_in_size = 0;
static uint16_t last_out_size = 0;

static void dcd_open_ep0(PCD_HandleTypeDef* usb)
{
    amk_printf("open ep0\n");
    // open ep0 here
    HAL_StatusTypeDef status = HAL_PCD_EP_Open(usb, 0x00U, CFG_TUD_ENDPOINT0_SIZE, TUSB_XFER_CONTROL);
    if (status != HAL_OK) {
        amk_printf("Failed to open ep0 IN, status:%d\n", status);
    }

    status = HAL_PCD_EP_Open(usb, 0x80U, CFG_TUD_ENDPOINT0_SIZE, TUSB_XFER_CONTROL);
    if (status != HAL_OK) {
        amk_printf("Failed to open ep0 OUT, status:%d\n", status);
    }
}

void dcd_init(uint8_t rhport)
{
#if defined(STM32L432xx)
    /* Enable USB power on Pwrctrl CR2 register. */
    HAL_PWREx_EnableVddUSB();
#endif

    dcd_usb.Instance                = DCD_USB_INSTANCE;
    dcd_usb.Init.dev_endpoints      = DCD_MAX_EP_NUM;
    dcd_usb.Init.speed              = PCD_SPEED_FULL;
    dcd_usb.Init.phy_itface         = PHY_INTERFACE;
    dcd_usb.Init.Sof_enable         = DISABLE;
    dcd_usb.Init.low_power_enable   = DISABLE;
    dcd_usb.Init.lpm_enable         = DISABLE;

#if defined(STM32F103xB)
    dcd_usb.Init.ep0_mps = CFG_TUD_ENDPOINT0_SIZE;
#else
    #if defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F722xx) || defined(STM32F401xC) || defined(STM32F446xx) || defined(STM32F412Rx)
    dcd_usb.Init.dma_enable = DISABLE;
    dcd_usb.Init.vbus_sensing_enable = DISABLE;
    dcd_usb.Init.use_dedicated_ep1 = DISABLE;
    #endif
#endif

#if defined(STM32F103xB) || defined(STM32F722xx) || defined(STM32L432xx) || defined(STM32L072xx) || defined(STM32F412Rx)
    dcd_usb.Init.battery_charging_enable = DISABLE;
#endif

#if defined(STM32F722xx)
    dcd_usb.Init.use_external_vbus = DISABLE;
#endif
    if (HAL_PCD_Init(&dcd_usb) != HAL_OK) {
        amk_printf("Failed to initialize HAL PCD\n");
    }
#if defined(STM32F411xE) || defined(STM32F401xC)
    HAL_PCDEx_SetRxFiFo(&dcd_usb, 0x80);            // shared rx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 0, 0x20);         // ep0 tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 1, 0x10);         // ep1 (keyboard) tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 2, 0x10);         // ep2 (mouse, system, consumer) tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 3, 0x80);         // ep3 (webusb or msc) tx buffer
#elif defined(STM32F405xx) || defined(STM32F722xx) || defined(STM32F446xx) || defined(STM32F412Rx)
    HAL_PCDEx_SetRxFiFo(&dcd_usb, 0x80);            // shared rx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 0, 0x20);         // ep0 tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 1, 0x10);         // ep1 (keyboard) tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 2, 0x10);         // ep2 (mouse, system, consumer) tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 3, 0x40);         // ep3 (vial or msc) tx buffer
    HAL_PCDEx_SetTxFiFo(&dcd_usb, 4, 0x40);         // ep3 (webusb) tx buffer
#else
    HAL_PCDEx_PMAConfig(&dcd_usb, 0x00, PCD_SNG_BUF, 0x40);
    HAL_PCDEx_PMAConfig(&dcd_usb, 0x80, PCD_SNG_BUF, 0x80);
    
    HAL_PCDEx_PMAConfig(&dcd_usb, 0x81, PCD_SNG_BUF, 0xC0);
    HAL_PCDEx_PMAConfig(&dcd_usb, 0x82, PCD_SNG_BUF, 0x100);
    HAL_PCDEx_PMAConfig(&dcd_usb, 0x03, PCD_SNG_BUF, 0x140);
    HAL_PCDEx_PMAConfig(&dcd_usb, 0x84, PCD_SNG_BUF, 0x180);
#endif

    HAL_NVIC_SetPriority(DCD_USB_IRQn, 1, 0);

    if (HAL_PCD_Start(&dcd_usb) != HAL_OK) {
        amk_printf("Failed to start HAL PCD\n");
    }
}

void dcd_int_handler(uint8_t rhport)
{
    HAL_PCD_IRQHandler(&dcd_usb);
}

void dcd_int_enable (uint8_t rhport)
{
    HAL_NVIC_EnableIRQ(DCD_USB_IRQn);
}

void dcd_int_disable(uint8_t rhport)
{
    HAL_NVIC_DisableIRQ(DCD_USB_IRQn);
}

void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    HAL_PCD_SetAddress(&dcd_usb, dev_addr);
    HAL_PCD_EP_Transmit(&dcd_usb, 0, NULL, 0);
}

void dcd_remote_wakeup(uint8_t rhport)
{
    HAL_PCD_ActivateRemoteWakeup(&dcd_usb);
    HAL_Delay(5);
    HAL_PCD_DeActivateRemoteWakeup(&dcd_usb);
}

void dcd_connect(uint8_t rhport)
{
    HAL_PCD_DevConnect(&dcd_usb);
}

void dcd_disconnect(uint8_t rhport)
{
    HAL_PCD_DevDisconnect(&dcd_usb);
}

// Invoked when a control transfer's status stage is complete.
// May help DCD to prepare for next control transfer, this API is optional.
// void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request) TU_ATTR_WEAK;

bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * p_ep)
{
    HAL_StatusTypeDef status = HAL_PCD_EP_Open(&dcd_usb, p_ep->bEndpointAddress, p_ep->wMaxPacketSize, p_ep->bmAttributes.xfer);
    if (status != HAL_OK) {
        amk_printf("Failed to open ep:%d, status:%d\n", p_ep->bEndpointAddress, status);
        return false;
    }
    return true;
}

void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    HAL_PCD_EP_Close(&dcd_usb, ep_addr);
}

void dcd_edpt_close_all (uint8_t rhport)
{}

bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir   = tu_edpt_dir(ep_addr);

    HAL_StatusTypeDef status = HAL_OK;
    if (dir == TUSB_DIR_IN) {
        status = HAL_PCD_EP_Transmit(&dcd_usb, epnum, buffer, total_bytes);
        last_in_size = total_bytes;
    } else {
        status = HAL_PCD_EP_Receive(&dcd_usb, epnum, buffer, total_bytes);
        last_out_size = total_bytes;
    }
    if (status != HAL_OK) {
        amk_printf("Failed to xfer ep:%d, status:%d\n", ep_addr, status);
        return false;
    }
    return true;
}

static uint8_t ff_buf[1024];
static tu_fifo_t *cur_ff = NULL;
static bool ff_valid = false;

bool dcd_edpt_xfer_fifo(uint8_t rhport, uint8_t ep_addr, tu_fifo_t * ff, uint16_t total_bytes)
{
    if (total_bytes > 1024) {
        amk_printf("XFER fifo oversize, ep:%d, size:%d\n", ep_addr, total_bytes);
        return false;
    }

    uint8_t const dir = tu_edpt_dir(ep_addr);
    if (dir == TUSB_DIR_OUT) {
        cur_ff = ff;
        ff_valid = true;
    } else {
        tu_fifo_read_n(ff, ff_buf, total_bytes);
    }
    return dcd_edpt_xfer(rhport, ep_addr, ff_buf, total_bytes);
}

void dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
    HAL_PCD_EP_SetStall(&dcd_usb, ep_addr);
}

void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    HAL_PCD_EP_ClrStall(&dcd_usb, ep_addr);
}


// PCD callback override
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *usb, uint8_t epnum)
{
    //amk_printf("PCD DataOut\n");
    //if (last_out_size==0) {
    //    dcd_event_xfer_complete(0, epnum, 0, XFER_RESULT_SUCCESS, true); 
    //} else {
        if (ff_valid) {
            int readed = HAL_PCD_EP_GetRxCount(usb, epnum);
            tu_fifo_write_n(cur_ff, ff_buf,  readed);
            ff_valid = false;
        }
        dcd_event_xfer_complete(0, epnum, HAL_PCD_EP_GetRxCount(usb, epnum), XFER_RESULT_SUCCESS, true); 
    //}
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *usb, uint8_t epnum)
{
    //amk_printf("PCD DataIn, last in size: %d\n", last_in_size);
    //if (last_in_size==0) {
    //    dcd_event_xfer_complete(0, 0x80|epnum, 0, XFER_RESULT_SUCCESS, true); 
    //} else {
#if defined(STM32L432xx)
        if (epnum == 0) {
            dcd_event_xfer_complete(0, 0x80|epnum, usb->IN_ep[epnum & EP_ADDR_MSK].xfer_count, XFER_RESULT_SUCCESS, true); 
        } else
        {
            dcd_event_xfer_complete(0, 0x80|epnum, last_in_size, XFER_RESULT_SUCCESS, true); 
        }
#else
        dcd_event_xfer_complete(0, 0x80|epnum, usb->IN_ep[epnum & EP_ADDR_MSK].xfer_count, XFER_RESULT_SUCCESS, true); 
#endif
    //}
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *usb)
{
    dcd_event_setup_received(0, (uint8_t*)usb->Setup, true);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *usb)
{
    dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *usb)
{
    dcd_open_ep0(usb);
    dcd_event_bus_signal(0, DCD_EVENT_BUS_RESET, true);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *usb)
{
    dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *usb)
{
    dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *usb)
{
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *usb)
{
    dcd_event_bus_signal(0, DCD_EVENT_UNPLUGGED, true);
}
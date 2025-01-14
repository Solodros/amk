/**
 * @file hc32.c
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"

#ifndef HSE_VALUE
#define HSE_VALUE   XTAL_VALUE
#endif

// put on retention memory
uint32_t BKP_DR0 __attribute__((section(".ret_ram_data")));     // for reboot to bootloader 
uint32_t BKP_DR1 __attribute__((section(".ret_ram_data")));     // for special reboot 

static void system_clock_init(void);
static void system_usb_init(void);
static void debug_port_init(void);

void system_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PRIGROUP_PRE4_SUB0);

    system_clock_init();

#ifndef RTOS_ENABLE
    systick_init();
#endif

    dwt_delay_init();

    debug_port_init();

#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
    system_usb_init();
}


uint32_t magic_read(void)
{
    return BKP_DR0;
}

void magic_write(uint32_t magic)
{
    BKP_DR0 = magic;
}

uint32_t reset_read(void)
{
    return BKP_DR1;
}

void reset_write(uint32_t reason)
{
    BKP_DR1 = reason;
}

static void debug_port_init(void)
{
    PORT_DebugPortSetting(TDO_SWO|TDI|TRST, Disable);

    PORT_DebugPortSetting(TCK_SWCLK|TMS_SWDIO, Enable);
}

// set clock use MPLL@200MHz
static void system_clock_init(void)
{
    stc_clk_sysclk_cfg_t    stcSysClkCfg;
    stc_clk_xtal_cfg_t      stcXtalCfg;
    stc_clk_mpll_cfg_t      stcMpllCfg;
    stc_sram_config_t       stcSramConfig;

    MEM_ZERO_STRUCT(stcSysClkCfg);
    MEM_ZERO_STRUCT(stcXtalCfg);
    MEM_ZERO_STRUCT(stcMpllCfg);
    MEM_ZERO_STRUCT(stcSramConfig);

    /* Set bus clk div. */
    stcSysClkCfg.enHclkDiv  = ClkSysclkDiv2;
    stcSysClkCfg.enExclkDiv = ClkSysclkDiv4;
    stcSysClkCfg.enPclk0Div = ClkSysclkDiv2;
    stcSysClkCfg.enPclk1Div = ClkSysclkDiv4;
    //stcSysClkCfg.enPclk2Div = ClkSysclkDiv8;
    stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;
    stcSysClkCfg.enPclk3Div = ClkSysclkDiv8;
    stcSysClkCfg.enPclk4Div = ClkSysclkDiv4;
    CLK_SysClkConfig(&stcSysClkCfg);

    /* Config Xtal and Enable Xtal */
    stcXtalCfg.enMode = ClkXtalModeOsc;
    stcXtalCfg.enDrv = ClkXtalLowDrv;
    stcXtalCfg.enFastStartup = Enable;
    CLK_XtalConfig(&stcXtalCfg);
    CLK_XtalCmd(Enable);

    /* sram init include read/write wait cycle setting */
    stcSramConfig.u8SramIdx = Sram12Idx | Sram3Idx | SramHsIdx | SramRetIdx;
    stcSramConfig.enSramRC = SramCycle2;
    stcSramConfig.enSramWC = SramCycle2;
    SRAM_Init(&stcSramConfig);

    /* flash read wait cycle setting */
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_5);
    EFM_Lock();

    /* MPLL config (XTAL / pllmDiv * plln / PllpDiv = 200M). */
    stcMpllCfg.pllmDiv = XTAL_VALUE/1000000;
    stcMpllCfg.plln    = 400ul;
    stcMpllCfg.PllpDiv = 2ul;
    stcMpllCfg.PllqDiv = 2ul;
    stcMpllCfg.PllrDiv = 2ul;
    CLK_SetPllSource(ClkPllSrcXTAL);
    CLK_MpllConfig(&stcMpllCfg);

    /* Enable MPLL. */
    CLK_MpllCmd(Enable);
    /* Wait MPLL ready. */
    while(Set != CLK_GetFlagStatus(ClkFlagMPLLRdy))
    {
        ;
    }
    /* Switch driver ability */
    PWC_HS2HP();
    /* Switch system clock source to MPLL. */
    CLK_SetSysClkSource(CLKSysSrcMPLL);

    SystemCoreClockUpdate();
}

void system_usb_init(void)
{}

void custom_board_task(void)
{}

#if 0
static void fault_handler(void) 
{
    #ifdef FAULT_BREAK
    __asm__("BKPT");
    #else
    NVIC_SystemReset();
    #endif
}

void NMI_Handler(void)
{
    fault_handler();
}

void HardFault_Handler(void)
{
    fault_handler();
}

void MemManage_Handler(void)
{
    fault_handler();
}

void BusFault_Handler(void)
{
    fault_handler();
}

void UsageFault_Handler(void)
{
    fault_handler();
}
#endif
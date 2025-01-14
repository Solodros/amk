/**
 * @file at32f405.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"
#include "usb_common.h"

#include "tusb.h"

// if want to USE USB_HS, the HEXT MUST be 12000000

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = (hext * pll_ns)/(pll_ms * pll_fp)
  *         system clock source   = pll (hext)
  *         - hext                = HEXT_VALUE
  *         - sclk                = 216000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 216000000
  *         - apb2div             = 1
  *         - apb2clk             = 216000000
  *         - apb1div             = 2
  *         - apb1clk             = 108000000
  *         - pll_ns              = 72
  *         - pll_ms              = 1
  *         - pll_fr              = 4
  *         - flash_wtcyc         = 6 cycle
  * @param  none
  * @retval none
  */
void system_clock_init(void)
{
    /* reset crm */
    crm_reset();

    /* config flash psr register */
    flash_psr_set(FLASH_WAIT_CYCLE_6);

    /* enable pwc periph clock */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* set power ldo output voltage to 1.3v */
    pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

    crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

    /* wait till hext is ready */
    while(crm_hext_stable_wait() == ERROR)
    {
    }

    /* if pll parameter has changed, please use the AT32_New_Clock_Configuration tool for new configuration. */
    crm_pll_config(CRM_PLL_SOURCE_HEXT, 72, 1, CRM_PLL_FP_4);

    /* config pllu div */
    crm_pllu_div_set(CRM_PLL_FU_18);

    /* enable pll */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

    /* wait till pll is ready */
    while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
    {
    }

    /* config ahbclk */
    crm_ahb_div_set(CRM_AHB_DIV_1);

    /* config apb2clk, the maximum frequency of APB2 clock is 216 MHz */
    crm_apb2_div_set(CRM_APB2_DIV_1);

    /* config apb1clk, the maximum frequency of APB1 clock is 120 MHz */
    crm_apb1_div_set(CRM_APB1_DIV_2);

    /* enable auto step mode */
    crm_auto_step_mode_enable(TRUE);

    /* select pll as system clock source */
    crm_sysclk_switch(CRM_SCLK_PLL);

    /* wait till pll is used as system clock source */
    while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
    {
    }

    /* disable auto step mode */
    crm_auto_step_mode_enable(FALSE);

    /* update system_core_clock global variable */
    system_core_clock_update();
}


static void usb_custom_init(void)
{
    /* enable otg clock */
    crm_periph_clock_enable(CRM_OTGHS_PERIPH_CLOCK, TRUE);

    /* select usb 48m clcok source */
    //usb_clock48m_select(USB_CLK_HEXT);

    /* enable otg irq */
    nvic_irq_enable(OTGHS_IRQn, 0, 0);
}

#ifdef USE_SPI1
spi_type *hspi1 = SPI1;
static void spi1_gpio_init(void)
{
    gpio_init_type gpio_initstructure;
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    /* master sck pin */
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
    gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_pins           = GPIO_PINS_5;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE5, GPIO_MUX_5);

    /* master miso pin */
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_pins           = GPIO_PINS_6;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE6, GPIO_MUX_5);

        /* master mosi pin */
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_pins           = GPIO_PINS_7;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_5);
}

static void spi1_init(void)
{
    spi1_gpio_init();

    crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
    spi_init_type spi_init_struct;
    spi_default_para_init(&spi_init_struct);
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_16;
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init(SPI1, &spi_init_struct);

    spi_enable(SPI1, TRUE);
}
#endif

#ifdef USE_I2C1
#include "i2c_application.h"
#define I2Cx_ADDRESS                     0x78 
//#define I2Cx_CLKCTRL                   0x4170FEFE   //10K
//#define I2Cx_CLKCTRL                   0x90F06666   //50K
#define I2Cx_CLKCTRL                     0x90F03030   //100K
//#define I2Cx_CLKCTRL                   0x20F07DDE   //200K
i2c_handle_type hi2c1;

static void i2c1_init(void)
{
    /* i2c periph clock enable */
    crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    hi2c1.i2cx = I2C1;

    /* reset i2c peripheral */
    i2c_reset(hi2c1.i2cx);

    /* i2c peripheral initialization */
    gpio_init_type gpio_init_structure;

    /* configure i2c pins: scl */
    gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_structure.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_structure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_structure.gpio_pull           = GPIO_PULL_UP;

    gpio_init_structure.gpio_pins           = GPIO_PINS_8;
    gpio_init(GPIOB, &gpio_init_structure);
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_4);

    /* configure i2c pins: sda */
    gpio_init_structure.gpio_pins           = GPIO_PINS_9;
    gpio_init(GPIOB, &gpio_init_structure);
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE9, GPIO_MUX_4);

    /* config i2c */
    i2c_init(hi2c1.i2cx, 0x0F, I2Cx_CLKCTRL);
    i2c_own_address1_set(hi2c1.i2cx, I2C_ADDRESS_MODE_7BIT, 0);

    /* i2c peripheral enable */
    i2c_enable(hi2c1.i2cx, TRUE);
    wait_ms(1);
}
#endif

#ifdef USE_ADC1

uint32_t adc1_channels[ADC_CHANNEL_COUNT] = ADC_CHANNELS;

extern void adc1_pins_init(void);
extern void adc1_dma_init(void);

static void adc1_init(void)
{
    adc1_pins_init();
    adc1_dma_init();

    adc_base_config_type adc_base_struct;
    crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
    adc_clock_div_set(ADC_DIV_8);
    //nvic_irq_enable(ADC1_IRQn, 1, 0);

    adc_base_default_para_init(&adc_base_struct);

    adc_base_struct.sequence_mode = TRUE;
    adc_base_struct.repeat_mode = FALSE;
    adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
    adc_base_struct.ordinary_channel_length = ADC_CHANNEL_COUNT;
    adc_base_config(ADC1, &adc_base_struct);

    /* config ordinary channel */
    for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        adc_ordinary_channel_set(ADC1, adc1_channels[i], i+1, ADC_SAMPLETIME_7_5);
    }

    /* config ordinary trigger source and trigger edge */
    adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);

    /* config dma mode,it's not useful when common dma mode is use */
    adc_dma_mode_enable(ADC1, TRUE);

    /* adc enable */
    adc_enable(ADC1, TRUE);

    /* adc calibration */
    adc_calibration_init(ADC1);
    while(adc_calibration_init_status_get(ADC1));
    adc_calibration_start(ADC1);
    while(adc_calibration_status_get(ADC1));
}

#endif

#ifdef USE_UART2
static void uart2_init(void)
{
    /* enable the usart2 and gpio clock */
    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    /* configure the usart2 tx, rx pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_2 | GPIO_PINS_3;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_7);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE3, GPIO_MUX_7);

    nvic_irq_enable(USART2_IRQn, 0, 0);

    /* configure usart2 param */
    usart_init(USART2, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(USART2, TRUE);
    usart_receiver_enable(USART2, TRUE);

//    usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
//    usart_enable(USART2, TRUE);
}

#endif

void custom_board_init(void)
{
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    usb_custom_init();

#ifdef USE_SPI1
    spi1_init();
#endif
#ifdef USE_I2C1
    i2c1_init();
#endif

#ifdef USE_ADC1
    adc1_init();
#endif

#ifdef USE_UART2
    uart2_init();
#endif

#ifdef DYNAMIC_CONFIGURATION
    uint32_t reset = reset_read();

    //if (reset == 0) {
    if (reset > 0) {
        usb_setting |= USB_MSC_BIT;
    } else {
        usb_setting = 0;
    }
    reset_write(0);
#endif
}

void custom_board_task(void)
{}

void OTGHS_IRQHandler(void)
{
    tud_int_handler(0);
}
/**
 * @file qspi_stm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>
#include "amk_hal.h"
#include "amk_error.h"
#include "amk_printf.h"

#ifndef QSPI_DEBUG
#define QSPI_DEBUG 0
#endif

#if QSPI_DEBUG
#define qspi_debug  amk_printf
#else
#define qspi_debug(...)
#endif

#if !defined(STM32F412Rx) && !defined(STM32L476xx)
#error "Current MCU Do NOt Support QSPI"
#endif

extern QSPI_HandleTypeDef hqspi;
extern DMA_HandleTypeDef hdma_quadspi;
extern void Error_Handler(void);

#define QSPI_FLASH_ADDR_SIZE                 (23)

/* FLASH parameters definition */
#define QSPI_FLASH_PAGE_SIZE                 (0x100u)
#define QSPI_FLASH_SECTOR_SIZE               (0x1000u)
#define QSPI_FLASH_MAX_ADDR                  (0x1000000u)
#define QSPI_FLASH_DUMMY_BYTE_VALUE          (0xffu)
#define QSPI_FLASH_BUSY_BIT_MASK             (0x01u)

/* FLASH instruction definition */
#define FLASH_INSTR_WRITE_ENABLE        (0x06u)
#define FLASH_INSTR_PAGE_PROGRAM        (0x02u)
#define FLASH_INSTR_PAGE_PROGRAM_QUAD   (0x32U)
#define FLASH_INSTR_ERASE_4KB_SECTOR    (0x20u)
#define FLASH_INSTR_ERASE_CHIP          (0xC7u)
#define FLASH_INSTR_READ_SR1            (0x05u)
#define FLASH_INSTR_READ_SR2            (0x35u)
#define FLASH_INSTR_READ_SR3            (0x15u)
#define FLASH_INSTR_WRITE_SR1           (0x01u)
#define FLASH_INSTR_WRITE_SR2           (0x31u)
#define FLASH_INSTR_WRITE_SR3           (0x11u)
#define FLASH_INSTR_READ_CMD            (0x03)
#define FLASH_INSTR_FAST_READ_CMD       (0x0B)
#define FLASH_INSTR_QUAD_READ_CMD       (0x6B)
#define FLASH_INSTR_QUAD_READ_IO_CMD    (0xEB)
#define FLASH_INSTR_RESET_ENABLE        (0x66U)
#define FLASH_INSTR_RESET               (0x99U)
#define FLASH_INSTR_JEDECID             (0x9FU)

/* TIMEOUT*/
#define FLASH_BULK_ERASE_MAX_TIME       (250000)
#define FLASH_SECTOR_ERASE_MAX_TIME     (3000)
#define FLASH_SUBSECTOR_ERASE_MAX_TIME  (800)

/* DUMMY CYCLES*/
#define DUMMY_CYCLES_FAST_READ          (8)
#define DUMMY_CYCLES_QUAD_READ          (8)
#define DUMMY_CYCLES_QUAD_READ_IO       (4)

/* STATUS */
#define W25Q128JV_FSR_BUSY              ((uint8_t)0x01)    /*!< busy */
#define W25Q128JV_FSR_WREN              ((uint8_t)0x02)    /*!< write enable */
#define W25Q128JV_FSR_QE                ((uint8_t)0x02)    /*!< quad enable */

#ifdef RTOS_ENABLE
#define QSPI_ASYNC  1
//#define QSPI_ASYNC  0
#else
#define QSPI_ASYNC  0
#endif

#if QSPI_ASYNC
#include "tx_api.h"
static TX_SEMAPHORE qspi_sema;
static volatile bool qspi_wait = false;
static void qspi_sema_init(void)
{
    if (TX_SUCCESS != tx_semaphore_create(&qspi_sema, "qspi_sema", 0)) {
        amk_printf("faield to create qspi semaphore\n");
    }
    qspi_wait = true;
}
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    if (qspi_wait) {
        qspi_wait = false;
        tx_semaphore_put(&qspi_sema);
    }
}
#endif

static amk_error_t QSPI_InitW25Qxx(QSPI_HandleTypeDef *hqspi);
static amk_error_t QSPI_EraseSector(QSPI_HandleTypeDef *hqspi, uint32_t address);
static amk_error_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static amk_error_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static amk_error_t QSPI_BusyWait(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);

bool qspi_init(uint32_t map_addr)
{
    hqspi.Instance = QUADSPI;

    /* Call the DeInit function to reset the driver */
    HAL_QSPI_DeInit(&hqspi);

    hqspi.Init.ClockPrescaler     = 1;
    hqspi.Init.FifoThreshold      = 4;
    hqspi.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    hqspi.Init.FlashSize          = QSPI_FLASH_ADDR_SIZE;
    hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_8_CYCLE;
    hqspi.Init.ClockMode          = QSPI_CLOCK_MODE_0;
            
#if defined(STM32F412Rx)
    /* QSPI initialization */
    hqspi.Init.FlashID            = QSPI_FLASH_ID_2;
    hqspi.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;
#endif

    if (HAL_QSPI_Init(&hqspi) != HAL_OK) {
        return false;
    }

    /* QSPI memory reset */
    if (QSPI_InitW25Qxx(&hqspi) != AMK_SUCCESS) {
        return false;
    }
    
#if QSPI_ASYNC
    qspi_sema_init();
#endif
    return true;
}

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length)
{
    if (length != QSPI_FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: read_sector: invalid size:%d\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    QSPI_CommandTypeDef s_command;

    /* Initialize the read command */
#if 1
    s_command.InstructionMode       = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction           = FLASH_INSTR_QUAD_READ_CMD;
    s_command.AddressMode           = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize           = QSPI_ADDRESS_24_BITS;
    s_command.Address               = address;
    s_command.AlternateByteMode     = QSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytes        = QSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize    = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode              = QSPI_DATA_4_LINES;
    s_command.DummyCycles           = DUMMY_CYCLES_QUAD_READ;
    s_command.NbData                = length;
    s_command.DdrMode               = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle      = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode              = QSPI_SIOO_INST_EVERY_CMD;
#else
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_READ_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = address;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = length;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
#endif
    
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return AMK_QSPI_ERROR;
    }

    /* Reception of the data */
#if QSPI_ASYNC
    if (HAL_QSPI_Receive_DMA(&hqspi, buffer) != HAL_OK) {
        qspi_debug("QSPI: receive dma failed:from 0x%x, size:%d\n", address, length);

        return AMK_QSPI_ERROR;
    }

    qspi_wait = true;
    tx_semaphore_get(&qspi_sema, TX_WAIT_FOREVER);
#else

    if (HAL_QSPI_Receive(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return AMK_QSPI_ERROR;
    }
#endif
    qspi_debug("QSPI: read_sector: from 0x%x, size:%d\n", address, length);

    return AMK_SUCCESS;
}

amk_error_t qspi_write_sector(uint32_t address, const uint8_t* buffer, size_t length)
{
    //qspi_debug("QSPI WRITE: addr=%d, size=%d\n", address, length);
    if (length != QSPI_FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: write_sector: invalid size:%u\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    // erase sector first
    if (address >= QSPI_FLASH_MAX_ADDR) {
        qspi_debug("QSPI: write_sector: invalid address:%u\n", address);
        return AMK_QSPI_INVALID_PARAM;
    } else {
        if ( QSPI_EraseSector(&hqspi, address) != AMK_SUCCESS) {
            qspi_debug("QSPI: write_sector: faled to erase sector:%u\n", address);
            return AMK_QSPI_ERROR;
        }
    }

    // then program
    uint32_t addr = address;
    const uint8_t *cur = buffer;
    for (int i = 0; i < length/QSPI_FLASH_PAGE_SIZE; i++) {
        QSPI_CommandTypeDef s_command;

        s_command.InstructionMode       = QSPI_INSTRUCTION_1_LINE;
        s_command.Instruction           = FLASH_INSTR_PAGE_PROGRAM_QUAD;
        s_command.AddressMode           = QSPI_ADDRESS_1_LINE;//QSPI_ADDRESS_4_LINES;
        s_command.AddressSize           = QSPI_ADDRESS_24_BITS;
        s_command.DataMode              = QSPI_DATA_4_LINES;
        s_command.DummyCycles           = 0;
        s_command.DdrMode               = QSPI_DDR_MODE_DISABLE;
        s_command.DdrHoldHalfCycle      = QSPI_DDR_HHC_ANALOG_DELAY;
        s_command.SIOOMode              = QSPI_SIOO_INST_EVERY_CMD;
        s_command.AlternateByteMode     = QSPI_ALTERNATE_BYTES_NONE;
        s_command.AlternateBytes        = QSPI_ALTERNATE_BYTES_NONE;
        s_command.AlternateBytesSize    = QSPI_ALTERNATE_BYTES_NONE;

        s_command.Address = addr;
        s_command.NbData  = QSPI_FLASH_PAGE_SIZE;
        
        /* Enable write operations */
        if (QSPI_WriteEnable(&hqspi) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
        
        /* Configure the command */
        if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return AMK_QSPI_ERROR;
        }
        
        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&hqspi, (uint8_t*)cur, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return AMK_QSPI_ERROR;
        }
        
        /* Configure automatic polling mode to wait for end of program */  
        if (QSPI_BusyWait(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
        addr += QSPI_FLASH_PAGE_SIZE;
        cur += QSPI_FLASH_PAGE_SIZE;
    }
    
    qspi_debug("QSPI: write_sector:%u success\n", address);
    return AMK_SUCCESS;
}

amk_error_t qspi_erase_chip(void)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_ERASE_CHIP;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (QSPI_WriteEnable(&hqspi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (QSPI_BusyWait(&hqspi, FLASH_BULK_ERASE_MAX_TIME) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

void qspi_uninit(void)
{
    /* Call the DeInit function to reset the driver */
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
    }
}

amk_error_t QSPI_EraseSector(QSPI_HandleTypeDef *hqspi, uint32_t address)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_ERASE_4KB_SECTOR;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = address;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (QSPI_WriteEnable(hqspi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (QSPI_BusyWait(hqspi, FLASH_SECTOR_ERASE_MAX_TIME) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_RESET_ENABLE;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    /* Send the reset memory command */
    s_command.Instruction = FLASH_INSTR_RESET;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (QSPI_BusyWait(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef     s_command;
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_WRITE_ENABLE;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    s_command.Instruction    = FLASH_INSTR_READ_SR1;
    s_command.DataMode       = QSPI_DATA_1_LINE;
    s_command.NbData         = 1;

    QSPI_AutoPollingTypeDef s_config;
    s_config.Match           = W25Q128JV_FSR_WREN;
    s_config.Mask            = W25Q128JV_FSR_WREN;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if(HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t QSPI_BusyWait(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef     s_command;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_READ_SR1;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    QSPI_AutoPollingTypeDef s_config;
    s_config.Match           = 0;//FLASH_BUSY_BIT_MASK;
    s_config.Mask            = QSPI_FLASH_BUSY_BIT_MASK;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

static amk_error_t QSPI_InitW25Qxx(QSPI_HandleTypeDef *hqspi)
{
    if (QSPI_ResetMemory(hqspi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    if (QSPI_WriteEnable(hqspi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    QSPI_CommandTypeDef s_command;
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_WRITE_SR2;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        return AMK_QSPI_ERROR;
    }

    uint8_t value = W25Q128JV_FSR_QE;
    if (HAL_QSPI_Transmit(hqspi, &value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        return AMK_QSPI_ERROR;
    }

    if (QSPI_BusyWait(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

/**
 * spi_stm32.c
 */

#include "amk_hal.h"
#include "spi.h"
#include "amk_printf.h"

#ifndef SPI_DEBUG
#define SPI_DEBUG 1
#endif

#if SPI_DEBUG
#define spi_debug  amk_printf
#else
#define spi_debug(...)
#endif

#define TIMEOUT_DEFAULT 100

#ifdef USE_SPI1
extern SPI_HandleTypeDef hspi1;
#endif

#ifdef USE_SPI2
extern SPI_HandleTypeDef hspi2;
#endif

#ifdef USE_SPI3
extern SPI_HandleTypeDef hspi3;
#endif

spi_handle_t ak_spi_init(spi_instance_t inst)
{
#ifdef USE_SPI1
    if (inst == SPI_INSTANCE_1) return &hspi1;
#endif

#ifdef USE_SPI2
    if (inst == SPI_INSTANCE_2) return &hspi2;
#endif

#ifdef USE_SPI3
    if (inst == SPI_INSTANCE_3) return &hspi3;
#endif

    return NULL;
}

bool ak_spi_ready(spi_handle_t spi)
{
    SPI_HandleTypeDef* hspi = (SPI_HandleTypeDef*)spi;
    return hspi->State == HAL_SPI_STATE_READY; 
}

amk_error_t ak_spi_send_async(spi_handle_t spi, const void *data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (ak_spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(hspi, (uint8_t *)data, length);
        if (status != HAL_OK) {
            spi_debug("Failed async spi transmit, error=%d\n", status);
            return AMK_SPI_ERROR;
        }
        return AMK_SUCCESS;
    } else {
        return AMK_SPI_BUSY;
    }
}

amk_error_t ak_spi_recv_async(spi_handle_t spi, void* data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (ak_spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_Receive_DMA(hspi, (uint8_t*)data, length);
        if (status != HAL_OK) {
            return AMK_SPI_ERROR;
        } 
        return AMK_SUCCESS;
    } else {
        return AMK_SPI_BUSY;
    }
}

amk_error_t ak_spi_xfer_async(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (ak_spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(hspi, (uint8_t*)tx_buffer, (uint8_t*)rx_buffer, length);
        if (status != HAL_OK) {
            return AMK_SPI_ERROR;
        }
        return AMK_SUCCESS;
    } else  {
        return AMK_SPI_BUSY;
    }
}

amk_error_t ak_spi_send(spi_handle_t spi, const void *data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, (uint8_t *)data, length, TIMEOUT_DEFAULT);
    if (status != HAL_OK) {
        spi_debug("spi send error: %d\n", status);
        return AMK_SPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t ak_spi_recv(spi_handle_t spi, void* data, size_t length)
{
    #if 1
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (ak_spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_Receive_DMA(hspi, (uint8_t *)data, length);
        if (status != HAL_OK) {
            spi_debug("Failed async spi transmit, error=%d\n", status);
            return AMK_SPI_ERROR;
        }
        while( !ak_spi_ready(spi));
        return AMK_SUCCESS;
    } else {
        return AMK_SPI_BUSY;
    }
    #else
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    HAL_StatusTypeDef status = HAL_SPI_Receive(hspi, (uint8_t*)data, length, TIMEOUT_DEFAULT);

    if (status != HAL_OK) {
        spi_debug("spi recv error: %d\n", status);
        return AMK_SPI_ERROR;
    }

    return AMK_SUCCESS;
    #endif
}

amk_error_t ak_spi_xfer(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(hspi, (uint8_t*)tx_buffer, (uint8_t*)rx_buffer, length, TIMEOUT_DEFAULT);
    if (status != HAL_OK) {
        spi_debug("spi xfer error: %d\n", status);
        return AMK_SPI_ERROR;
    }

    return AMK_SUCCESS;
}

void ak_spi_uninit(spi_handle_t spi)
{}

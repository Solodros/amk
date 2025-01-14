/**
 * spi.h
 * spi interface
 */

#pragma once

#include <stdbool.h>
#include "amk_error.h"

typedef enum {
    SPI_INSTANCE_0,
    SPI_INSTANCE_1,
    SPI_INSTANCE_2,
    SPI_INSTANCE_3,
} spi_instance_t;

typedef void *spi_handle_t;

spi_handle_t ak_spi_init(spi_instance_t inst);
bool ak_spi_ready(spi_handle_t spi);
amk_error_t ak_spi_send(spi_handle_t spi, const void *data, size_t length);
amk_error_t ak_spi_recv(spi_handle_t spi, void* data, size_t length);
amk_error_t ak_spi_xfer(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length);
amk_error_t ak_spi_send_async(spi_handle_t spi, const void *data, size_t length);
amk_error_t ak_spi_recv_async(spi_handle_t spi, void* data, size_t length);
amk_error_t ak_spi_xfer_async(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length);
void ak_spi_uninit(spi_handle_t spi);
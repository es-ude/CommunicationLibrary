//
// Created by Peter Zdankin on 01.03.18.
//

#ifndef COMMUNICATIONMODULE_SPIIMPL_H
#define COMMUNICATIONMODULE_SPIIMPL_H

#include "lib/include/SPI_Layer/SPI.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/platform/io.h"

typedef struct SPIConfig{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    enum sck_rate sck_rate;
    Allocator allocate;
} SPIConfig;

SPI * SPI_createSPI(SPIConfig config);

#endif //COMMUNICATIONMODULE_SPIIMPL_H

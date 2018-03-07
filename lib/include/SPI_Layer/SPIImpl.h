//
// Created by Peter Zdankin on 01.03.18.
//

#ifndef COMMUNICATIONMODULE_SPIIMPL_H
#define COMMUNICATIONMODULE_SPIIMPL_H

#include "lib/include/SPI_Layer/SPI.h"
#include "lib/include/RuntimeLibraryInterface.h"

SPI * SPI_createSPI(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *spcr, volatile uint8_t *spdr, uint8_t sck_rate, Allocator allocate);
void setSPCR(SPI *self, volatile uint8_t *SPCR);

#endif //COMMUNICATIONMODULE_SPIIMPL_H

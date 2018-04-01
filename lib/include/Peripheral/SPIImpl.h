//
// Created by Peter Zdankin on 01.03.18.
//

#ifndef COMMUNICATIONMODULE_SPIIMPL_H
#define COMMUNICATIONMODULE_SPIIMPL_H

#include "lib/include/Peripheral/PeripheralInterfaceImpl.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/platform/io.h"

typedef struct SPIConfig{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    enum sck_rate sck_rate;
    Allocator allocate;
    Deallocator deallocate;
    InterruptData *interruptData;
} SPIConfig;



PeripheralInterface * SPI_createSPI(SPIConfig config);

#endif //COMMUNICATIONMODULE_SPIIMPL_H

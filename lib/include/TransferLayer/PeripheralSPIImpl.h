//
// Created by Peter Zdankin on 01.04.18.
//

#ifndef COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H
#define COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H


#include "lib/include/TransferLayer/PeripheralInterface.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/platform/io.h"

typedef struct TransferLayerConfig {
    Allocator  allocate;
    Deallocator deallocate;
} TransferLayerConfig;

typedef struct SPIConfig{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *spcr;
    volatile uint8_t *spdr;
    volatile uint8_t *spsr;
    enum sck_rate sck_rate;
} SPIConfig;




PeripheralInterface * PeripheralInterface_create(TransferLayerConfig transferLayerConfig, SPIConfig spiConfig);

#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACEIMPL_H

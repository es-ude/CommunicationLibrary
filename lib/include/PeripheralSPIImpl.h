#ifndef PERIPHERALSPIIMPL_H
#define PERIPHERALSPIIMPL_H

#include "lib/include/Peripheral.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct SPIConfig {
  volatile uint8_t *ddr;
} SPIConfig;

Peripheral * createPeripheralSPI(SPIConfig *config, Allocator allocate);

#endif /* PERIPHERALSPIIMPL_H */

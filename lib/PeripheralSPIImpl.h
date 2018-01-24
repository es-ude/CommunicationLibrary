#ifndef PERIPHERALSPIIMPL_H
#define PERIPHERALSPIIMPL_H

#include "Peripheral.h"

typedef struct SPIConfig {
  volatile uint8_t *ddr;
} SPIConfig;

Peripheral * createPeripheralSPI(SPIConfig *config);

#endif /* PERIPHERALSPIIMPL_H */

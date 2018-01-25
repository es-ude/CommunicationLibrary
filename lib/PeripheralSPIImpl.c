#include "lib/PeripheralSPIImpl.h"

typedef struct PeripheralSPIImpl {
  Peripheral interface;
  volatile uint8_t *ddr;
} PeripheralSPIImpl;

Peripheral * createPeripheralSPI(SPIConfig *config, Allocator allocate) {
  PeripheralSPIImpl *implementation = allocate(sizeof(PeripheralSPIImpl));
  implementation->ddr = config->ddr;
  return (Peripheral*) implementation;
}

#include "lib/PeripheralSPIImpl.h"

typedef struct PeripheralSPIImpl {
  Peripheral interface;
  volatile uint8_t *ddr;
} PeripheralSPIImpl;

Peripheral * createPeripheralSPI(SPIConfig *config) {
  PeripheralSPIImpl *impl = malloc(sizeof(PeripheralSPIImpl));
  impl->ddr = config->ddr;
}

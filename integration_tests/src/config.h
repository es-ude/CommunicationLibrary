#ifndef COMMUNICATIONMODULE_CONFIG_H
#define COMMUNICATIONMODULE_CONFIG_H

#include "lib/include/Mac802154.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"

Mac802154 *mac802154;
PeripheralSPI mrf_spi_client;
PeripheralInterface peripheral_interface;

void setup(void);
void debug(uint8_t *string);

#endif //COMMUNICATIONMODULE_CONFIG_H

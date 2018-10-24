#ifndef COMMUNICATIONMODULE_CONFIG_H
#define COMMUNICATIONMODULE_CONFIG_H

#include "include/Mac802154.h"
#include "include/PeripheralSPIImpl.h"

Mac802154 *mac802154;
SPISlave mrf_spi_client;
PeripheralInterface peripheral_interface;

void setup(void);
void setUpPeripheral(void);
void debug(uint8_t *string);
void delay_microseconds(uint16_t microseconds);

#endif //COMMUNICATIONMODULE_CONFIG_H

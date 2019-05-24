#ifndef COMMUNICATIONMODULE_MRFMACSETUP_H
#define COMMUNICATIONMODULE_MRFMACSETUP_H

#include "CommunicationModule/CommunicationModule.h"

extern Mac802154 *mac802154;

extern SPISlave mrf_spi_client;
extern PeripheralInterface *peripheral_interface;


void
setUpMac(void);

void
setUpPeripheral(void);

void
delay_microseconds(uint16_t microseconds);

#endif //COMMUNICATIONMODULE_MRFMACSETUP_H

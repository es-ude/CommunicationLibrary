#ifndef PERIPHERALMOCK_H
#define PERIPHERALMOCK_H

#include "lib/include/Peripheral.h"


typedef struct PeripheralInterfaceMock {
    Peripheral interface;
    volatile uint8_t *ddr;
    volatile uint8_t *port;

} PeripheralInterfaceMock;

void initPeripheralInterfaceMock(Peripheral *peripheral);




#endif /* PERIPHERALMOCK_H */

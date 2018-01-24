#ifndef PERIPHERALMOCK_H
#define PERIPHERALMOCK_H

#include "lib/Peripheral.h"

Peripheral * createPeripheralInterfaceMock(void);

typedef struct PeripheralInterfaceMock {
  Peripheral interface;
} PeripheralInterfaceMock;


#endif /* PERIPHERALMOCK_H */

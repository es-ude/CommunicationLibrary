#ifndef PERIPHERALMOCK_H
#define PERIPHERALMOCK_H

#include "lib/include/Peripheral.h"


typedef struct MockPeripheralImpl {
  Peripheral interface;
  uint8_t *written_bytes;
  uint8_t written_bytes_capacity;
  uint8_t *read_bytes;
  uint8_t read_bytes_capacity;

} MockPeripheralImpl;

void MockPeripheralImpl_init(Peripheral *mock);



#endif /* PERIPHERALMOCK_H */

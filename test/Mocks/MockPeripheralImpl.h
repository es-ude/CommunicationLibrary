#ifndef PERIPHERALMOCK_H
#define PERIPHERALMOCK_H

#include "lib/include/Peripheral.h"
#include <stdbool.h>

typedef struct MockPeripheralImpl MockPeripheralImpl;

struct MockPeripheralImpl {
  Peripheral interface;
  uint8_t *written_bytes;
  const uint8_t *last_buffer_arg;
  uint8_t read_data;
};

void MockPeripheralImpl_init(Peripheral *mock);



#endif /* PERIPHERALMOCK_H */

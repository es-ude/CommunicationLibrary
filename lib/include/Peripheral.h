#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;

struct Peripheral {
  void (*writeByteNonBlocking)(Peripheral *self, uint8_t byte);
  void (*writeByteBlocking) (Peripheral *self, uint8_t byte);
  void (*writeBufferNonBlocking) (Peripheral *self, const uint8_t *buffer, uint16_t length);
  void (*writeBufferBlocking) (Peripheral *self, const uint8_t *output, uint16_t length);

  uint8_t (*readByteBlocking) (Peripheral *self);
  uint8_t (*readByteNonBlocking) (Peripheral *self);

};






#endif /* PERIPHERALINTERFACE_H */

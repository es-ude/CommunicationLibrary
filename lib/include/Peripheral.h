#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;

typedef void (*InterruptHandler) (void);

struct Peripheral {
  void (*writeByteNonBlocking) (Peripheral *self, uint8_t byte);
  void (*writeByteBlocking) (Peripheral *self, uint8_t byte);
  void (*writeBufferBlocking) (Peripheral *self, const uint8_t *buffer);
  void (*writeBufferNonBlocking) (Peripheral *self, const uint8_t *buffer);
  uint8_t (*readByte) (Peripheral *self);
  void (*setInterruptHandler) (Peripheral *self, InterruptHandler);
  void (*handleInterrupt) (Peripheral *self);
};






#endif /* PERIPHERALINTERFACE_H */

#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;

typedef void (*InterruptHandler) (void);

struct Peripheral {
  void (*writeByteNonBlocking) (Peripheral *self, uint8_t byte);
  void (*writeByteBlocking) (Peripheral *self, uint8_t byte);
  void (*writeSequenceBlocking) (Peripheral *self, const uint8_t *buffer, uint16_t length);
  void (*writeSequenceNonBlocking) (Peripheral *self, const uint8_t *buffer, uint16_t length);
  uint8_t (*readByteBlocking) (Peripheral *self);
  uint8_t (*readByteNonBlocking) (Peripheral *self);
  void (*readSequenceBlocking) (Peripheral *self, uint8_t *output, uint16_t length);
  void (*readSequenceNonBlocking) (Peripheral *self, uint8_t *output, uint16_t length);
  void (*setInterruptHandler) (Peripheral *self, InterruptHandler handle);
  void (*handleInterrupt) (Peripheral *self);
};






#endif /* PERIPHERALINTERFACE_H */

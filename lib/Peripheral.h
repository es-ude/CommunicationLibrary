#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;

typedef void (*InterruptHandler) (void);

struct Peripheral {
  void (*writeByte) (uint8_t byte);
  uint8_t (*readByte) (void);
  void (*setInterruptHandler) (InterruptHandler);
  void (*handleInterrupt) (void);
};


#endif /* PERIPHERALINTERFACE_H */

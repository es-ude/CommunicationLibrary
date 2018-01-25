#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;

typedef void (*InterruptHandler) (void);

struct Peripheral {
    void (*writeByte) (Peripheral *self, uint8_t byte);
    uint8_t (*readByte) (Peripheral *self);
    void (*setInterruptHandler) (Peripheral *self, InterruptHandler);
    void (*handleInterrupt) (Peripheral *self);
};






#endif /* PERIPHERALINTERFACE_H */

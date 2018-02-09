#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include "lib/include/Message.h"

typedef struct NetworkHardware NetworkHardware;

struct NetworkHardware {
    void (*setAddress16Bit) (NetworkHardware *self, uint16_t address);
    void (*setAddress64Bit) (NetworkHardware *self, uint8_t* address);
    void (*setPayload) (NetworkHardware *self, uint8_t* buffer, size_t size);
    void (*setPAN) (NetworkHardware *self, uint16_t pan_id);
    void (*receive) (NetworkHardware *self, Message *received_msg);
    void (*send) (NetworkHardware *self);
    void (*init) (NetworkHardware *self);
};

static inline void NetworkHardware_init(NetworkHardware *hardware) {
  hardware->init(hardware);
}

#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H

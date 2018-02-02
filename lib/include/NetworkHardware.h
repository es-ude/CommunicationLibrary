#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include "lib/include/Message.h"

typedef struct NetworkHardware NetworkHardware;

struct NetworkHardware {
    void (*setAddress16Bit) (NetworkHardware *self, uint16_t);
    void (*setAddress64Bit) (NetworkHardware *self, uint8_t*);
    void (*setPayload) (NetworkHardware *self, uint8_t* buffer, size_t size);
    void (*setPAN) (NetworkHardware *self, uint16_t);
    void (*receive) (NetworkHardware *self, Message *received_msg);
    void (*send) (NetworkHardware *self);
    void (*init) (NetworkHardware *self);
};

#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H

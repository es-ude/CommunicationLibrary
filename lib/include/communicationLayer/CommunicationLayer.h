//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_COMMUNICATIONLAYER_H
#define COMMUNICATIONMODULE_COMMUNICATIONLAYER_H

#include "stdbool.h"
#include "lib/include/communicationLayer/Message.h"

typedef struct CommunicationLayer CommunicationLayer;

struct CommunicationLayer{
    void (*init)(CommunicationLayer *self);
    bool (*isBusy)(CommunicationLayer *self);
    void (*transferSync)(CommunicationLayer *self, Message *m);
    void (*transferAsync)(CommunicationLayer *self);
    //The Interrupt Handler is always the same, therefore we shouldn't need to set it
    //void (*setInterruptHandler)(void (*handler)());
};

#endif //COMMUNICATIONMODULE_COMMUNICATIONLAYER_H

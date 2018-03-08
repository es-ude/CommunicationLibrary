//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_COMMUNICATIONLAYER_H
#define COMMUNICATIONMODULE_COMMUNICATIONLAYER_H

#include "stdbool.h"
#include "lib/include/communicationLayer/Message.h"
#include "lib/include/Interrupt.h"
#include "lib/include/SPI_Layer/SPI.h"

typedef struct CommunicationLayer CommunicationLayer;

struct CommunicationLayer{
    void (*init)(CommunicationLayer *self, SPI *spi);
    bool (*isBusy)(CommunicationLayer *self);
    void (*transferSync)(CommunicationLayer *self, Message *m);
    void (*transferAsync)(CommunicationLayer *self, Message *m);
    /**
     * Set the method to handle an interrupt
     */
    void (*setInterruptHandler)(CommunicationLayer *self, void (*handle)(InterruptData *id));

};

#endif //COMMUNICATIONMODULE_COMMUNICATIONLAYER_H

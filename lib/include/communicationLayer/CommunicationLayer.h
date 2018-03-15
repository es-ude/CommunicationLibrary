//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_COMMUNICATIONLAYER_H
#define COMMUNICATIONMODULE_COMMUNICATIONLAYER_H

#include "stdbool.h"
#include "lib/include/communicationLayer/Message.h"
#include "lib/include/communicationLayer/InterruptData.h"
#include "lib/include/Peripheral/PeripheralInterface.h"

typedef struct CommunicationLayer CommunicationLayer;

struct CommunicationLayer{
    /**
     * Initialize a CommunicationLayer, used to transmit Messages
     * If coming from an upper layer, use CL_createCommunicationLayer in CommunicationLayerImpl.h instead
     * @param self - Pointer to a CommunicationLayer
     * @param spi - Pointer to a PeripheralInterface
     */
    void (*init)(CommunicationLayer *self, PeripheralInterface *spi);
    /**
     * Returns whether the CommunicationLayer is currently busy with transmitting a message
     * @param self - The CommunicationLayer
     * @return - whether the Layer is busy
     */
    bool (*isBusy)(CommunicationLayer *self);
    /**
     * Transfer a Message synchronous, i.e. wait until everything is sent
     * @param self - The CommunicationLayer
     * @param m - A Message
     */
    void (*transferSync)(CommunicationLayer *self, Message *m);
    /**
     * Start an asynchronous Message transmission
     * Interrupts need to be enabled for this
     * @param self - The CommunicationLayer
     * @param m - A Message
     */
    void (*transferAsync)(CommunicationLayer *self, Message *m);
    /**
     * Set the method to handle an interrupt
     * @param self - The CommunicationLayer
     * @param handle - A function to be called when the previous byte was written
     */
    void (*setInterruptHandler)(CommunicationLayer *self, void (*handle)(PeripheralInterface *peripheralInterface));

    /**
     * Clean up the CommunicationLayer
     * @param self - The CommunicationLayer to clean up
     */
    void (*destroy)(CommunicationLayer *self);
};

#endif //COMMUNICATIONMODULE_COMMUNICATIONLAYER_H

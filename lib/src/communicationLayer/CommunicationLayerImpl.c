//
// Created by Peter Zdankin on 07.03.18.
//

#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayerImpl.h"
#include "lib/include/communicationLayer/InterruptData.h"

typedef struct CommunicationLayerImpl{
    CommunicationLayer communicationLayer;
    PeripheralInterface *peripheralInterface;
} CommunicationLayerImpl;


struct InterruptData{
    Message *m;
    PeripheralInterface *peripheral;
    bool busy;
};




static void init(CommunicationLayer *self, PeripheralInterface *spi);
static bool isBusy(CommunicationLayer *self);
static void transferSync(CommunicationLayer *self, Message *m);
static void transferAsync(CommunicationLayer *self, Message *m);
static void setInterruptHandler(CommunicationLayer *self, void (*handle)(PeripheralInterface*));


static bool busy = false;


static void handleInterrupt(PeripheralInterface *self);



CommunicationLayer *CL_createCommunicationLayer(PeripheralInterface *peripheral, Allocator allocate){
    CommunicationLayerImpl *communicationLayerImpl = allocate(sizeof(CommunicationLayerImpl));

    communicationLayerImpl->communicationLayer.init = init;
    communicationLayerImpl->communicationLayer.isBusy = isBusy;
    communicationLayerImpl->communicationLayer.transferSync = transferSync;
    communicationLayerImpl->communicationLayer.transferAsync = transferAsync;
    communicationLayerImpl->communicationLayer.setInterruptHandler = setInterruptHandler;
    communicationLayerImpl->peripheralInterface = peripheral;
    communicationLayerImpl->peripheralInterface->handleInterrupt = handleInterrupt;

    return (CommunicationLayer*)communicationLayerImpl;
}

void init(CommunicationLayer *self, PeripheralInterface *spi){
    spi->interruptData->busy = false;
}

bool isBusy(CommunicationLayer *self){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    return impl->peripheralInterface->interruptData->busy;
}

void transferAsync(CommunicationLayer *self, Message *m){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    impl->peripheralInterface->interruptData->m = m;
    impl->peripheralInterface->interruptData->peripheral = impl->peripheralInterface;
    impl->peripheralInterface->interruptData->busy = true;
}

//TODO test sync in a real environment
void transferSync(CommunicationLayer *self, Message *m){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    PeripheralInterface *peripheral = impl->peripheralInterface;
    Message *msg = peripheral->interruptData->m;

    int i;
    for(i = 0; i < msg->length; i++){
        m->inputBuffer[m->index] = peripheral->transfer(peripheral, m->outputBuffer[m->index]);
        ++(m->index);
    }
    peripheral->interruptData->busy = false;
}

static void handleInterrupt(PeripheralInterface *self){
    Message *m = self->interruptData->m;
    PeripheralInterface *peripheral = self->interruptData->peripheral;
    m->inputBuffer[m->index] = peripheral->read(peripheral);
    if(m->index < m->length){
        peripheral->write(peripheral,m->outputBuffer[++(m->index)]);
    }
    else{
        self->interruptData->busy = false;
    }
}


void setInterruptHandler(CommunicationLayer *self, void (*handle)(PeripheralInterface *self)){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    impl->peripheralInterface->handleInterrupt = handle;
}
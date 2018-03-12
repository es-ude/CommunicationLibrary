//
// Created by Peter Zdankin on 07.03.18.
//

#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayerImpl.h"
#include "lib/include/Interrupt.h"

typedef struct CommunicationLayerImpl{
    CommunicationLayer communicationLayer;
    PeripheralInterface *spi;
} CommunicationLayerImpl;

struct InterruptData{
    Message *m;
    PeripheralInterface *s;
};

InterruptData interruptData;



static void init(CommunicationLayer *self, PeripheralInterface *spi);
static bool isBusy(CommunicationLayer *self);
static void transferAsync(CommunicationLayer *self, Message *m);
static void setInterruptHandler(CommunicationLayer *self, void (*handle)(InterruptData *id));

static void handleInterrupt(InterruptData *id);

static bool busy = false;

CommunicationLayer *CL_createCommunicationLayer(PeripheralInterface *spi, Allocator allocate){
    CommunicationLayerImpl *communicationLayerImpl = allocate(sizeof(CommunicationLayerImpl));
    communicationLayerImpl->communicationLayer.init = init;
    communicationLayerImpl->communicationLayer.isBusy = isBusy;
    communicationLayerImpl->communicationLayer.transferAsync = transferAsync;
    communicationLayerImpl->communicationLayer.setInterruptHandler = setInterruptHandler;
    communicationLayerImpl->spi = spi;
    communicationLayerImpl->spi->handleInterrupt = handleInterrupt;

    return (CommunicationLayer*)communicationLayerImpl;
}

void init(CommunicationLayer *self, PeripheralInterface *spi){
    busy = false;
}

bool isBusy(CommunicationLayer *self){
    return busy;
}

void transferAsync(CommunicationLayer *self, Message *m){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    interruptData.m = m;
    interruptData.s = impl->spi;
    busy = true;
}

static void handleInterrupt(InterruptData *id){
    id->m->inputBuffer[id->m->index] = id->s->read(id->s);
    if(id->m->index < id->m->length){
        id->s->write(id->s,id->m->outputBuffer[++(id->m->index)]);
    }
    else{
        busy = false;
    }
}


void setInterruptHandler(CommunicationLayer *self, void (*handle)(InterruptData *id)){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    impl->spi->handleInterrupt = handle;
}
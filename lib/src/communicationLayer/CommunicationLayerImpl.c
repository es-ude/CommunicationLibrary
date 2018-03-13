//
// Created by Peter Zdankin on 07.03.18.
//

#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayerImpl.h"
#include "lib/include/communicationLayer/InterruptData.h"

typedef struct CommunicationLayerImpl{
    CommunicationLayer communicationLayer;
    PeripheralInterface *spi;
} CommunicationLayerImpl;

struct InterruptData{
    Message *m;
    PeripheralInterface *peripheral;
};

static InterruptData interruptData;


static void init(CommunicationLayer *self, PeripheralInterface *spi);
static bool isBusy(CommunicationLayer *self);
static void transferSync(CommunicationLayer *self, Message *m);
static void transferAsync(CommunicationLayer *self, Message *m);
static void setInterruptHandler(CommunicationLayer *self, void (*handle)(void));


static bool busy = false;


static void handleInterrupt();



CommunicationLayer *CL_createCommunicationLayer(PeripheralInterface *spi, Allocator allocate){
    CommunicationLayerImpl *communicationLayerImpl = allocate(sizeof(CommunicationLayerImpl));
    communicationLayerImpl->communicationLayer.init = init;
    communicationLayerImpl->communicationLayer.isBusy = isBusy;
    communicationLayerImpl->communicationLayer.transferSync = transferSync;
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
    interruptData.peripheral = impl->spi;
    busy = true;
}

//If user thinks he can transfer without interrupts, he is wrong
void transferSync(CommunicationLayer *self, Message *m){
    transferAsync(self,m);
    while(busy){
        //PROCRASTINATE
    }
}

static void handleInterrupt(){
    Message *m = interruptData.m;
    PeripheralInterface *peripheral = interruptData.peripheral;
    m->inputBuffer[m->index] = peripheral->read(peripheral);
    if(m->index < m->length){
        peripheral->write(peripheral,m->outputBuffer[++(m->index)]);
    }
    else{
        busy = false;
    }
}


void setInterruptHandler(CommunicationLayer *self, void (*handle)(void)){
    CommunicationLayerImpl *impl = (CommunicationLayerImpl *) self;
    impl->spi->handleInterrupt = handle;
}
//
// Created by Peter Zdankin on 02.03.18.
//

#include "lib/include/communicationLayer/SPICommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct CommunicationLayerImpl{
    CommunicationLayer *communicationLayer;
} CommunicationLayerImpl;



CommunicationLayer * SPI_createCommunicationLayer(SPI *self, Allocator allocate){
    CommunicationLayerImpl *commImpl = allocate(sizeof(CommunicationLayerImpl));
}
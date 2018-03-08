//
// Created by Peter Zdankin on 07.03.18.
//

#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/communicationLayer/CommunicationLayerImpl.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct CommunicationLayerImpl{
    CommunicationLayer *communicationLayer;
} CommunicationLayerImpl;


CommunicationLayer *CL_createCommunicationLayer(Allocator allocate){
    CommunicationLayerImpl *communicationLayerImpl = allocate(sizeof(CommunicationLayerImpl));
}
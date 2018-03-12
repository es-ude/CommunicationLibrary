//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_SPICOMMUNICATIONLAYERIMPL_H_H
#define COMMUNICATIONMODULE_SPICOMMUNICATIONLAYERIMPL_H_H


#include "lib/include/Peripheral/PeripheralInterface.h"
#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/RuntimeLibraryInterface.h"

//DEPRACATED

CommunicationLayer * SPI_createCommunicationLayer(PeripheralInterface *self, Allocator allocate);

#endif //COMMUNICATIONMODULE_SPICOMMUNICATIONLAYERIMPL_H_H

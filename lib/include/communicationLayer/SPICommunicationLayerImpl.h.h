//
// Created by Peter Zdankin on 02.03.18.
//

#ifndef COMMUNICATIONMODULE_SPICOMMUNICATIONLAYERIMPL_H_H
#define COMMUNICATIONMODULE_SPICOMMUNICATIONLAYERIMPL_H_H


#include "lib/include/SPI_Layer/SPI.h"
#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/RuntimeLibraryInterface.h"

CommunicationLayer * SPI_createCommunicationLayer(SPI *self, Allocator allocate);

#endif //COMMUNICATIONMODULE_SPICOMMUNICATIONLAYERIMPL_H_H

//
// Created by Peter Zdankin on 07.03.18.
//

#ifndef COMMUNICATIONMODULE_COMMUNICATIONLAYERIMPL_H
#define COMMUNICATIONMODULE_COMMUNICATIONLAYERIMPL_H

#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/SPI_Layer/SPI.h"

CommunicationLayer *CL_createCommunicationLayer(SPI *spi, Allocator allocate);

#endif //COMMUNICATIONMODULE_COMMUNICATIONLAYERIMPL_H

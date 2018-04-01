//
// Created by Peter Zdankin on 07.03.18.
//

#ifndef COMMUNICATIONMODULE_COMMUNICATIONLAYERIMPL_H
#define COMMUNICATIONMODULE_COMMUNICATIONLAYERIMPL_H

#include "lib/include/communicationLayer/CommunicationLayer.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/Peripheral/PeripheralInterfaceImpl.h"

typedef struct CommunicationLayerConfig {
    PeripheralInterface *peripheralInterface;
    Allocator  allocate;
    Deallocator deallocate;
} CommunicationLayerConfig;


/**
 * Create a CommunicationLayer with the specified PeripheralInterface and an allocationfunction
 * @param config - Configuration of the CommunicationLayer
 * @return - A Pointer to a CommunicationLayer
 */
CommunicationLayer *CL_createCommunicationLayer(CommunicationLayerConfig config);

#endif //COMMUNICATIONMODULE_COMMUNICATIONLAYERIMPL_H

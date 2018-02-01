#ifndef COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H
#define COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H

#include "lib/include/NetworkHardware.h"
#include "lib/include/Peripheral.h"
#include "lib/include/RuntimeLibraryInterface.h"

NetworkHardware *NetworkHardware_createMRF(Peripheral *peripheral, Allocator allocate);

#endif //COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H

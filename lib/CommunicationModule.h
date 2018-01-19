#ifndef COMMUNICATIONMODULE_H
#define COMMUNICATIONMODULE_H

#include "lib/RuntimeLibraryInterface.h"

typedef struct CommunicationModule CommunicationModule;

CommunicationModule * CommunicationModule_create(AllocationFunction allocate);

#endif /* COMMUNICATIONMODULE_H */

#ifndef COMMUNICATIONMODULE_H
#define COMMUNICATIONMODULE_H

#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/include/Message.h"

typedef struct CommunicationModule CommunicationModule;

CommunicationModule * CommunicationModule_create(AllocationFunction allocate);

struct CommunicationModule {
  void (*sendNonBlocking) (Message*);
};

#endif /* COMMUNICATIONMODULE_H */

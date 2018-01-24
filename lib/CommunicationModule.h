#ifndef COMMUNICATIONMODULE_H
#define COMMUNICATIONMODULE_H

#include "lib/RuntimeLibraryInterface.h"
#include "lib/Message.h"

typedef struct CommunicationModule CommunicationModule;

CommunicationModule * CommunicationModule_create(AllocationFunction allocate);

struct CommunicationModule {
  void (*sendNonBlocking) (Message*);
};

#endif /* COMMUNICATIONMODULE_H */

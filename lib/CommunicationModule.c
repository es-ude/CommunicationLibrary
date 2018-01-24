#include "lib/CommunicationModule.h"

static void send(Message*);

CommunicationModule * CommunicationModule_create(AllocationFunction allocate){
  CommunicationModule *module = allocate(sizeof(CommunicationModule));
  module->sendNonBlocking = send;
  return module;
}

void send(Message* message) {
  
}


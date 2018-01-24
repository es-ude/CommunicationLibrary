#include "lib/CommunicationModule.h"


static void sendNonBlocking(Message*);

CommunicationModule * CommunicationModule_create(AllocationFunction allocate){
  CommunicationModule *module = allocate(sizeof(CommunicationModule));
  module->sendNonBlocking = sendNonBlocking;
  return module;
}

void sendNonBlocking(Message *message) {

}

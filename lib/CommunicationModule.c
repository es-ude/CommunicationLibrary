#include "lib/CommunicationModule.h"

struct CommunicationModule {};

CommunicationModule * CommunicationModule_create(AllocationFunction allocate){
  CommunicationModule *module = allocate(sizeof(CommunicationModule));
  return module;
}

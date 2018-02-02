#include "lib/include/NetworkHardwareMRFImpl.h"


typedef struct NetworkHardwareMRFImpl NetworkHardwareMRFImpl;

struct NetworkHardwareMRFImpl {
  NetworkHardware interface;
  Peripheral *output;
};

static void init(NetworkHardware *self);

NetworkHardware *NetworkHardware_createMRF(Peripheral *output, Allocator allocate) {
  NetworkHardwareMRFImpl *impl = allocate(sizeof(NetworkHardwareMRFImpl));
  impl->output = output;
  NetworkHardware *interface = (NetworkHardware*) impl;
  interface->init = init;
  return interface;
}

void init(NetworkHardware *self) {

}
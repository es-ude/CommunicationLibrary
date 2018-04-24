#include "MRFState.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"

void MRFSendContext_setExtendedAddress(MRFSendContext *self, uint64_t address) {}
void MRFSendContext_setShortAddress(MRFSendContext *self, uint16_t address){}
void MRFSendContext_setPayload(MRFSendContext *self, const uint8_t *payload, uint16_t size){}
void MRFSendContext_setPeripheral(MRFSendContext *self, Peripheral *device){
  self->device = device;
}
void MRFSendContext_setPeripheralInterface(MRFSendContext *self, PeripheralInterface *interface){
  self->hw_interface = interface;
}
void MRFSendContext_setPanId(MRFSendContext *self, uint16_t pan_id){}

void some_function(void *args) {}

void MRFSendContext_runSendSequence(MRFSendContext *self){
  PeripheralInterface_selectPeripheral(self->hw_interface, self->device);
  uint8_t command[] = {MRF_writeLongCommandHighByte(0), MRF_writeLongCommandLowByte(0)};
  PeripheralInterface_writeNonBlocking(self->hw_interface, command, 2);
  MRFWriteCallback callback = {.function = some_function, .argument= NULL};
  PeripheralInterface_setWriteCallback(self->hw_interface, callback);
}

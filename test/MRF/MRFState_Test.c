#include "unity.h"
#include "lib/include/MockPeripheral.h"
#include "lib/src/Mac802154/MRF/MRFState.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"


MRFWriteCallback current_write_callback;

void setWriteCallbackStub(PeripheralInterface *self, MRFWriteCallback callback, int times_called){
  current_write_callback = callback;
}

void test_writeEveryField(void) {
  MRFSendContext data;

  MRFSendContext_setExtendedAddress(&data, 0);
  MRFSendContext_setPanId(&data, 0);
  uint8_t payload[] = "foobar123";
  MRFSendContext_setPayload(&data, payload, 9);
  PeripheralInterface_setWriteCallback_Ignore();
//  PeripheralInterface_setWriteCallback_StubWithCallback(setWriteCallbackStub);
  PeripheralInterface_selectPeripheral_Expect(data.hw_interface, data.device);
  uint8_t command[] = {MRF_writeLongCommandHighByte(0), MRF_writeLongCommandLowByte(0)};
  PeripheralInterface_writeNonBlocking_ExpectWithArray(data.hw_interface, 1, command, 2, 2);

  MRFSendContext_runSendSequence(&data);
}

void test_writeEveryFieldWithCallback(void) {
  MRFSendContext data;

  MRFSendContext_setExtendedAddress(&data, 0);
  MRFSendContext_setPanId(&data, 0);
  uint8_t payload[] = "foobar123";
  MRFSendContext_setPayload(&data, payload, 9);
  PeripheralInterface_setWriteCallback_StubWithCallback(setWriteCallbackStub);
  PeripheralInterface_selectPeripheral_Ignore();
  uint8_t command[] = {MRF_writeLongCommandHighByte(0), MRF_writeLongCommandLowByte(0)};
  PeripheralInterface_writeNonBlocking_ExpectWithArray(data.hw_interface, 1, command, 2, 2);
  MRFSendContext_runSendSequence(&data);
  current_write_callback.function(current_write_callback.argument);
}
#include "unity.h"
#include "lib/include/MockPeripheral.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"

void test_setAddressForShortAddress(void){
  MRF mrf;
  uint8_t command = MRF_writeShortCommand(mrf_register_software_reset);
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &command, 1, 1);
  uint8_t value = 0xAB;
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_setControlRegister(&mrf, mrf_register_software_reset, 0xAB);
}

void test_setAddressForLongAddress(void) {
  MRF mrf;
  uint8_t command[] = {
          MRF_writeLongCommandHighByte(mrf_register_rf_control6),
          MRF_writeLongCommandLowByte(mrf_register_rf_control6),
  };
  uint8_t value;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_setControlRegister(&mrf, mrf_register_rf_control6, value);
}

void test_writeBytesToShortRegisterAddress(void) {
  MRF mrf;
  uint8_t command = MRF_writeShortCommand(mrf_register_short_address_low_byte);
  uint8_t buffer[10];
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &command, 1, 1);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, buffer, 10, 10);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_writeBlockingToShortAddress(&mrf, mrf_register_short_address_low_byte, buffer, 10);
}

static PeripheralCallback current_write_callback;

static void setWriteCallbackStub(PeripheralInterface *self, PeripheralCallback callback, int times_called)
{
    current_write_callback = callback;
}

/**
 * Because the SPI implementation that will be used for the PeripheralInterface
 * writes the first byte immediately, the used command buffer does not have to
 * exist outside the stack of the function that calls Peripheral_writeNonBlocking.
 */
void test_TriggerSendNonBlocking(void) {
  MRF mrf;
  uint8_t command = MRF_writeShortCommand(mrf_register_tx_normal_fifo_control);
  uint8_t value = 1;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_setWriteCallback_ExpectAnyArgs();

  PeripheralInterface_writeNonBlocking_ExpectWithArray(mrf.interface, 1, &command, 1, 1);
  MRF_triggerTXSendNonBlocking(&mrf);
}

void test_triggerNonBlockingSendIncludingCallback(void) {
  MRF mrf;
  uint8_t command = MRF_writeShortCommand(mrf_register_tx_normal_fifo_control);
  uint8_t value = 1;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_setWriteCallback_StubWithCallback(setWriteCallbackStub);
  PeripheralInterface_writeNonBlocking_ExpectWithArray(mrf.interface, 1, &command, 1, 1);
  PeripheralInterface_writeNonBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_triggerTXSendNonBlocking(&mrf);
  current_write_callback.function(current_write_callback.argument);
  TEST_ASSERT_NULL(current_write_callback.function);
  TEST_ASSERT_NULL(current_write_callback.argument);
}

/**
 * A call to the send function of the Mac802154 interface should trigger
 * a chain of non blocking write calls to set up the necessary fields of the
 * packet.
 * Idea for non blocking write chain:
 * Each callback sets up the next callback based on the current internal state of the mrf module.
 * For unit-testing just check that the transitions from state to state are correct.
 */
void test_writeNonBlockingToLongAddress(void) {
  MRF mrf;
  uint16_t address = 0;
  const uint8_t buffer[] = {0xAB, 0xCD};
  uint16_t size = 2;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  MRF_writeNonBlockingToLongAddress(&mrf, address, buffer, 2);
}

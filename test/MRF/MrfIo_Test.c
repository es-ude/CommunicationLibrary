#include "unity.h"
#include "src/Mac802154/MRF/MrfIo.h"
#include "PeripheralInterface/MockPeripheralInterface.h"
#include "src/Mac802154/MRF/MRFHelperFunctions.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"

#define TEST_ASSERT_EQUAL_CALLBACK(expected, actual)\
  TEST_ASSERT_EQUAL_PTR_MESSAGE(expected.function, actual.function, "Callback: function differs");\
  TEST_ASSERT_EQUAL_PTR_MESSAGE(expected.argument, actual.argument, "Callback.argument differs");\

#define TEST_ASSERT_MRFIO_WRITE_CONTEXT_EQUAL(expected, actual) \
  TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected.length, actual.length, "MrfIo_NonBlockingWriteContext length field differs");\
  TEST_ASSERT_EQUAL_PTR_MESSAGE(expected.output_buffer, actual.output_buffer, "MrfIo_NonBlockingWriteContext output_buffer differs");\
  TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected.address, actual.address, "MrfIo_NonBlockingWriteContext address differs");\
  TEST_ASSERT_EQUAL_CALLBACK(expected.callback, actual.callback);

static void captureWriteContext(PeripheralInterface *interface,
                                 PeripheralInterface_NonBlockingWriteContext context,
                                 int number_of_calls);

static PeripheralInterface_NonBlockingWriteContext last_write_context;

void debug(const uint8_t *message) {}

void test_writeBlockingToLongAddress(void) {
  MrfIo mrf;
  uint8_t size = 2;
  uint8_t payload[size];
  uint16_t address = 19;
  uint8_t command[2] = {MRF_writeLongCommandFirstByte(address),
                        MRF_writeLongCommandSecondByte(address)};
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, payload, size, size);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_writeBlockingToLongAddress(&mrf, payload, size, address);
}

void test_writeBlockingToShortAddress(void) {
  MrfIo mrf;
  uint8_t size = 5;
  uint8_t address = 1;
  uint8_t payload[5] = {0,1,2,3,4};
  uint8_t commands[size];
  for (uint8_t i=0; i<size; i++) {
    commands[i] = MRF_writeShortCommand(address+i);
  }
  for (uint8_t i=0; i<size; i++) {
    PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
    PeripheralInterface_writeBlocking_Expect(mrf.interface, commands+i, 1);
    PeripheralInterface_writeBlocking_Expect(mrf.interface, payload+i, 1);
    PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  }
  MrfIo_writeBlockingToShortAddress(&mrf, payload, size, address);
}

void test_writeNonBlockingToShortAddressCheckingBothCallbacks(void) {
  MrfIo mrf;
  mrf.callback.function = NULL;
  uint8_t size = 20;
  uint8_t payload[size];
  uint8_t address;
  uint8_t command[1] = {MRF_writeShortCommand(address)};
  MrfIo_NonBlockingWriteContext context = {
      .output_buffer = payload,
      .length = size,
      .callback = NULL,
      .address = address,
  };
  PeripheralInterface_writeNonBlocking_StubWithCallback(captureWriteContext);
  PeripheralInterface_NonBlockingWriteContext expected_peripheral_context =
      {
          .length = 1,
          .output_buffer = command,
      };
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_writeNonBlockingToShortAddress(&mrf, context);
  TEST_ASSERT_EQUAL_UINT16(expected_peripheral_context.length, last_write_context.length);
  TEST_ASSERT_EQUAL_UINT8(expected_peripheral_context.output_buffer[0], last_write_context.output_buffer[0]);
  //TODO: convert tests to new writeNonBlocking function signature
//  PeripheralInterface_writeNonBlocking_Expect(mrf.interface, payload, size);
//  last_write_context.function(last_write_context.argument);
//
//  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
//  last_write_context.function(last_write_context.argument);
//
//  TEST_ASSERT_NULL(last_write_context.function);
//  TEST_ASSERT_NULL(last_write_context.argument);
}

//TODO: convert tests to new writeNonBlocking function signature
//void test_writeNonBlockingToLongAddress(void) {
//  MrfIo mrf;
//  mrf.callback.function = NULL;
//  uint8_t payload_size = 13;
//  uint8_t payload[payload_size];
//  uint16_t address;
//  uint8_t command_size = 2;
//  uint8_t command[] = {
//          MRF_writeLongCommandFirstByte(address),
//          MRF_writeLongCommandSecondByte(address)
//  };
//  PeripheralInterface_NonBlockingWriteContext context = {
//      .callback.function = NULL,
//      .output_buffer = payload,
//      .length = payload_size,
//  };
//
//  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
//  PeripheralInterface_writeNonBlocking_Expect(mrf.interface, context);
//
//  MrfIo_writeNonBlockingToLongAddress(&mrf, payload, payload_size, address);
//
//  PeripheralInterface_writeNonBlocking_Expect(mrf.interface, context);
//  last_write_context.function(last_write_context.argument);
//
//  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
//  last_write_context.function(last_write_context.argument);
//
//}

static void writeCallback(void *arg) {
  *(bool *) arg = true;
}

//TODO: convert tests to new writeNonBlocking function signature
//void test_writeNonBlockingToLongAddressTwoTimesUsingCallback(void) {
//  MrfIo mrf;
//  uint8_t first_payload_size = 24;
//  uint8_t first_payload[first_payload_size];
//  uint16_t address;
//  uint8_t command_size = 2;
//  uint8_t command[] = {
//          MRF_writeLongCommandFirstByte(address),
//          MRF_writeLongCommandSecondByte(address)
//  };
//  uint8_t second_payload_size = 16;
//  uint8_t second_payload[second_payload_size];
//  bool write_callback_called = false;
//  MrfIoCallback callback = {
//          .function = writeCallback,
//          .argument = &write_callback_called,
//  };
//  MrfIo_setWriteCallback(&mrf, callback);
//  PeripheralInterface_setWriteCallback_StubWithCallback (captureWriteContext);
//
//  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
//  PeripheralInterface_writeNonBlocking_Expect(mrf.interface, command, command_size);
//
//  MrfIo_writeNonBlockingToLongAddress(&mrf, first_payload, first_payload_size, address);
//
//  PeripheralInterface_writeNonBlocking_Expect(mrf.interface, first_payload, first_payload_size);
//  last_write_context.function(last_write_context.argument);
//
//  TEST_ASSERT_FALSE(write_callback_called);
//  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
//}

void captureWriteContext (PeripheralInterface *interface, PeripheralInterface_NonBlockingWriteContext context, int number_of_calls) {
   last_write_context = context;
}

void check_setControlRegister(uint16_t register_address, const uint8_t *command, uint8_t command_length) {
  MrfIo mrf;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, command_length, command_length);
  uint8_t value;
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, &value, 1, 1);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_setControlRegister(&mrf, register_address, value);
}

void test_setShortAddressRegisterValue(void){
  uint8_t command = MRF_writeShortCommand(mrf_register_software_reset);
  check_setControlRegister(mrf_register_software_reset, &command, 1);
}

void test_setLongAddressRegisterValue(void) {
  uint8_t command[] = {
          MRF_writeLongCommandFirstByte(mrf_register_rf_control6),
          MRF_writeLongCommandSecondByte(mrf_register_rf_control6),
  };
  check_setControlRegister(mrf_register_rf_control6, command, 2);
}

void check_readAddressControlRegister(uint16_t register_address, const uint8_t *command, uint8_t command_length) {
  MrfIo mrf;
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, command_length, command_length);
  uint8_t value = 0xAB;
  PeripheralInterface_readBlocking_Expect(mrf.interface, &value, 1);
  PeripheralInterface_readBlocking_IgnoreArg_buffer();
  PeripheralInterface_readBlocking_ReturnThruPtr_buffer(&value);
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  uint8_t expected = MrfIo_readControlRegister(&mrf, register_address);
  TEST_ASSERT_EQUAL_HEX8(expected, value);
}

void test_readShortAddressControlRegister(void) {
  uint8_t command = MRF_readShortCommand(mrf_register_interrupt_status);
  check_readAddressControlRegister(mrf_register_interrupt_status, &command, 1);
}

void test_readLongAddressControlRegister(void) {
  uint8_t command[] = {MRF_readLongCommandFirstByte(mrf_register_rf_control6),
          MRF_readLongCommandSecondByte(mrf_register_rf_control6)};
  check_readAddressControlRegister(mrf_register_rf_control6, command, 2);
}

void test_readBlockingFromLongAddress(void) {
  MrfIo mrf;
  uint8_t buffer;
  uint8_t expected_buffer = 0xC9;
  uint8_t command[] = {
          MRF_readLongCommandFirstByte(mrf_rx_fifo_start),
          MRF_readLongCommandSecondByte(mrf_rx_fifo_start),
  };
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_readBlocking_ExpectWithArray(mrf.interface, 1, &buffer, 1, 1);
  PeripheralInterface_readBlocking_ReturnThruPtr_buffer(&expected_buffer);
  PeripheralInterface_readBlocking_IgnoreArg_buffer();
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_readBlockingFromLongAddress(&mrf, mrf_rx_fifo_start, &buffer, 1);
  TEST_ASSERT_EQUAL_HEX8(expected_buffer, buffer);
}

void test_readBlockingFromLongAddress2(void) {
  MrfIo mrf;
  uint8_t buffer[3];
  uint8_t expected_buffer[] = {0xC9, 0xAB, 0x14};
  uint8_t command[] = {
          MRF_readLongCommandFirstByte(mrf_rx_fifo_start),
          MRF_readLongCommandSecondByte(mrf_rx_fifo_start),
  };
  PeripheralInterface_selectPeripheral_Expect(mrf.interface, mrf.device);
  PeripheralInterface_writeBlocking_ExpectWithArray(mrf.interface, 1, command, 2, 2);
  PeripheralInterface_readBlocking_ExpectWithArray(mrf.interface, 1, &buffer, 3, 3);
  PeripheralInterface_readBlocking_ReturnThruPtr_buffer(&expected_buffer);
  PeripheralInterface_readBlocking_IgnoreArg_buffer();
  PeripheralInterface_deselectPeripheral_Expect(mrf.interface, mrf.device);
  MrfIo_readBlockingFromLongAddress(&mrf, mrf_rx_fifo_start, &buffer, 3);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer, 3);
}

#include "src/unity.h"
#include "test/Mocks/MockPeripheralInterface.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "lib/include/Exception.h"
#include <string.h>

#define READ_BUFFER_SIZE 64
#define WRITE_BUFFER_SIZE 128

static uint8_t read_buffer[READ_BUFFER_SIZE];
static uint8_t write_buffer[WRITE_BUFFER_SIZE];
static MockPeripheral mock_device = {
        .read_buffer = read_buffer,
        .write_buffer = write_buffer,
        .write_buffer_size = WRITE_BUFFER_SIZE,
        .read_buffer_size = READ_BUFFER_SIZE,
};
static PeripheralInterface *interface;
static Peripheral *device = (Peripheral*) &mock_device;
static MemoryManagement *dynamic_memory;

void setUp(void) {
  dynamic_memory = MemoryManagement_createMockImpl();
  interface = PeripheralInterface_createMockImpl(dynamic_memory);
  memset(read_buffer, 0, READ_BUFFER_SIZE);
  memset(write_buffer, 0, WRITE_BUFFER_SIZE);
}

void tearDown(void) {
  interface->destroy(interface);
  TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(dynamic_memory));
}

void test_writeTwoCharacters(void) {
  interface->selectPeripheral(interface, device);
  interface->write(interface, 'A');
  TEST_ASSERT_EQUAL_UINT8('A', write_buffer[0]);
  interface->write(interface, 'B');
  TEST_ASSERT_EQUAL_UINT8('B', write_buffer[1]);
}


void test_writeBeyondBufferBound(void) {
  interface->selectPeripheral(interface, device);
  for (uint8_t i = 0; i < WRITE_BUFFER_SIZE; i++){
    interface->write(interface, 'A');
  }
  interface->write(interface, 'B');
  TEST_ASSERT_EQUAL_UINT8('B', write_buffer[0]);
}

void test_readTwoCharacters(void) {
  interface->selectPeripheral(interface, device);
  read_buffer[0] = 'A';
  read_buffer[1] = 'B';
  TEST_ASSERT_EQUAL_UINT8('A', interface->read(interface));
  TEST_ASSERT_EQUAL_UINT8('B', interface->read(interface));
}

void test_readBeyondBounds(void) {
  interface->selectPeripheral(interface, device);
  read_buffer[0] = 'A';
  for (uint8_t i = 0; i < READ_BUFFER_SIZE; i++) {
    interface->read(interface);
  }
  TEST_ASSERT_EQUAL_UINT8('A', interface->read(interface));
}

void test_selectWithoutDeselect(void) {
  MockPeripheral second_mock_device;
  interface->selectPeripheral(interface, device);
  CEXCEPTION_T exception = NO_EXCEPTION;
  Try {
        interface->selectPeripheral(interface, (Peripheral *) &second_mock_device);
      } Catch(exception) {}
  TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, exception);
}

void test_selectWithDeselect(void) {
  CEXCEPTION_T exception = NO_EXCEPTION;
  Try {
        MockPeripheral second_mock_device;
        PeripheralInterface_selectPeripheral(interface, &second_mock_device);
        PeripheralInterface_deselectPeripheral(interface, &second_mock_device);
        PeripheralInterface_selectPeripheral(interface, device);
      } Catch(exception) {}
  TEST_ASSERT_EQUAL_UINT8(NO_EXCEPTION, exception);
}

void test_writeToTwoDevices(void) {
  uint8_t second_read_buffer_size = 8;
  uint8_t second_read_buffer[second_read_buffer_size];
  uint8_t second_write_buffer_size = 8;
  uint8_t second_write_buffer[second_write_buffer_size];
  MockPeripheral second_mock_device = {
          .write_buffer_size = second_write_buffer_size,
          .read_buffer_size = second_read_buffer_size,
          .write_buffer = second_write_buffer,
          .read_buffer = second_read_buffer,
  };
  Peripheral *second_device = (Peripheral *)&second_mock_device;
  interface->selectPeripheral(interface, device);
  interface->write(interface, 'A');
  interface->deselectPeripheral(interface, device);
  interface->selectPeripheral(interface, second_device);
  interface->write(interface, 'A');
  TEST_ASSERT_EQUAL_UINT8('A', second_write_buffer[0]);
}

void test_readFromDeviceTwoTimes(void) {
  PeripheralInterface_selectPeripheral(interface, device);
  read_buffer[0] = 'A';
  read_buffer[1] = 'B';
  PeripheralInterface_read(interface);
  PeripheralInterface_selectPeripheral(interface, device);
  TEST_ASSERT_EQUAL_UINT8('A', PeripheralInterface_read(interface));
}

void test_writeToDeviceTwoTimes(void) {
  PeripheralInterface_selectPeripheral(interface, device);
  PeripheralInterface_write(interface, 'A');
  PeripheralInterface_selectPeripheral(interface, device);
  PeripheralInterface_write(interface, 'B');
  TEST_ASSERT_EQUAL_UINT8('B', write_buffer[0]);
}

void test_writeWithoutSelect(void) {
  CEXCEPTION_T exception = NO_EXCEPTION;
  Try {
        PeripheralInterface_write(interface, 0);

      } Catch(exception) {}
  TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, exception);
}

void test_readWithoutSelect(void) {
  CEXCEPTION_T exception = NO_EXCEPTION;
  Try {
        PeripheralInterface_read(interface);

      } Catch(exception) {}
  TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, exception);
}
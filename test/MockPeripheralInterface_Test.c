#include "src/unity.h"
#include "test/Mocks/MockPeripheralInterface.h"
#include <stdlib.h>
#include <string.h>

#define READ_BUFFER_SIZE 64
#define WRITE_BUFFER_SIZE 128

static uint8_t read_buffer[READ_BUFFER_SIZE];
static uint8_t write_buffer[WRITE_BUFFER_SIZE];
static MockPeripheral mock_device = {
        .interface = NULL,
        .read_buffer = read_buffer,
        .write_buffer = write_buffer,
        .write_buffer_size = WRITE_BUFFER_SIZE,
        .read_buffer_size = READ_BUFFER_SIZE,
};
static PeripheralInterface *interface;
static Peripheral *device = (Peripheral*) &mock_device;

void setUp(void) {
  MemoryManagement memory_managers = {
          .allocate = malloc,
          .deallocate = free,
  };
  interface = PeripheralInterface_createMockImpl(&memory_managers);
  memset(read_buffer, 0, READ_BUFFER_SIZE);
  memset(write_buffer, 0, WRITE_BUFFER_SIZE);
}

void tearDown(void) {
  interface->destroy(interface);
}

void test_writeTwoCharacters(void) {
  interface->selectPeripheral(interface, device);
  interface->write(device, 'A');
  TEST_ASSERT_EQUAL_UINT8('A', write_buffer[0]);
  interface->write(device, 'B');
  TEST_ASSERT_EQUAL_UINT8('B', write_buffer[1]);
}
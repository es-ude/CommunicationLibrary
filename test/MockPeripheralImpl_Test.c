#include "test/Mocks/MockPeripheralImpl.h"
#include "lib/include/PeripheralAPI.h"
#include "unity.h"

#define RAW_MEMORY_SIZE 8


static uint8_t raw_memory[RAW_MEMORY_SIZE];
static MockPeripheralImpl implementation = {.written_bytes = raw_memory};
static Peripheral * const interface = (Peripheral*) &implementation;

void setUp(void) {
  for (int i = 0; i < RAW_MEMORY_SIZE; i++) {
    raw_memory[i] = 0;
  }
  MockPeripheralImpl_init(interface);

}

void test_writeOneByte(void) {

  Peripheral_writeByteNonBlocking(interface, 0xA2);
  TEST_ASSERT_EQUAL_UINT8(0xA2, implementation.written_bytes[0]);
}

void test_writeByteSecond(void) {

  Peripheral_writeByteNonBlocking(interface, 0x6D);
  TEST_ASSERT_EQUAL_UINT8(0x6D, implementation.written_bytes[0]);
}
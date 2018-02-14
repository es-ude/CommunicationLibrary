#include "test/Mocks/MockPeripheralImpl.h"
#include <string.h>
#include "unity.h"
#include "lib/include/SPIMessage.h"

#define RAW_MEMORY_SIZE 64


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

  interface->writeByteNonBlocking(interface, 0xA2);
  TEST_ASSERT_EQUAL_UINT8(0xA2, implementation.written_bytes[0]);
}

void test_writeByteSecond(void) {

  interface->writeByteNonBlocking(interface, 0x6D);
  TEST_ASSERT_EQUAL_UINT8(0x6D, implementation.written_bytes[0]);
}

void test_writeSequence() {
  uint8_t sequence[] = "hello, world!";
  uint16_t length = (uint16_t)strlen((char*)sequence);
  interface->writeBufferBlocking(interface, sequence, length);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(sequence, implementation.written_bytes, length);
}

void test_inspectLastBufferArg() {
  uint16_t length = 8;
  uint8_t content[length];
  interface->writeBufferNonBlocking(interface, content, length);
  TEST_ASSERT_EQUAL_PTR(content, implementation.last_buffer_arg);
}
#include "test/Mocks/MockPeripheralImpl.h"
#include <string.h>
#include "unity.h"

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
  interface->writeSequenceBlocking(interface, sequence, length);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(sequence, implementation.written_bytes, length);
}

void test_readSequence() {
  implementation.written_bytes = (uint8_t*)"this represents some arbitrary string";
  uint16_t length = (uint16_t) strlen((char*)implementation.written_bytes);
  uint8_t read_buffer[length];
  interface->readSequenceBlocking(interface, read_buffer, length);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(implementation.written_bytes, read_buffer, length);
}
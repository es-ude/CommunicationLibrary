#include "test/Mocks/MockPeripheralImpl.h"
#include <memory.h>

static uint8_t readByte(Peripheral *self);
static void writeByte(Peripheral *self, uint8_t byte);

static void writeBuffer(Peripheral *self, const uint8_t *output, uint16_t length);

void MockPeripheralImpl_init(Peripheral *mock) {
  mock->readByteNonBlocking = readByte;
  mock->readByteBlocking = readByte;
  mock->writeByteBlocking = writeByte;
  mock->writeByteNonBlocking = writeByte;
  mock->writeBufferNonBlocking = writeBuffer;
  mock->writeBufferBlocking = writeBuffer;
}

void writeByte(Peripheral *self, uint8_t byte) {
  MockPeripheralImpl *mock = (MockPeripheralImpl*) self;
  mock->written_bytes[0] = byte;
}

uint8_t readByte(Peripheral *self) {
  return ((MockPeripheralImpl*)self)->read_data;
}

void writeBuffer(Peripheral *peripheral, const uint8_t *output, uint16_t length) {
  MockPeripheralImpl* impl = (MockPeripheralImpl*) peripheral;
  impl->last_buffer_arg = output;
  memcpy(impl->written_bytes, output, length);
}


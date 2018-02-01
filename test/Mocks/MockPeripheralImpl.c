#include "test/Mocks/MockPeripheralImpl.h"
#include <memory.h>

static uint8_t readByte(Peripheral *);
static void writeByte(Peripheral *, uint8_t);
static void setInterruptHandler(Peripheral *, InterruptHandler);
static void handleInterrupt(Peripheral *);
static void writeSequence(Peripheral* self, const uint8_t *sequence, uint16_t length);
static void readSequence(Peripheral *self, uint8_t *sequence, uint16_t length);

void writeByte(Peripheral *self, uint8_t byte) {
  MockPeripheralImpl *mock = (MockPeripheralImpl*) self;
  mock->written_bytes[0] = byte;
}

void setInterruptHandler(Peripheral *self, InterruptHandler handler) {}

void handleInterrupt(Peripheral *self) {}

uint8_t readByte(Peripheral *self) { return 0; }

void MockPeripheralImpl_init(Peripheral *mock) {
  mock->readByteNonBlocking = readByte;
  mock->writeByteNonBlocking = writeByte;
  mock->setInterruptHandler = setInterruptHandler;
  mock->handleInterrupt = handleInterrupt;
  mock->writeSequenceBlocking = writeSequence;
  mock->readSequenceBlocking = readSequence;
}

void writeSequence(Peripheral *peripheral, const uint8_t *sequence, uint16_t length) {
  const MockPeripheralImpl* impl = (MockPeripheralImpl*) peripheral;
  memcpy(impl->written_bytes, sequence, length);
}

void readSequence(Peripheral *self, uint8_t *sequence, uint16_t length) {
  const MockPeripheralImpl *impl = (MockPeripheralImpl*) self;
  memcpy(sequence, impl->written_bytes, length);
}
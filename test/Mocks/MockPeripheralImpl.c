#include "test/Mocks/MockPeripheralImpl.h"

static uint8_t readByte(Peripheral *);
static void writeByte(Peripheral *, uint8_t);
static void setInterruptHandler(Peripheral *, InterruptHandler);
static void handleInterrupt(Peripheral *);

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
}
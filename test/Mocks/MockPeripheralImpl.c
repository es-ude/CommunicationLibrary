#include "test/Mocks/MockPeripheralImpl.h"

static uint8_t readByte(Peripheral *);
static void writeByte(Peripheral *, uint8_t);
static void setInterruptHandler(Peripheral *, InterruptHandler);
static void handleInterrupt(Peripheral *);

void initPeripheralInterfaceMock(Peripheral *interface) {
  interface->readByte = readByte;
  interface->writeByte = writeByte;
  interface->setInterruptHandler = setInterruptHandler;
  interface->handleInterrupt = handleInterrupt;
}

void writeByte(Peripheral *self, uint8_t byte) {}

void setInterruptHandler(Peripheral *self, InterruptHandler handler) {}

void handleInterrupt(Peripheral *self) {}

uint8_t readByte(Peripheral *self) { return 0; }

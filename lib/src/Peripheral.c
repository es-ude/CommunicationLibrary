#include "lib/include/Peripheral.h"


void Peripheral_writeByte(Peripheral *self, uint8_t byte) {
    self->writeByte(self, byte);
}

uint8_t Peripheral_readByte(Peripheral *self) {
    return self->readByte(self);
}

void Peripheral_setInterruptHandler(Peripheral *self, InterruptHandler handler) {
    self->setInterruptHandler(self, handler);
}

void Peripheral_handleInterrupt(Peripheral *self) {
    self->handleInterrupt(self);
}

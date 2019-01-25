#include "src/Peripheral/PeripheralIntern.h"

void
PeripheralInterface_writeBlocking(PeripheralInterface *self, const uint8_t *buffer, size_t size)
{
  while(size > 0)
  {
    self->writeByteBlocking(self, *buffer);
    buffer++;
    size--;
  }
}

void
PeripheralInterface_writeNonBlocking(PeripheralInterface *self,
                                      PeripheralInterface_NonBlockingWriteContext context)
{
  self->writeNonBlocking(self, context);
}

void PeripheralInterface_selectPeripheral(PeripheralInterface *self, Peripheral *device)
{
  self->selectPeripheral(self, device);
}

void PeripheralInterface_deselectPeripheral(PeripheralInterface *self, Peripheral *device)
{
  self->deselectPeripheral(self, device);
}

void PeripheralInterface_readBlocking(PeripheralInterface *self, uint8_t *destination_buffer, size_t size)
{
  while(size > 0)
  {
    *destination_buffer = self->readByteBlocking(self);
    destination_buffer++;
    size--;
  }
}

void
PeripheralInterface_readNonBlocking(PeripheralInterface *self, uint8_t *destination_buffer, uint16_t length)
{
  self->readNonBlocking(self, destination_buffer, length);
}

void
PeripheralInterface_handleWriteInterrupt(PeripheralInterface *self)
{
  self->handleWriteInterrupt(self);
}

void
PeripheralInterface_handleReadInterrupt(PeripheralInterface *self)
{
  self->handleReadInterrupt(self);
}

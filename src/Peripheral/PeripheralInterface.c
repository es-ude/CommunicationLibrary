#include "src/Peripheral/PeripheralIntern.h"
#include "CException.h"

static void
deSelectAfterLocking(PeripheralInterface *self,
                    Peripheral *device,
                    void (*function)(PeripheralInterface *self, Peripheral *device));

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
  deSelectAfterLocking(self, device, self->selectPeripheral);
}

void PeripheralInterface_deselectPeripheral(PeripheralInterface *self, Peripheral *device)
{
  deSelectAfterLocking(self, device, self->deselectPeripheral);
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

static void
deSelectAfterLocking(PeripheralInterface *self,
                     Peripheral *device,
                     void (*function)(PeripheralInterface *self, Peripheral *device))
{
  CEXCEPTION_T e;
  Try
  {
    lockMutex(&self->mutex, device);
  }
  Catch (e)
  {
    Throw(PERIPHERALINTERFACE_BUSY_EXCEPTION);
  }
  function(self, device);
}
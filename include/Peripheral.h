#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void Peripheral;
typedef struct PeripheralInterface* PeripheralInterface;
typedef struct InterruptData InterruptData;

typedef struct PeripheralInterface_Callback {
  void (*function) (void *);
  void *argument;
} PeripheralInterface_Callback;

typedef struct PeripheralInterface_NonBlockingWriteContext
{
  PeripheralInterface_Callback callback;
  const uint8_t *output_buffer;
  size_t length;
} PeripheralInterface_NonBlockingWriteContext;

typedef struct PeripheralInterface_NonBlockingReadContext
{
  PeripheralInterface_Callback callback;
  uint8_t *input_buffer;
  size_t length;
} PeripheralInterface_NonBlockingReadContext;

void
PeripheralInterface_init(PeripheralInterface self);

void
PeripheralInterface_configurePeripheral(PeripheralInterface self,
                                        Peripheral *device);
void
PeripheralInterface_writeBlocking(PeripheralInterface self,
                                  const uint8_t *buffer,
                                  uint16_t size);

void
PeripheralInterface_writeNonBlocking(PeripheralInterface self,
                                     PeripheralInterface_NonBlockingWriteContext context);

void
PeripheralInterface_readBlocking(PeripheralInterface self,
                                 uint8_t *buffer,
                                 uint16_t length);

void
PeripheralInterface_readNonBlocking(PeripheralInterface self,
                                    uint8_t *buffer,
                                    uint16_t size);

void
PeripheralInterface_setReadCallback(PeripheralInterface self,
                                    PeripheralInterface_Callback callback);

void
PeripheralInterface_resetReadCallback(PeripheralInterface self);

void
PeripheralInterface_selectPeripheral(PeripheralInterface self,
                                     Peripheral *device);

void
PeripheralInterface_deselectPeripheral(PeripheralInterface self,
                                       Peripheral *device);

void
PeripheralInterface_handleWriteInterrupt(PeripheralInterface self);

void
PeripheralInterface_handleReadInterrupt(PeripheralInterface self);


#endif /* PERIPHERALINTERFACE_H */

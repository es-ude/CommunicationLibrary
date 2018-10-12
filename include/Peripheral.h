#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>
#include <stdbool.h>

typedef void Peripheral;
typedef struct PeripheralInterface* PeripheralInterface;
typedef struct InterruptData InterruptData;

typedef struct PeripheralCallback {
  void (*function) (void *);
  void *argument;
} PeripheralCallback;

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
                                     const uint8_t *buffer,
                                     uint16_t size);

void
PeripheralInterface_setWriteCallback(PeripheralInterface self,
                                     PeripheralCallback write_callback);

void
PeripheralInterface_resetWriteCallback(PeripheralInterface self);

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
                                    PeripheralCallback callback);

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

#ifndef COMMUNICATIONMODULE_PERIPHERALINTERN_H
#define COMMUNICATIONMODULE_PERIPHERALINTERN_H

#include "include/Peripheral.h"

struct PeripheralInterface {
  void (*init)(PeripheralInterface self);

  void (*writeBlocking)(PeripheralInterface self, const uint8_t *buffer, uint16_t length);
  void (*writeNonBlocking)(PeripheralInterface self, PeripheralInterface_NonBlockingWriteContext context);

  void (*readBlocking)(PeripheralInterface self, uint8_t *buffer, uint16_t length);
  void (*readNonBlocking) (PeripheralInterface self, uint8_t *buffer, uint16_t length);
  void (*setReadCallback) (PeripheralInterface self, PeripheralInterface_Callback callback);
  void (*resetReadCallback) (PeripheralInterface self);

  void (*selectPeripheral)(PeripheralInterface self, Peripheral *device);
  void (*deselectPeripheral)(PeripheralInterface self, Peripheral *device);

  void (*handleWriteInterrupt)(PeripheralInterface self);
  void (*handleReadInterrupt) (PeripheralInterface self);

};

#endif //COMMUNICATIONMODULE_PERIPHERALINTERN_H

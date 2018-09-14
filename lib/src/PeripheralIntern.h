#ifndef COMMUNICATIONMODULE_PERIPHERALINTERN_H
#define COMMUNICATIONMODULE_PERIPHERALINTERN_H

#include "lib/include/Peripheral.h"

struct PeripheralInterface {
  void (*init)(PeripheralInterface self);
  void (*writeNonBlocking)(PeripheralInterface self, uint8_t *buffer, uint16_t length);
  void (*readNonBlocking)(PeripheralInterface self, uint8_t *buffer, uint16_t length);
  void (*writeBlocking)(PeripheralInterface self, const uint8_t *buffer, uint16_t length);
  void (*readBlocking)(PeripheralInterface self, uint8_t *buffer, uint16_t length);
  void (*setReadCallback)(PeripheralInterface self, PeripheralCallback callback);
  void (*setWriteCallback)(PeripheralInterface self, PeripheralCallback callback);
  void (*setCallbackClearFlags)(PeripheralInterface self, bool clearReadCallbackOnCall, bool clearWriteCallbackOnCall);
  void (*configurePeripheral)(Peripheral *device);
  void (*selectPeripheral)(PeripheralInterface self, Peripheral *device);
  void (*deselectPeripheral)(PeripheralInterface self, Peripheral *device);
  bool (*isBusy)(PeripheralInterface self);
  void (*destroy)(PeripheralInterface self);
  void (*handleInterrupt)(void);
};

#endif //COMMUNICATIONMODULE_PERIPHERALINTERN_H

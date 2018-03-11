#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;
typedef struct PeripheralInterface PeripheralInterface;
typedef struct InterruptData InterruptData;

struct PeripheralInterface {
  void (*init) (PeripheralInterface *self);
  void (*write) (Peripheral *device, uint8_t byte);
  uint8_t (*read) (Peripheral *device);
  void (*selectPeripheral) (PeripheralInterface *self, Peripheral *device);
  void (*deselectPeripheral) (Peripheral *device);
  void (*destroy) (PeripheralInterface *self);
  void (*handleInterrupt) (InterruptData *data);
};






#endif /* PERIPHERALINTERFACE_H */

#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef void Peripheral;
typedef struct PeripheralInterface PeripheralInterface;
typedef struct InterruptData InterruptData;

/**
 * Datei mit ISR sollte dann wie folgt aussehen:
 *
 * PeripheralInterface *interface; // das interface zu dem der ISR gehoert, muss vom User gesetzt werden
 *
 * ISR(vect) {
 *   interface->handleInterrupt(interface);
 * }
 */
struct PeripheralInterface {
  void (*init) (PeripheralInterface *self);
  void (*write)(PeripheralInterface *self, uint8_t byte);
  uint8_t (*read)(PeripheralInterface *self);
  void (*selectPeripheral) (PeripheralInterface *self, Peripheral *device);
  void (*deselectPeripheral)(PeripheralInterface *interface, Peripheral *device);
  void (*destroy) (PeripheralInterface *self);
  void (*enableInterrupt) (PeripheralInterface *self);
  void (*disableInterrupt) (PeripheralInterface *self);
  void (*handleInterrupt) (PeripheralInterface *self);
  InterruptData *interrupt_data;
};


void PeripheralInterface_init(PeripheralInterface *self);

uint8_t PeripheralInterface_read(PeripheralInterface *self);

void PeripheralInterface_writeBlocking(PeripheralInterface *self, const uint8_t *buffer, uint16_t size);

void PeripheralInterface_selectPeripheral(PeripheralInterface *self, Peripheral *device);

void PeripheralInterface_deselectPeripheral(PeripheralInterface *self, Peripheral *device);


void PeripheralInterface_destroy(PeripheralInterface *self);


#endif /* PERIPHERALINTERFACE_H */

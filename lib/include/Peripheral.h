#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <stdint.h>

typedef struct Peripheral Peripheral;
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


static inline void PeripheralInterface_init(PeripheralInterface *self) {
  self->init(self);
}

static inline void PeripheralInterface_write(PeripheralInterface *self, uint8_t byte) {
  self->write(self, byte);
}

static inline uint8_t PeripheralInterface_read(PeripheralInterface *self) {
  return self->read(self);
}

static inline void PeripheralInterface_selectPeripheral(PeripheralInterface *self, Peripheral *device) {
  self->selectPeripheral(self, device);
}

static inline void PeripheralInterface_deselectPeripheral(PeripheralInterface *self, Peripheral *device) {
  self->deselectPeripheral(self, device);
}

static inline void PeripheralInterface_destroy(PeripheralInterface *self) {
  self->destroy(self);
}

static inline void PeripheralInterface_handleInterrupt(PeripheralInterface *self) {
  self->handleInterrupt(self);
}

static inline void PeripheralInterface_enableInterrupt(PeripheralInterface *self) {
  self->enableInterrupt(self);
}

static inline void PeripheralInterface_disableInterrupt(PeripheralInterface *self) {
  self->disableInterrupt(self);
}
#endif /* PERIPHERALINTERFACE_H */

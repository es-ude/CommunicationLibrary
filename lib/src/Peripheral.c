#include "lib/include/Peripheral.h"


void PeripheralInterface_writeBlocking(PeripheralInterface *self, const uint8_t *buffer, uint16_t size) {
}

void PeripheralInterface_writeNonBlocking(PeripheralInterface *self, const uint8_t *buffer, uint16_t size) {

}

void
PeripheralInterface_setWriteCallback(PeripheralInterface *self, PeripheralCallback write_callback) {}

void PeripheralInterface_selectPeripheral(PeripheralInterface *self, Peripheral *device){}

void PeripheralInterface_deselectPeripheral(PeripheralInterface *self, Peripheral *device){}
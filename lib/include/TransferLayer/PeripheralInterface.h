//
// Created by Peter Zdankin on 01.04.18.
//

#ifndef COMMUNICATIONMODULE_PERIPHERALINTERFACE_H
#define COMMUNICATIONMODULE_PERIPHERALINTERFACE_H
#include <stdint.h>
#include <stdbool.h>

typedef struct PeripheralInterface PeripheralInterface;
typedef struct Peripheral Peripheral;
typedef void (*PeripheralCallback) (void *);

struct PeripheralInterface {
    void (*init)(PeripheralInterface *self);
    void (*writeNonBlocking)(PeripheralInterface *self, uint8_t *buffer, uint16_t length);
    void (*readNonBlocking)(PeripheralInterface *self, uint8_t *buffer, uint16_t length);
    void (*writeBlocking)(PeripheralInterface *self, uint8_t *buffer, uint16_t length);
    void (*readBlocking)(PeripheralInterface *self, uint8_t *buffer, uint16_t length);
    void (*setReadCallback)(PeripheralInterface *self, PeripheralCallback callback, void *callback_parameter);
    void (*setWriteCallback)(PeripheralInterface *self, PeripheralCallback callback, void *callback_parameter);
    void (*setCallbackClearFlags)(PeripheralInterface *self, bool clearReadCallbackOnCall, bool clearWriteCallbackOnCall);
    void (*configurePeripheral)(PeripheralInterface *self, Peripheral *device);
    void (*selectPeripheral)(PeripheralInterface *self, Peripheral *device);
    void (*deselectPeripheral)(PeripheralInterface *self, Peripheral *device);
    void (*destroy)(PeripheralInterface *self);
};

void PeripheralInterface_writeNonBlocking(PeripheralInterface *self, const uint8_t *buffer, uint16_t length);
void PeripheralInterface_readNonBlocking(PeripheralInterface *self, uint8_t *buffer, uint16_t length);


#endif //COMMUNICATIONMODULE_PERIPHERALINTERFACE_H

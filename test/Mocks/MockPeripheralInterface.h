#ifndef COMMUNICATIONMODULE_MOCKPERIPHERALINTERFACE_H
#define COMMUNICATIONMODULE_MOCKPERIPHERALINTERFACE_H

#include "lib/include/Peripheral.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct MockPeripheralTransfer MockPeripheralTransfer;

typedef struct MockPeripheral {
  uint8_t *read_buffer;
  uint8_t *write_buffer;
  uint8_t read_buffer_size;
  uint8_t write_buffer_size;
  MockPeripheralTransfer *first;
} MockPeripheral;

struct MockPeripheralTransfer {
  uint8_t number_of_reads;
  uint8_t number_of_writes;
  MockPeripheralTransfer *next;
};

// destroys the linked transfer list
void MockPeripheral_clean(MockPeripheral *peripheral);
MockPeripheralTransfer *MockPeripheral_getTransfer(MockPeripheral *device, uint8_t number);
PeripheralInterface *PeripheralInterface_createMockImpl(const MemoryManagement *memory_managers);
#endif //COMMUNICATIONMODULE_MOCKPERIPHERALINTERFACE_H

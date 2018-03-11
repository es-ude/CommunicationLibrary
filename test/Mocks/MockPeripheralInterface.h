#ifndef COMMUNICATIONMODULE_MOCKPERIPHERALINTERFACE_H
#define COMMUNICATIONMODULE_MOCKPERIPHERALINTERFACE_H

#include "lib/include/Peripheral.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct MockPeripheral {
  PeripheralInterface *interface;
  uint8_t *read_buffer;
  uint8_t *write_buffer;
  uint8_t read_buffer_size;
  uint8_t write_buffer_size;
} MockPeripheral;

PeripheralInterface *PeripheralInterface_createMockImpl(const MemoryManagement *memory_managers);

#endif //COMMUNICATIONMODULE_MOCKPERIPHERALINTERFACE_H

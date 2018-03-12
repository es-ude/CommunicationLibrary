#include <stdbool.h>
#include "MockPeripheralInterface.h"
#include "lib/include/Exception.h"

static void selectPeripheral(PeripheralInterface *interface, Peripheral *device);
static void write(PeripheralInterface *self, uint8_t byte);
static uint8_t read(PeripheralInterface *self);
static void deselectPeripheral(PeripheralInterface *interface, Peripheral *device);
static void destroy(PeripheralInterface *interface);
static void enableInterrupt(PeripheralInterface *self);
static void disableInterrupt(PeripheralInterface *self);

typedef struct MockPeripheralInterface {
  PeripheralInterface interface;
  uint8_t current_write_buffer_position;
  uint8_t current_read_buffer_position;
  Deallocator deallocate;
  MockPeripheral *device;
} MockPeripheralInterface;

static bool isBusy(const MockPeripheralInterface *impl,
                   const MockPeripheral *device);
static void updateBufferPosition(uint8_t *buffer_position, uint8_t buffer_size);
static void initInterface(PeripheralInterface *interface);
static void initPrivateVariables(MockPeripheralInterface *impl, const MemoryManagement *dynamic_memory);

PeripheralInterface *PeripheralInterface_createMockImpl(const MemoryManagement *memory_managers) {
  PeripheralInterface *interface = memory_managers->allocate(sizeof(MockPeripheralInterface));
  initInterface(interface);
  MockPeripheralInterface *impl = (MockPeripheralInterface *) interface;
  initPrivateVariables(impl, memory_managers);
  return interface;
}

void initInterface(PeripheralInterface *interface) {
  interface->selectPeripheral = selectPeripheral;
  interface->write = write;
  interface->read = read;
  interface->deselectPeripheral = deselectPeripheral;
  interface->destroy = destroy;
  interface->enableInterrupt = enableInterrupt;
  interface->disableInterrupt = disableInterrupt;
}

void initPrivateVariables(MockPeripheralInterface *impl, const MemoryManagement *dynamic_memory) {
  impl->deallocate = dynamic_memory->deallocate;
  impl->current_write_buffer_position = 0;
  impl->current_read_buffer_position = 0;
  impl->device = NULL;
}

void selectPeripheral(PeripheralInterface *interface, Peripheral *device) {
  MockPeripheral *mock_device = (MockPeripheral *) device;
  MockPeripheralInterface *impl = (MockPeripheralInterface *) interface;
  if (isBusy(impl, mock_device)) {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
  impl->device = mock_device;
  impl->current_read_buffer_position = 0;
  impl->current_write_buffer_position = 0;
}

void write(PeripheralInterface *self, uint8_t byte) {
  MockPeripheralInterface *impl = (MockPeripheralInterface *) self;
  MockPeripheral *mock = impl->device;
  if ( impl->device == NULL ) {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
  mock->write_buffer[impl->current_write_buffer_position] = byte;
  updateBufferPosition(&impl->current_write_buffer_position,
                       mock->write_buffer_size);
}

uint8_t read(PeripheralInterface *self) {
  MockPeripheralInterface *impl = (MockPeripheralInterface *) self;
  MockPeripheral *mock = impl->device;
  if ( impl->device == NULL ) {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
  uint8_t byte = mock->read_buffer[impl->current_read_buffer_position];
  updateBufferPosition(&impl->current_read_buffer_position,
                       mock->read_buffer_size);
  return byte;
}

void deselectPeripheral(PeripheralInterface *interface, Peripheral *device) {
  MockPeripheralInterface *impl = (MockPeripheralInterface *) interface;
  impl->device = NULL;
}

void enableInterrupt(PeripheralInterface *interface) {

}

void disableInterrupt(PeripheralInterface *interface) {

}

void updateBufferPosition(uint8_t *buffer_position,
                          uint8_t buffer_size) {
  *buffer_position = (*buffer_position + 1) % buffer_size;
}

bool isBusy(const MockPeripheralInterface *impl,
                   const MockPeripheral *device) {
  return (impl->device != NULL && impl->device != device);
}

void destroy(PeripheralInterface *self) {
  MockPeripheralInterface *impl = (MockPeripheralInterface *) self;
  impl->deallocate(self);
}

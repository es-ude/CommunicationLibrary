#include <stdbool.h>
#include <stdlib.h>
#include "PeripheralInterfaceMock.h"
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
  Allocator allocate;
  MockPeripheral *device;
} MockPeripheralInterface;

static bool isBusy(const MockPeripheralInterface *impl,
                   const MockPeripheral *device);
static void updateBufferPosition(uint8_t *buffer_position, uint8_t buffer_size);
static void initInterface(PeripheralInterface *interface);
static void initPrivateVariables(MockPeripheralInterface *impl, const MemoryManagement *dynamic_memory);
static void addNewTransfer(MockPeripheral *device);
static MockPeripheralTransfer* getCurrentTransfer(MockPeripheral *device);

PeripheralInterface *PeripheralInterface_createMockImpl(const MemoryManagement *memory_managers) {
  PeripheralInterface *interface = memory_managers->allocate(sizeof(*interface));
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
}

void initPrivateVariables(MockPeripheralInterface *impl, const MemoryManagement *dynamic_memory) {
  impl->deallocate = dynamic_memory->deallocate;
  impl->allocate = dynamic_memory->allocate;
  impl->device = NULL;
}

void selectPeripheral(PeripheralInterface *interface, Peripheral *device) {
  MockPeripheral *mock_device = (MockPeripheral *) device;
  MockPeripheralInterface *impl = (MockPeripheralInterface *) interface;
  if (isBusy(impl, mock_device)) {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
  impl->device = mock_device;
  addNewTransfer(mock_device);
}

void addNewTransfer(MockPeripheral *device){
  if (device->first == NULL) {
    device->first = malloc(sizeof(*device->first));
    device->first->number_of_writes = 0;
    device->first->number_of_reads = 0;
    device->first->next = NULL;
  }
  else {
    MockPeripheralTransfer *current = getCurrentTransfer(device);
    current->next = malloc(sizeof(*device->first));
    current->next->number_of_reads = 0;
    current->next->number_of_writes = 0;
    current->next->next = NULL;
  }
}

void write(PeripheralInterface *self, uint8_t byte) {
  MockPeripheralInterface *impl = (MockPeripheralInterface *) self;
  MockPeripheral *mock = impl->device;
  if ( impl->device == NULL ) {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
  mock->write_buffer[mock->current_write_buffer_position] = byte;
  updateBufferPosition(&mock->current_write_buffer_position,
                       mock->write_buffer_size);
  getCurrentTransfer(mock)->number_of_writes++;
}

uint8_t read(PeripheralInterface *self) {
  MockPeripheralInterface *impl = (MockPeripheralInterface *) self;
  MockPeripheral *mock = impl->device;
  if ( impl->device == NULL ) {
    Throw(PERIPHERAL_INTERFACE_BUSY_EXCEPTION);
  }
  uint8_t byte = mock->read_buffer[mock->current_read_buffer_position];
  updateBufferPosition(&mock->current_read_buffer_position,
                       mock->read_buffer_size);
  getCurrentTransfer(mock)->number_of_reads++;
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

void MockPeripheral_clean(MockPeripheral *peripheral) {
  while(peripheral->first != NULL) {
    MockPeripheralTransfer *tmp = peripheral->first->next;
    free(peripheral->first);
    peripheral->first = tmp;
  }
}

MockPeripheralTransfer *getCurrentTransfer(MockPeripheral *device) {
  MockPeripheralTransfer *current = device->first;
  while (current->next != NULL) {
    current = current->next;
  }
  return current;
}

MockPeripheralTransfer *MockPeripheral_getTransfer(MockPeripheral *device, uint8_t number) {
  MockPeripheralTransfer *current = device->first;
  uint8_t counter = 0;
  while (current->next != NULL && counter < number) {
    current = current->next;
    counter++;
  }
  if (counter < number) {
    return NULL;
  }
  else {
    return current;
  }
}
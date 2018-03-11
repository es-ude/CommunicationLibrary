#include "MockPeripheralInterface.h"

static void selectPeripheral(PeripheralInterface *interface, Peripheral *device);
static void write(Peripheral *device, uint8_t byte);
static void destroy(PeripheralInterface *interface);

typedef struct MockPeripheralInterface {
  PeripheralInterface interface;
  uint8_t current_write_buffer_position;
  Deallocator deallocate;
  MockPeripheral *device;
} MockPeripheralInterface;

PeripheralInterface *PeripheralInterface_createMockImpl(const MemoryManagement *memory_managers){
  PeripheralInterface *interface = memory_managers->allocate(sizeof(MockPeripheralInterface));
  interface->selectPeripheral = selectPeripheral;
  interface->write = write;
  interface->destroy = destroy;
  MockPeripheralInterface *impl = (MockPeripheralInterface *)interface;
  impl->current_write_buffer_position = 0;
  return interface;
}

void selectPeripheral(PeripheralInterface *interface, Peripheral *device) {
  MockPeripheral *mock_device = (MockPeripheral *) device;
  mock_device->interface = interface;
}

void write(Peripheral *peripheral, uint8_t byte) {
  MockPeripheral *mock = (MockPeripheral*)peripheral;
  MockPeripheralInterface *impl = (MockPeripheralInterface *)mock->interface;
  mock->write_buffer[impl->current_write_buffer_position++] = byte;
}

void destroy(PeripheralInterface *self) {

}

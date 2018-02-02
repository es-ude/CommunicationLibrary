#include "test/Mocks/MockPeripheralImpl.h"
#include "lib/include/NetworkHardware.h"
#include "lib/include/NetworkHardwareMRFImpl.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"



#define MEMORY_CAPACITY 64
static uint8_t raw_memory[MEMORY_CAPACITY];
static MockAllocateConfig allocate_config = {.returned_address = raw_memory};

void test_initWrites16BitBroadcastAddress(void) {
  MockAllocate_configure(&allocate_config);
  MockPeripheralImpl mockPeripheral;
  Peripheral *output = (Peripheral*) &mockPeripheral;
  NetworkHardware *hardware = NetworkHardware_createMRF(output, MockAllocate_allocate);
  hardware->init(hardware);
}

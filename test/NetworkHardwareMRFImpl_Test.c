#include "lib/include/NetworkHardware.h"
#include "test/Mocks/MockSPIImpl.h"
#include "lib/include/NetworkHardwareMRFImpl.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "src/unity.h"


#define MEMORY_CAPACITY 64
static uint8_t raw_memory[MEMORY_CAPACITY];
static MockAllocateConfig allocate_config = {.returned_address = raw_memory};

void test_initWrites16BitBroadcastAddress(void) {
  MockAllocate_configure(&allocate_config);
  SPIDeviceMockImpl spi_mock_device;
  SPIDevice *output_device = (SPIDevice *) &spi_mock_device;
  NetworkHardware *hardware = NetworkHardware_createMRF(output_device, MockAllocate_allocate);
  hardware->init(hardware);
}


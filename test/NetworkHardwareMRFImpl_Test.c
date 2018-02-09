#include <string.h>
#include "lib/include/NetworkHardware.h"
#include "test/Mocks/MockSPIImpl.h"
#include "lib/include/NetworkHardwareMRFImpl.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "src/unity.h"
#include "lib/include/NetworkHardwareMRFInternalConstants.h"


#define MEMORY_CAPACITY 64
static uint8_t raw_memory[MEMORY_CAPACITY];

static MockAllocateConfig allocate_config = {.returned_address = raw_memory};

void test_initPerformsSoftwareReset(void) {
  MockAllocate_configure(&allocate_config);
  SPIDeviceMockImpl spi_mock_device;
  SPIDevice *output_device = (SPIDevice *) &spi_mock_device;
  SPIDeviceMockImpl_init(&spi_mock_device);
  uint8_t buffer[128];
  spi_mock_device.input_buffer = buffer;
  spi_mock_device.output_buffer = buffer;
  memset(buffer, 0, 128);
  NetworkHardware *hardware = NetworkHardware_createMRF(output_device, MockAllocate_allocate);
  hardware->init(hardware);
  uint8_t expected_buffer[3] = {mrf_register_software_reset >> 8 & 0xFF, mrf_register_software_reset & 0xFF, 0x07};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_buffer, buffer, 3);
}


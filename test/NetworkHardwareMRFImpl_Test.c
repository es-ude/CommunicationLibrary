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
static SPIDeviceMockImpl spi_mock_device;
static SPIDevice *output_device = (SPIDevice *) &spi_mock_device;
static NetworkHardware *mrf;

void setUp(void) {
  MockAllocate_configure(&allocate_config);
  SPIDeviceMockImpl_init(&spi_mock_device);
  spi_mock_device.input_buffer = NULL;
  spi_mock_device.output_buffer = NULL;
  mrf = NetworkHardware_createMRF(output_device, MockAllocate_allocate);
}

void test_initPerformsSoftwareReset(void) {
  uint8_t buffer[128];
  memset(buffer, 0, 128);
  spi_mock_device.output_buffer = buffer;
  uint8_t expected_buffer[3] = {
          mrf_register_software_reset >> 3,
          mrf_register_software_reset << 5,
          0x07};
  expected_buffer[0] |= 1 << 7;
  expected_buffer[1] |= 1 << 4;
  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buffer, buffer, 3);
}


void test_initMakesNoAsynchronousCallsToSPI(void) {
  uint8_t buffer[128];
  memset(buffer, 0, 128);
  spi_mock_device.output_buffer = buffer;

  NetworkHardware_init(mrf);
  TEST_ASSERT_EQUAL_UINT8(0, spi_mock_device.number_of_async_transfer_calls);
}


#include "unity.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"

static uint8_t registers[5];
static SPIConfigNew config;

void setUp(void) {
  config.spi_control_register = registers;
  config.spi_data_register = registers+1;
  config.spi_slave_select_input_data_direction_register = registers+2;
  config.spi_status_register = registers+3;
  config.spi_slave_select_input_data_register = registers+4;

}

void test_setupSPIPeripheral(void) {
  uint8_t data_direction_register;
  uint8_t data_register;
  uint8_t pin_number;
  SPIPeripheralNew peripheral = {
          .data_direction_register = &data_direction_register,
          .data_register = &data_register,
          .pin_number = pin_number,
  };
}

void test_getModulesSize(void) {
  size_t size = PeripheralInterfaceSPI_requiredSize();
  TEST_ASSERT_GREATER_THAN(0, size);
}

void test_init(void) {
  uint8_t memory[PeripheralInterfaceSPI_requiredSize()];
  PeripheralInterface interface = PeripheralInterfaceSPI_createNew(memory, &config);
  PeripheralInterface_init(interface);
}
#include "unity.h"
#include "lib/include/Peripheral/PeripheralSPIImpl.h"
#include <memory.h>
#include "lib/src/Peripheral/SpiPinNumbers.h"
#include "lib/include/Exception.h"

#define NUMBER_OF_MASTER_REGISTERS 5
#define NUMBER_OF_SLAVE_REGISTERS 2
static uint8_t registers_master[NUMBER_OF_MASTER_REGISTERS];
static uint8_t registers_slave[NUMBER_OF_SLAVE_REGISTERS];
static SPIConfig config;
static PeripheralSPI peripheral = {
        .select_chip_pin_number = 1,
        .data_register = registers_slave,
        .data_direction_register = registers_slave+1,
        .spi_mode = SPI_MODE_0,
        .idle_signal = SPI_IDLE_SIGNAL_HIGH,
        .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_64,
        .data_order = SPI_DATA_ORDER_LSB_FIRST,
};

void setUp(void) {
  uint8_t *registers_local_copy = registers_master;
  config.control_register = registers_local_copy++;
  config.data_register = registers_local_copy++;
  config.status_register = registers_local_copy++;
  config.io_lines_data_direction_register = registers_local_copy++;
  config.io_lines_data_register = registers_local_copy;
  config.clock_pin = 0;
  config.miso_pin = 1;
  config.mosi_pin = 2;
  memset(registers_master, 0, NUMBER_OF_MASTER_REGISTERS);
}

/**
 * TestCheckList of minimum functionality:
 * - configures the clock divider correctly
 * - configures the io line correctly
 * - configures io pin for peripheral
 * - writes correct pin for peripheral select
 * - writes correct pin for peripheral deselect
 *
 */

void test_selectSetsSPIEnableBit(void) {
  uint8_t memory[PeripheralInterfaceSPI_requiredSize()];
  PeripheralInterface interface = PeripheralInterfaceSPI_createNew(memory, &config);
  CEXCEPTION_T e;
  Try {
        PeripheralInterface_selectPeripheral(interface, &peripheral);
      } Catch(e) {
    TEST_FAIL();
  }

  TEST_ASSERT_BIT_HIGH(spi_enable_bit, *config.control_register);
}

void test_deselectSetsControlRegisterToDefault(void) {
  uint8_t memory[PeripheralInterfaceSPI_requiredSize()];
  PeripheralInterface interface = PeripheralInterfaceSPI_createNew(memory, &config);
  CEXCEPTION_T e;
  Try {
        PeripheralInterface_selectPeripheral(interface, &peripheral);
        PeripheralInterface_deselectPeripheral(interface, &peripheral);
      } Catch (e) {
    TEST_FAIL();
  }

  TEST_ASSERT_BITS_LOW(0xFF, *config.control_register);
}

void test_failedSetupEndsWithDefaultControlRegisterValues(void) {
  uint8_t memory[PeripheralInterfaceSPI_requiredSize()];
  PeripheralInterface  interface = PeripheralInterfaceSPI_createNew(memory, &config);
  PeripheralSPI invalid_peripheral = peripheral;
  invalid_peripheral.spi_mode = 30;
  CEXCEPTION_T e;
  Try {
        PeripheralInterface_selectPeripheral(interface, &invalid_peripheral);
      }
      Catch(e) {
    TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_SELECT_EXCEPTION, e);
  }
  TEST_ASSERT_BITS_LOW(0xFF, *config.control_register);
}

void test_selectConfiguresInterfaceAsMaster(void) {
  uint8_t memory[PeripheralInterfaceSPI_requiredSize()];
  PeripheralInterface interface = PeripheralInterfaceSPI_createNew(memory, &config);
  PeripheralInterface_selectPeripheral(interface, &peripheral);

  TEST_ASSERT_BIT_HIGH(master_slave_select_bit, *config.control_register);
}
#include "unity.h"
#include "CommunicationModule/PeripheralSPIImpl.h"
#include <memory.h>
#include <stdlib.h>
#include "src/Peripheral/SpiPinNumbers.h"
#include "CommunicationModule/Exception.h"

#define NUMBER_OF_MASTER_REGISTERS 5
#define NUMBER_OF_SLAVE_REGISTERS 2
static uint8_t registers_master[NUMBER_OF_MASTER_REGISTERS];
static uint8_t registers_slave[NUMBER_OF_SLAVE_REGISTERS];
static SPIConfig config;
static SPISlave peripheral = {
        .slave_select_pin_number = 1,
        .data_register = registers_slave,
        .data_direction_register = registers_slave+1,
        .spi_mode = SPI_MODE_0,
        .idle_signal = SPI_IDLE_SIGNAL_HIGH,
        .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_64,
        .data_order = SPI_DATA_ORDER_LSB_FIRST,
};
static PeripheralInterface interface = NULL;

static bool callback_called = false;

static PeripheralInterface_NonBlockingWriteContext
createNonBlockingWriteContextWithoutCallback(uint8_t *data, size_t length)
{
  PeripheralInterface_NonBlockingWriteContext context = {
      .output_buffer = data,
      .length = length,
      .callback = {
          .function = NULL,
          .argument = NULL,
      },
  };
  return context;
}

static void
stubCallback(void *arg)
{
  if (*(uint8_t *)arg == 13)
  {
    callback_called = true;
  }
}

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
  interface = malloc(PeripheralInterfaceSPI_getADTSize());
  PeripheralInterfaceSPI_createNew((uint8_t *)interface, &config);
  callback_called = false;
}

void
tearDown(void)
{
  free(interface);
}

void test_selectSetsSPIEnableBit(void) {
  CEXCEPTION_T e;
  Try {
        PeripheralInterface_selectPeripheral(interface, &peripheral);
      } Catch(e) {
    TEST_FAIL();
  }

  TEST_ASSERT_BIT_HIGH(spi_enable_bit, *config.control_register);
}

void test_deselectSetsControlRegisterToDefault(void) {
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
  SPISlave invalid_peripheral = peripheral;
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

void
test_selectConfiguresInterfaceAsMaster(void)
{
  PeripheralInterface_selectPeripheral(interface, &peripheral);

  TEST_ASSERT_BIT_HIGH(master_slave_select_bit, *config.control_register);
}

void
test_nonBlockingWriteTransfersFirstByte(void)
{
  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "hello";
  PeripheralInterface_NonBlockingWriteContext context = {
      .length = 5,
      .callback = {
          .argument = NULL,
          .function = NULL,
      },
      .output_buffer = data,
  };
  PeripheralInterface_writeNonBlocking(interface, context);
  TEST_ASSERT_EQUAL_UINT8('h', *config.data_register);
}

void
test_handleWriteInterruptTransmitsSecondByte(void)
{
  TEST_IGNORE();
  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "abcde";
  PeripheralInterface_NonBlockingWriteContext context = {
      .length = 5,
      .output_buffer = data,
      .callback = {
          .argument = NULL,
          .function = NULL,
      },
  };
  PeripheralInterface_writeNonBlocking(interface, context);
  PeripheralInterface_handleWriteInterrupt(interface);

  TEST_ASSERT_EQUAL_UINT8('b', *config.data_register);
}


void
test_handleWriteInterruptTransmitsThirdByte(void)
{
  TEST_IGNORE();

  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "fghij";

  PeripheralInterface_NonBlockingWriteContext context = {
      .length = 5,
      .output_buffer = data,
      .callback = {
          .argument = NULL,
          .function = NULL,
      },
  };

  PeripheralInterface_writeNonBlocking(interface, context);
  PeripheralInterface_handleWriteInterrupt(interface);
  PeripheralInterface_handleWriteInterrupt(interface);

  TEST_ASSERT_EQUAL_STRING_LEN(data+2, config.data_register, 1);
}


void
test_callHandleWriteInterruptTooManyTimes(void)
{
  TEST_IGNORE();

  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "fghij__";

  PeripheralInterface_NonBlockingWriteContext context = {
      .length = 5,
      .output_buffer = data,
      .callback = {
          .argument = NULL,
          .function = NULL,
      },
  };

  PeripheralInterface_writeNonBlocking(interface, context);
  for (uint8_t i = 0; i < 5; i++) {
    PeripheralInterface_handleWriteInterrupt(interface);
  }

  TEST_ASSERT_EQUAL_STRING_LEN(data+4, config.data_register, 1);
}

void
test_callWriteNonBlockingWhileInterfaceIsBusy(void)
{
  TEST_IGNORE();

  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "fghij__";
  PeripheralInterface_NonBlockingWriteContext context = createNonBlockingWriteContextWithoutCallback(data, 5);
  PeripheralInterface_writeNonBlocking(interface, context);

  CEXCEPTION_T e = 0;
  Try
      {
        PeripheralInterface_writeNonBlocking(interface, context);
      }
      Catch(e)
  {
    TEST_ASSERT_EQUAL_UINT8(PERIPHERAL_INTERFACE_BUSY_EXCEPTION, e);
  }
  TEST_ASSERT_NOT_EQUAL(0, e);
}


void
test_callWriteNonBlockingWithCallback(void)
{
  TEST_IGNORE();

  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "fghij__";
  uint8_t arg = 13;
  PeripheralInterface_Callback callback = {
          .function = stubCallback,
          .argument = &arg,
  };
  PeripheralInterface_NonBlockingWriteContext context = {
      .output_buffer = data,
      .length = 1,
      .callback = callback
  };
  PeripheralInterface_writeNonBlocking(interface, context);
  PeripheralInterface_handleWriteInterrupt(interface);
  TEST_ASSERT_TRUE(callback_called);
}

// TODO: adjust test to fit the new readNonBlocking function signature
void
test_readNonBlockingTransmitsFirstByte(void)
{

  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t buffer = 'a';
  uint8_t expected = 'm';
  PeripheralInterface_readNonBlocking(interface, &buffer, 1);
  *config.data_register = 'm';
  PeripheralInterface_handleReadInterrupt(interface);

  TEST_ASSERT_EQUAL_STRING_LEN(&expected, &buffer, 1);
}

void
test_readNonBlockingWithCallback(void)
{
  TEST_IGNORE();
  PeripheralInterface_selectPeripheral(interface, &peripheral);

  *config.data_register = 'l';
  uint8_t buffer = 'a';
  uint8_t arg = 13;
  PeripheralInterface_readNonBlocking(interface, &buffer, 1);
  PeripheralInterface_Callback callback = {
          .function = stubCallback,
          .argument = &arg,
  };
  PeripheralInterface_handleReadInterrupt(interface);
  TEST_ASSERT_TRUE(callback_called);
}

void
test_readNonBlockingTransfersAllBytes(void)
{
  PeripheralInterface_selectPeripheral(interface, &peripheral);

  uint8_t data[] = "hello";
  uint8_t buffer[5];
  PeripheralInterface_readNonBlocking(interface, buffer, 1);
  for (uint8_t i = 0; i < 5; i++)
  {
    *config.data_register = data[i];
    PeripheralInterface_handleReadInterrupt(interface);
  }

  TEST_ASSERT_EQUAL_STRING_LEN(data, buffer, 5);
}

void
test_callBackIsOnlyTriggeredOnReadCompletion(void)
{
  PeripheralInterface_selectPeripheral(interface, &peripheral);
  TEST_IGNORE();

  uint8_t buffer[8];
  uint8_t arg = 13;
  PeripheralInterface_Callback callback = {
          .function = stubCallback,
          .argument = &arg,
  };
  PeripheralInterface_readNonBlocking(interface, buffer, 8);
  for (uint8_t i = 0; i < 7; i++)
  {
    PeripheralInterface_handleReadInterrupt(interface);
    TEST_ASSERT_FALSE(callback_called);
  }
}

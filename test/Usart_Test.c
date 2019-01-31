#include "Peripheral/Usart.h"
#include "src/Peripheral/UsartIntern.h"
#include "unity.h"
#include "CException.h"

/**
 * TODO:
 * - support more peripheral setups
 */

static PeripheralInterfaceUsartImpl impl;
static PeripheralInterface *interface = (PeripheralInterface *)&impl;
static uint8_t usart_data_register    = 0;

/**
 * this register is used to determine end of transmission and
 * the test for blocking write will not terminate if a certain
 * bit is not set. Usually the hardware takes care of that.
 * A more sophisticated approach would be to abstract reading
 * that register by using a function that could then be mocked
 * in the test. But this might induce additional overhead during runtime
 * so i'm not sure it's worth the programming overhead.
 */
static uint8_t usart_control_and_status_register_a = 0xFF;
static uint8_t usart_control_and_status_register_b = 0;
static uint8_t usart_control_and_status_register_c = 0;
static uint8_t baud_rate_register_high             = 0;
static uint8_t baud_rate_register_low              = 0;
static uint32_t cpu_frequency                      = 16000000;
static UsartPeripheral peripheral;
UsartConfig config;

static uint16_t
calculateBaudRateRegisterValue(uint32_t cpu_frequency, uint32_t baud_rate)
{
  return cpu_frequency / 16 / baud_rate - 1;
}

void
setUp(void)
{
  for (uint8_t i = 0; i < sizeof(impl); i++)
    {
      ((uint8_t *)interface)[i] = 0;
    }
  usart_data_register                 = 0;
  usart_control_and_status_register_a = 0xFF;
  usart_control_and_status_register_b = 0;
  usart_control_and_status_register_c = 0;
  baud_rate_register_high             = 0;
  baud_rate_register_low              = 0;

  /*
   * Please note that the local config is removed from stack
   * after setUp return, but the created interface remains
   * operable, because the configuration is copied to the adt.
   */
  UsartConfig local_config = {
    .data_register                 = &usart_data_register,
    .control_and_status_register_a = &usart_control_and_status_register_a,
    .control_and_status_register_b = &usart_control_and_status_register_b,
    .control_and_status_register_c = &usart_control_and_status_register_c,
    .cpu_frequency                 = cpu_frequency,
    .baud_rate_register_high       = &baud_rate_register_high,
    .baud_rate_register_low        = &baud_rate_register_low,
  };
  config     = local_config;
  peripheral = PeripheralInterfaceUsartImpl_getDefaultPeripheral();
  PeripheralInterfaceUsartImpl_createNew(interface, &local_config);
}

/**
 * We cannot test that the whole string is written to the data register
 * here.
 */
void
test_writeBlockingWritesToDataRegister(void)
{
  interface->writeByteBlocking(interface, 'o');
  TEST_ASSERT_EQUAL_UINT8('o', usart_data_register);
}

void
test_readBlockingReadsFromDataRegister(void)
{
  for (uint8_t data = 0; data < 255; data++)
    {
      usart_data_register = data;
      TEST_ASSERT_EQUAL_UINT8(usart_data_register,
                              interface->readByteBlocking(interface));
    }
}

void
test_selectUsartPeripheralSetsBaudRateHighForNormalMode(void)
{
  baud_rate_register_high = 0xFF;
  interface->selectPeripheral(interface, &peripheral);
  uint16_t baud_rate_register_value =
    calculateBaudRateRegisterValue(cpu_frequency, 9600);
  TEST_ASSERT_EQUAL_UINT16(baud_rate_register_value >> 8,
                           baud_rate_register_high);
}

void
test_selectUsartPeriphealSetsBaudRateLowForNormalMode(void)
{
  interface->selectPeripheral(interface, &peripheral);
  uint16_t baud_rate_register_value =
    calculateBaudRateRegisterValue(cpu_frequency, 9600);
  TEST_ASSERT_EQUAL_UINT8((uint8_t)baud_rate_register_value,
                          baud_rate_register_low);
}

void
test_selectUsartPeripheralSetsBaudRateLowForNormalModeAndSmallCPUFrequency(
  void)
{
  baud_rate_register_high = 0xFF;
  uint32_t one_mhz        = 1000000;
  config.cpu_frequency    = one_mhz,
  PeripheralInterfaceUsartImpl_createNew(interface, &config);
  interface->selectPeripheral(interface, &peripheral);
  uint16_t baud_rate_register_value =
    calculateBaudRateRegisterValue(config.cpu_frequency, 9600);
  TEST_ASSERT_EQUAL_UINT8((uint8_t)baud_rate_register_value,
                          baud_rate_register_low);
}

void
test_selectUsartPeripheralSetsBaudRateLowForNormalModeAndOtherBaudRate(void)
{
  uint32_t one_mhz     = 1000000;
  config.cpu_frequency = one_mhz;
  peripheral.baud_rate = USART_BAUD_RATE_115200;
  PeripheralInterfaceUsartImpl_createNew(interface, &config);
  interface->selectPeripheral(interface, &peripheral);
  uint16_t baud_rate_register_value =
    calculateBaudRateRegisterValue(config.cpu_frequency, 115200);
  TEST_ASSERT_EQUAL_UINT8((uint8_t)baud_rate_register_value,
                          baud_rate_register_low);
}

void
test_selectUsartPeripheralSetsBaudRateHighForNormalModeAndOtherBaudRate(void)
{
  uint32_t two_mhz     = 2000000;
  config.cpu_frequency = two_mhz;
  peripheral.baud_rate = USART_BAUD_RATE_2400;
  PeripheralInterfaceUsartImpl_createNew(interface, &config);
  interface->selectPeripheral(interface, &peripheral);
  uint16_t baud_rate_register_value =
    calculateBaudRateRegisterValue(config.cpu_frequency, 2400);
  TEST_ASSERT_EQUAL_UINT8((uint8_t)baud_rate_register_value,
                          baud_rate_register_low);
}

void
test_selectingPeripheralEnablesTransmitter(void)
{
  usart_control_and_status_register_b = 0x80;
  interface->selectPeripheral(interface, &peripheral);
  TEST_ASSERT_BIT_HIGH(usart_transmitter_enable_bit,
                       usart_control_and_status_register_b);
}

void
test_selectingPeripheralEnablesReceiver(void)
{
  interface->selectPeripheral(interface, &peripheral);
  TEST_ASSERT_BIT_HIGH(usart_receiver_enable_bit,
                       usart_control_and_status_register_b);
}

void
test_selectingPeripheralSetsDataBits(void)
{
  interface->selectPeripheral(interface, &peripheral);
  TEST_ASSERT_BIT_HIGH(usart_character_size_0_bit,
                       usart_control_and_status_register_c);
  TEST_ASSERT_BIT_HIGH(usart_character_size_1_bit,
                       usart_control_and_status_register_c);
}

void
test_selectingUnsupportedDataSizeThrowsException(void)
{
  CEXCEPTION_T exception;
  Try
  {
    peripheral.data_bits = 255;
    interface->selectPeripheral(interface, &peripheral);
    TEST_FAIL_MESSAGE("exception not thrown");
  }
  Catch(exception)
  {
    TEST_ASSERT_EQUAL_UINT8(
      PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION, exception);
  }
}

void
test_selectingUnsupportedBaudRateThrowsException(void)
{
  CEXCEPTION_T exception;
  Try
  {
    peripheral.baud_rate = 244;
    interface->selectPeripheral(interface, &peripheral);
    TEST_FAIL_MESSAGE("exception not thrown");
  }
  Catch(exception)
  {
    TEST_ASSERT_EQUAL_UINT8(
      PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION, exception);
  }
}

void
test_selectingPeripheralSetsStopBit(void)
{
  usart_control_and_status_register_c = 0xFF;
  interface->selectPeripheral(interface, &peripheral);
  TEST_ASSERT_BIT_LOW(usart_stop_bit_select_bit,
                      usart_control_and_status_register_c);
}

void
test_selectingUnsupportedStopBitThrowsException(void)
{
  peripheral.stop_bits = 23;
  CEXCEPTION_T exception;
  Try
  {
    interface->selectPeripheral(interface, &peripheral);
    TEST_FAIL_MESSAGE("exception not thrown");
  }
  Catch(exception)
  {
    TEST_ASSERT_EQUAL(
      PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION, exception);
  }
}

void
test_selectingPeripheralSetsNoParity(void)
{
  interface->selectPeripheral(interface, &peripheral);
  TEST_ASSERT_BIT_LOW(usart_parity_mode_0_bit,
                      usart_control_and_status_register_c);
}

void
test_selectingUnsupportedParityThrowsException(void)
{
  peripheral.parity = 244;
  CEXCEPTION_T exception;
  Try
  {
    interface->selectPeripheral(interface, &peripheral);
    TEST_FAIL_MESSAGE("no exception thrown");
  }
  Catch(exception)
  {
    TEST_ASSERT_EQUAL(
      PERIPHERALINTERFACE_UNSUPPORTED_PERIPHERAL_SETUP_EXCEPTION, exception);
  }
}

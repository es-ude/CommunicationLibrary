#include <stdint.h>
#include "PeripheralInterface/PeripheralSPIImpl.h"
#include "PeripheralInterface/PeripheralInterface.h"
#include "Setup/HardwareSetup.h"
#include "PeripheralInterface/Usart.h"
#include "Util/Debug.h"
#include "Setup/DebugSetup.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

PeripheralInterfaceSPIImpl peripheral_interface_struct;
PeripheralInterface *peripheral_interface = (PeripheralInterface*) &peripheral_interface_struct;
Mac802154 *mac802154 = NULL;

SPISlave mrf_spi_client = {
  .data_register           = &PORTF,
  .clock_rate_divider      = SPI_CLOCK_RATE_DIVIDER_64,
  .data_order              = SPI_DATA_ORDER_MSB_FIRST,
  .idle_signal             = SPI_IDLE_SIGNAL_HIGH,
  .data_direction_register = &DDRF,
  .slave_select_pin        = 1,
  .spi_mode                = SPI_MODE_0,
};

void
setUpPeripheral(void)
{
  SPIConfig elastic_node_spi_mrf_config = {
    .clock_pin                        = 1,
    .miso_pin                         = 3,
    .mosi_pin                         = 2,
    .slave_select_pin                 = 0,
    .control_register                 = &SPCR,
    .data_register                    = &SPDR,
    .status_register                  = &SPSR,
    .io_lines_data_direction_register = &DDRB,
    .io_lines_data_register           = &PORTB
  };
  SPISlave flash_chip_config = {
    .data_direction_register = &DDRB,
    .data_register = &PORTB,
    .slave_select_pin = 4,
    .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_32,
    .data_order = SPI_DATA_ORDER_LSB_FIRST,
    .idle_signal = SPI_IDLE_SIGNAL_HIGH,
    .spi_mode = SPI_MODE_0
  };
  PeripheralInterfaceSPI_createNew(peripheral_interface,
                                   &elastic_node_spi_mrf_config);
  PeripheralInterface_selectPeripheral(peripheral_interface, &flash_chip_config);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &flash_chip_config);
}

void
setUpMac(void)
{
  setUpPeripheral();
  if (mac802154 == NULL)
  {
    MRFConfig mrf_hardware_config = {
            .delay_microseconds = delay_microseconds,
            .device = &mrf_spi_client,
            .interface = peripheral_interface,
            .reset_line = {
                    .data_direction_register = &DDRB,
                    .data_register = &PORTB,
                    .pin_number = 5,
            },
            .transmitter_power = 0,
    };
    mac802154 = malloc(Mac802154MRF_getADTSize());
    Mac802154MRF_create(mac802154, &mrf_hardware_config);
  }
}

static PeripheralInterfaceUsartImpl debug_usart_impl;
static PeripheralInterface *debug_interface = (PeripheralInterface *)&debug_usart_impl;
static UsartPeripheral terminal;

void
setUpDebugging(void)
{
  UsartConfig usart_config = {
      .control_and_status_register_a = &UCSR1A,
      .control_and_status_register_b = &UCSR1B,
      .control_and_status_register_c = &UCSR1C,
      .data_register                 = &UDR1,
      .baud_rate_register_high       = &UBRR1H,
      .baud_rate_register_low        = &UBRR1L,
      .cpu_frequency                 = F_CPU,
  };
  terminal = PeripheralInterfaceUsartImpl_getDefaultPeripheral();
  PeripheralInterfaceUsartImpl_createNew(debug_interface, &usart_config);
}

void
printString(const char *string)
{
  PeripheralInterface_selectPeripheral(debug_interface, &terminal);
  uint8_t length = strlen(string);
  PeripheralInterface_writeBlocking(debug_interface, string, length);
  PeripheralInterface_deselectPeripheral(debug_interface, &terminal);
}

void
printNewLine(void)
{
  printString("\n");
}

void
printDec16(uint16_t number)
{
  char text[10];
  sprintf(text, "%u", number);
  printString(text);
}
#include <stdint.h>
#include "PeripheralInterface/PeripheralSPIImpl.h"
#include "PeripheralInterface/PeripheralInterface.h"
#include "Setup/HardwareSetup.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

PeripheralInterfaceSPIImpl peripheral_interface_struct;
PeripheralInterface *peripheral_interface = (PeripheralInterface*) &peripheral_interface_struct;
Mac802154 *mac802154 = NULL;

SPISlave mrf_spi_client = {
  .data_register           = &PORTE,
  .clock_rate_divider      = SPI_CLOCK_RATE_DIVIDER_64,
  .data_order              = SPI_DATA_ORDER_MSB_FIRST,
  .idle_signal             = SPI_IDLE_SIGNAL_HIGH,
  .data_direction_register = &DDRE,
  .slave_select_pin        = 6,
  .spi_mode                = SPI_MODE_0,
};

// V3
// MCU_MOSI    PB2
// MCU_MISO    PB3
// WIRELESS_CS    PF1
// WIRELESS_RESET  PB5
// WIRELESS_WAKE  PE2
// WIRELESS_INT  PF0


// V4 
// MCU_MOSI    PB2
// MCU_MISO    PB3
// WIRELESS_CS    PE6
// WIRELESS_RESET  PB5
// WIRELESS_WAKE  PF1
// WIRELESS_INT  PF0

void
setUpPeripheral(void)
{
  SPIConfig elastic_node_spi_mrf_config = { // should be elastic_node_spi_config
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


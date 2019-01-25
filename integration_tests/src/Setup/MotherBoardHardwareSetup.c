#include "integration_tests/src/Setup/MrfHardwareSetup.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


PeripheralInterfaceSPIImpl peripheral_interface_struct;
PeripheralInterface *peripheral_interface = (PeripheralInterface*) &peripheral_interface_struct;
Mac802154 mac802154 = NULL;

void setUpPeripheral(void) {
  static SPIConfig spi_config = {
        .data_register = &SPDR,
        .clock_pin = PORTB1,
        .miso_pin = PORTB3,
        .mosi_pin = PORTB2,
        .slave_select_pin = PORTB0,
        .io_lines_data_direction_register = &DDRB,
        .io_lines_data_register = &PORTB,
        .status_register = &SPSR,
        .control_register = &SPCR,
    };
  PeripheralInterfaceSPI_createNew(peripheral_interface, &spi_config);
}

SPISlave mrf_spi_client = {
        .data_register = &PORTB,
        .data_direction_register = &DDRB,
        .slave_select_pin = 4,
        .data_order = SPI_DATA_ORDER_MSB_FIRST,
        .spi_mode = SPI_MODE_0,
        .idle_signal = SPI_IDLE_SIGNAL_HIGH,
        .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_64,
};

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
                    .data_direction_register = NULL,
            },
            .transmitter_power = 0,
    };
    mac802154 = malloc(Mac802154MRF_getADTSize());
    Mac802154MRF_create(mac802154, &mrf_hardware_config);
  }
}
#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

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

PeripheralInterface peripheral_interface = NULL;

void setUpPeripheral(void) {
  peripheral_interface = malloc(PeripheralInterfaceSPI_requiredSize());
  PeripheralInterfaceSPI_createNew((uint8_t *)peripheral_interface, &spi_config);
  PeripheralInterface_init(peripheral_interface);
  PeripheralInterface_configurePeripheral(peripheral_interface, &mrf_spi_client);
}

void delay_microseconds(double microseconds) {
  while (microseconds > 0){
    _delay_ms(1);
    microseconds--;
  }
}

PeripheralSPI mrf_spi_client = {
        .data_register = &PORTB,
        .data_direction_register = &DDRB,
        .select_chip_pin_number = 4,
        .data_order = SPI_DATA_ORDER_MSB_FIRST,
        .spi_mode = SPI_MODE_0,
        .idle_signal = SPI_IDLE_SIGNAL_HIGH,
        .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_64,
};

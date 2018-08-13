#include "config.h"
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "CException.h"
#include "lib/include/Mac802154MRFImpl.h"
#include <avr/io.h>
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
Mac802154 *mac802154 = NULL;

Mac802154Config mac_config;

static void setUpPeripheral(void) {
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

static void setUpMac802154(void) {
  mac_config.interface = peripheral_interface;
  mac_config.channel = 11;
  mac_config.short_source_address = 0x1122;
  mac_config.pan_id = 0x1234;
  mac_config.extended_source_address = 0x1122334455667788;
  mac_config.device = &mrf_spi_client;
  mac802154 = malloc(Mac802154MRF_requiredSize());
  Mac802154MRF_create((uint8_t *)mac802154, delay_microseconds);
  Mac802154_init(mac802154, &mac_config);
}

void setup(void) {
  setUpUsbSerial();
  _delay_ms(1000);
  setUpPeripheral();
//  setUpMac802154();
}

void debug(uint8_t *string) {
  usbWriteString(string);
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

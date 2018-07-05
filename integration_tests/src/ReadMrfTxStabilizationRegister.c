#include <stdlib.h>
#include <util/delay.h>
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"

uint8_t* setup(void);
uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);
void debug(const uint8_t *string);

static SPIConfig spi_config = {
        .data_register = &SPDR,
        .clock_pin = PORTB1,
        .miso_pin = PORTB3,
        .mosi_pin = PORTB2,
        .io_lines_data_direction_register = &DDRB,
        .status_register = &SPSR,
        .control_register = &SPCR,
};

static PeripheralSPI spi_chip = {
        .data_register = &PORTB,
        .data_direction_register = &DDRB,
        .select_chip_pin_number = PORTB0,
        .data_order = SPI_DATA_ORDER_MSB_FIRST,
        .clock_polarity = SPI_CLOCK_POLARITY_LEADING_EDGE_RISING,
        .clock_phase = SPI_CLOCK_PHASE_LEADING_EDGE_SAMPLE,
        .idle_signal = SPI_IDLE_SIGNAL_LOW,
        .clock_rate_divider = SPI_CLOCK_RATE_DIVIDER_64,
};

static PeripheralInterface spi_interface;


int main(void){
  setup();
  char output[] = "0x00\n";
  uint8_t byte = 0xAB;
  usbWriteString("Start\n");
  periodicUsbTask();
  for(;;) {
    byte = readByteFromShortAddressRegister(mrf_register_tx_stabilization);
    convertByteToString(byte, output);
    usbWriteString(output);
    _delay_ms(1000);
    periodicUsbTask();
  }
}



uint8_t* setup(void) {
  setUpUsbSerial();
  _delay_ms(3000);
  uint8_t *memory = malloc(PeripheralInterfaceSPI_requiredSize());
  spi_interface = PeripheralInterfaceSPI_createNew(memory, &spi_config);
  PeripheralInterface_init(spi_interface);
  PeripheralInterface_configurePeripheral(spi_interface, &spi_chip);
}

uint8_t readByteFromShortAddressRegister(uint8_t register_address) {
  uint8_t bit_mask = 0b01111110;
  uint8_t command = register_address << 1;
  command &= bit_mask;
  uint8_t buffer = 0;
  PeripheralInterface_selectPeripheral(spi_interface, &spi_chip);
  PeripheralInterface_writeBlocking(spi_interface, &command, 1);
  PeripheralInterface_readBlocking(spi_interface, &buffer, 1);
  PeripheralInterface_deselectPeripheral(spi_interface, &spi_chip);
  return buffer;
}

uint8_t convertNumberToASCII(uint8_t number) {
  if ( number >= 0 && number <= 9) {
    return (uint8_t )('0' + number);
  }
  else {
    return (uint8_t )('A' + (number - 10));
  }
}

void convertByteToString(uint8_t byte, uint8_t *string) {
  uint8_t upper_half = byte >> 4;
  uint8_t lower_half = (uint8_t) (byte & 0x0F);
  string[2] = convertNumberToASCII(upper_half);
  string[3] = convertNumberToASCII(lower_half);
}

void debug(const uint8_t *string) {
  usbWriteString(string);
  periodicUsbTask();
}

void debugPrintHex(uint8_t byte) {
  uint8_t string[] = "0x00\n";
  convertByteToString(byte, string);
  debug(string);
}
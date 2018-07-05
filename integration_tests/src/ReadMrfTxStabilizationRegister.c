#include <stdlib.h>
#include <util/delay.h>
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "lib/include/Peripheral.h"
#include "lib/include/TransferLayer/PeripheralSPIImpl.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"

void setup(void);
uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);

typedef struct SPIPeripheral{
  volatile uint8_t *DDR;
  uint8_t PIN;
  volatile  uint8_t *PORT;
} SPIPeripheral;

SPIPeripheral peripheral = {
        .DDR = &DDRB,
        .PIN = 0,
        .PORT = &PORTB,
};

SPIConfig spi_config = {
        .ddr = &DDR_SPI,
        .port = &PORTB,
        .spcr = &SPCR,
        .spdr = &SPDR,
        .spsr = &SPSR,
        .sck_rate = f_osc_16,
};

TransferLayerConfig transfer_layer_config = {
        .allocate = malloc,
        .deallocate = free,
};

PeripheralInterface *spi_interface;

int main(void){
  setup();
  char output[] = "0x00\n";
  uint8_t byte = 0xAB;
  _delay_ms(2000);
  usbWriteString("Start\n");
  periodicUsbTask();
  for(;;) {
    _delay_ms(2000);
    byte = readByteFromShortAddressRegister(mrf_register_tx_stabilization);
    convertByteToString(byte, output);
    usbWriteString(output);
    periodicUsbTask();
  }
}

void setup(void) {
  spi_interface = PeripheralInterface_create(transfer_layer_config, spi_config);
  PeripheralInterface_init(spi_interface);
  spi_interface->configurePeripheral(spi_interface, (Peripheral*)&peripheral);
  setUpUsbSerial();
}

uint8_t readByteFromShortAddressRegister(uint8_t register_address) {
  uint8_t bit_mask = 0b01111110;
  uint8_t command = register_address << 1;
  command &= bit_mask;
  uint8_t buffer = 0;
  PeripheralInterface_selectPeripheral(spi_interface, &peripheral);
  PeripheralInterface_writeBlocking(spi_interface, &command, 1);
  PeripheralInterface_readBlocking(spi_interface, &buffer, 1);
  PeripheralInterface_deselectPeripheral(spi_interface, &peripheral);
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
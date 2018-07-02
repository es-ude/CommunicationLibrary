#include <stdlib.h>
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

uint8_t slave_select_pin = 1;

SPIPeripheral peripheral = {
        .DDR = &DDRB,
        .PIN = 1,
        .PORT = &PORTB,
};

SPIConfig spi_config = {
        .ddr = &DDRB,
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
  char output[] = "0x00";
  uint8_t byte = 0;
  for(;;) {
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
  uint8_t command = register_address << 1;
  uint8_t buffer = 0;
  PeripheralInterface_selectPeripheral(spi_interface, &peripheral);
  PeripheralInterface_writeBlocking(spi_interface, command, 1);
  PeripheralInterface_readBlocking(spi_interface, &buffer, 1);
  PeripheralInterface_deselectPeripheral(spi_interface, &peripheral);
  return buffer;
}

void convertByteToString(uint8_t byte, uint8_t *string) {
  string = "0x00";
  string[2] += byte >> 1;
  string[3] += byte & 0x01;
}
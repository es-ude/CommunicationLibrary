#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "integration_tests/src/config.h"
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"

void printTxMemory(void);
void convertByteToString(uint8_t byte, uint8_t *string);
uint8_t readByteFromShortAddressRegister(uint8_t register_address);

int main(void) {
  _delay_ms(2000);
  debug("Start\n");
  setup();
  debug("setup done\n");

  uint8_t output[] = "0x00\n";
//  writeTxMemory();

  for (;;){
    uint8_t byte = readByteFromShortAddressRegister(mrf_register_tx_stabilization);
    convertByteToString(byte, output);
    debug(output);
//    printTxMemory();
    _delay_ms(1000);
    periodicUsbTask();
  }

}

void printTxMemory(void) {
  uint8_t read_tx_command[] = {(uint8_t)(MRF_readLongCommand(0) >> 8), (uint8_t) (MRF_readLongCommand(8))};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, read_tx_command, 2);
  uint8_t buffer[32];
  PeripheralInterface_readBlocking(peripheral_interface, buffer, 32);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  debug(buffer);
  debug("\n");
}

void writeTxMemory(void) {
  uint8_t write_tx_command[] = {MRF_writeLongCommandFirstByte(0), MRF_writeLongCommandSecondByte(0)};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, write_tx_command, 2);
  PeripheralInterface_writeBlocking(peripheral_interface, "hello world", strlen("hello world")+1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

uint8_t readByteFromShortAddressRegister(uint8_t register_address) {
  uint8_t command = register_address << 1;
  uint8_t buffer = 0;
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, &command, 1);
  PeripheralInterface_readBlocking(peripheral_interface, &buffer, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
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
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "include/Peripheral/PeripheralSPIImpl.h"
#include "integration_tests/src/config.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "integration_tests/LUFA-Setup/Helpers.h"


/*
 * Current Bug is about the read write output_buffer. Reading/Writing one Byte
 * works fine, where as reading/writing multiple bytes doesn't and yields
 * just 0s on reads from the first time on.
 * Current test setup leads to alternating output of 0x00 and 0x75.
 */
uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);

void printTxMemory(void) {
  uint16_t address = 0;
  uint8_t address_high = address >> 3 | 0x80;
  uint8_t address_low = address << 5;
  uint8_t read_tx_command[] = {address_high, address_low};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, read_tx_command, 2);
  uint8_t buffer[32];
  PeripheralInterface_readBlocking(peripheral_interface, buffer, 15);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  debug("content: ");
  debug(buffer);
  debug("\n");
}

void writeTxMemory(void) {
  uint16_t address = 0;
  uint8_t address_high = (address >> 3) | 0x80;
  uint8_t address_low = (address << 5) | 0x10;
  uint8_t write_tx_command[] = {address_high, address_low};
  uint8_t data[] = "hello, world!";
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, write_tx_command, 2);
  PeripheralInterface_writeBlocking(peripheral_interface, data, strlen((char *) data)+1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void printTxStabilizationRegister(void) {
  char output[] = "0x00\n";
  uint8_t byte = readByteFromShortAddressRegister(mrf_register_tx_stabilization);
  convertByteToString(byte, output);
  usbWriteString(output);
}

int main(void){
  setUpPeripheral();
  setUpUsbSerial();
  uint8_t byte = 0xAB;
  periodicUsbTask();
  _delay_ms(2000);
  usbWriteString("Start\n");

  writeTxMemory();
  for(;;) {
    _delay_ms(1000);
//    writeTxMemory();
    printTxMemory();
//    printTxStabilizationRegister();
    periodicUsbTask();
  }
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
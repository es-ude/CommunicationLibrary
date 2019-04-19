#include <stdlib.h>
#include <util/delay.h>
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "PeripheralInterface/PeripheralSPIImpl.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "integration_tests/src/Setup/HardwareSetup.h"
#include "integration_tests/src/Setup/DebugSetup.h"
#include "Util/Debug.h"

uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);

int main(void){
  setUpDebugging();
  setUpPeripheral();
  char output[] = "0x00\n";
  uint8_t byte = 0xAB;
  debug(String, "Start\n");
  for(;;) {
    byte = readByteFromShortAddressRegister(mrf_register_tx_stabilization);

    convertByteToString(byte, output);
    debug(String, output);
    _delay_ms(1000);
  }
}

void select(void) {
  PORTB &= ~(_BV(PORTB0));
}

void deselect(void) {
  PORTB |= _BV(PORTB0);
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

void debugPrintHex(uint8_t byte) {
  uint8_t string[] = "0x00\n";
  convertByteToString(byte, string);
  debug(String, string);
}
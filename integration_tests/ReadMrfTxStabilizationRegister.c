#include <stdlib.h>
#include <util/delay.h>

#include "integration_tests/HelpersForUsageWithoutCommModule.h"
#include "PeripheralInterface/PeripheralSPIImpl.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "Setup/HardwareSetup.h"
#include "Setup/DebugSetup.h"
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

uint8_t readByteFromShortAddressRegister(uint8_t register_address) {
  uint8_t command = register_address << 1;
  uint8_t buffer = 0;
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, &command, 1);
  PeripheralInterface_readBlocking(peripheral_interface, &buffer, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  return buffer;
}
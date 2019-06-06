#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Setup/HardwareSetup.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "src/Mac802154/MRF/MRFHelperFunctions.h"

#include "Setup/DebugSetup.h"
#include "PeripheralInterface/PeripheralInterface.h"
#include "Util/Debug.h"
#include <stdio.h>

/**
 * The test writes and reads the short source address high and low register
 * on the mrf chip. It resides in the mrf's short address memory.
 * Other than registers in the long address memory these have to be set
 * byte per byte with explicit command bytes in between (Instead of just
 * specifying wether to read or write as well as the starting point and then
 * read/write all following bytes consecutively).
 * This test serves as a proof for this fact.
 *
 */

void doEachIOByteWise(void);
void doEachIOAsByteString(void);

int main(void) {
  setUpDebugging();
  setUpPeripheral();

  _delay_ms(1000);
  debug(String, "Start\n");
  doEachIOByteWise();
  doEachIOAsByteString();
  while(true)
    ;
}

void doEachIOByteWise(void) {
  _delay_ms(1000);
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  uint8_t write_command = MRF_writeShortCommand(mrf_register_short_address_low_byte);
  PeripheralInterface_writeBlocking(peripheral_interface, &write_command, 1);
  uint8_t short_address[] = {0xFF, 0xFF};
  PeripheralInterface_writeBlocking(peripheral_interface, short_address, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  write_command = MRF_writeShortCommand(mrf_register_short_address_high_byte);
  PeripheralInterface_writeBlocking(peripheral_interface, &write_command, 1);
  PeripheralInterface_writeBlocking(peripheral_interface, short_address+1, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  uint8_t read_command = MRF_readShortCommand(mrf_register_short_address_low_byte);
  PeripheralInterface_writeBlocking(peripheral_interface, &read_command, 1);
  uint8_t read_short_address[2] = {0, 0};
  PeripheralInterface_readBlocking(peripheral_interface, read_short_address, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  read_command = MRF_readShortCommand(mrf_register_short_address_high_byte);
  PeripheralInterface_writeBlocking(peripheral_interface, &read_command, 1);
  PeripheralInterface_readBlocking(peripheral_interface, read_short_address+1, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  char debug_string[32];
  sprintf(debug_string, "bytewise transfer: 0x%X 0x%X\n", read_short_address[0], read_short_address[1]);
  debug(String, debug_string);
  debug(String, "expected: 0xFF 0xFF\n");

}

void doEachIOAsByteString(void) {
  _delay_ms(1000);
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
//  uint8_t write_command[2] = {
//          MRF_writeLongCommandFirstByte(mrf_tx_fifo_start),
//          MRF_writeLongCommandSecondByte(mrf_tx_fifo_start)
//  };
  uint8_t write_command[] = {
          MRF_writeShortCommand(mrf_register_short_address_low_byte),
  };
  PeripheralInterface_writeBlocking(peripheral_interface, write_command, 1);
  uint8_t short_address[] = {0xAA, 0xAA};
  PeripheralInterface_writeBlocking(peripheral_interface, short_address, 2);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  uint8_t read_command[] = {
          MRF_readShortCommand(mrf_register_short_address_low_byte)
  };
//  uint8_t read_command[] = {
//          MRF_readLongCommandFirstByte(mrf_tx_fifo_start),
//          MRF_readLongCommandSecondByte(mrf_tx_fifo_start),
//  };
  PeripheralInterface_writeBlocking(peripheral_interface, read_command, 1);
  uint8_t read_short_address[2] = {0, 0};
  PeripheralInterface_readBlocking(peripheral_interface, read_short_address, 1);
  read_command[0] = MRF_readShortCommand(mrf_register_short_address_high_byte);
  PeripheralInterface_writeBlocking(peripheral_interface, read_command, 1);
  PeripheralInterface_readBlocking(peripheral_interface, read_short_address+1, 1);

  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  char debug_string[32];
  sprintf(debug_string, "byte string transfer: 0x%X 0x%X\n", read_short_address[0], read_short_address[1]);
  debug(String, debug_string);
  debug(String, "expected: 0xAA 0xAA\n");
}
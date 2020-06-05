#include <stdio.h>
#include <stdint.h>
#include "src/Mac802154/MRF/MrfIo.h"
#include "src/Mac802154/MRF/MRFHelperFunctions.h"
#include "EmbeddedUtilities/Debug.h"

static void setWriteLongCommand(MrfIo *mrf, uint16_t address);
static void writeBlockingWithCommand(MrfIo *mrf, const uint8_t *payload, uint8_t size);
static void setWriteShortCommand(MrfIo *mrf, uint8_t address);
static void setWriteLongCommand(MrfIo *mrf, uint16_t address);
static void setReadShortCommand(MrfIo *mrf, uint8_t address);
static void setReadLongCommand(MrfIo *mrf, uint16_t address);

static bool isLongAddress(uint16_t address);

void MrfIo_writeBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint16_t address) {
  setWriteLongCommand(mrf, address);
  writeBlockingWithCommand(mrf, payload, size);
}

void setWriteLongCommand(MrfIo *mrf, uint16_t address) {
  mrf->command[0] = MRF_writeLongCommandFirstByte(address);
  mrf->command[1] = MRF_writeLongCommandSecondByte(address);
  mrf->command_size = 2;
}

/**
* IMPORTANT:
* The scheme for io operation with spi shown
* in the datasheet is misleading. It is possible
* to write/read a sequence of bytes after specifying
* the starting point with the first control byte(s)
* instead of addressing every byte explicitly. This
* almost halves the necessary data for getting your packet
* onto the mrf's tx memory. However this does not seem
* to work for the short address memory. There you'll
* have to precede every byte you want to transmit by
* the corresponding control sequence.
*
*/
void MrfIo_writeBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint8_t address) {
  for (uint8_t i=0; i<size; i++) {
    setWriteShortCommand(mrf, address+i);
    writeBlockingWithCommand(mrf, payload+i, 1);
  }
}

void setWriteShortCommand(MrfIo *mrf, uint8_t address) {
  mrf->command_size = 1;
  mrf->command[0] = MRF_writeShortCommand(address);
}

void setReadLongCommand(MrfIo *mrf, uint16_t address) {
  mrf->command_size = 2;
  mrf->command[0] = MRF_readLongCommandFirstByte(address);
  mrf->command[1] = MRF_readLongCommandSecondByte(address);
}

void setReadShortCommand(MrfIo *mrf, uint8_t address) {
  mrf->command_size = 1;
  mrf->command[0] = MRF_readShortCommand(address);
}


void writeBlockingWithCommand(MrfIo *mrf, const uint8_t *payload, uint8_t size){
  debug(String, "selecting peripheral...\n");
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  debug(String, "done.\n writeBlocking...\n");
  PeripheralInterface_writeBlocking(mrf->interface, mrf->command, mrf->command_size);
  debug(String, "done.\n writeBlocking...\n");
  PeripheralInterface_writeBlocking(mrf->interface, payload, size);
  debug(String, "done.\n deselecting peripheral...\n");
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
  debug(String, "done.\n");
}

void readBlockingWithCommand(MrfIo *mrf, uint8_t *payload, uint8_t size) {
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  PeripheralInterface_writeBlocking(mrf->interface, mrf->command, mrf->command_size);
  PeripheralInterface_readBlocking(mrf->interface, payload, size);
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
}

void MrfIo_setControlRegister(MrfIo *mrf, uint16_t address, uint8_t value) {
  if (isLongAddress(address)) {
    debug(String, "write command long address...");
    setWriteLongCommand(mrf, address);
  }
  else {
    debug(String, "write command short address...");
    setWriteShortCommand(mrf, address);
  }
  debug(String, "start writing...\n");
  writeBlockingWithCommand(mrf, &value, 1);
}

uint8_t MrfIo_readControlRegister(MrfIo *mrf, uint16_t address) {
  if (isLongAddress(address)) {
    setReadLongCommand(mrf, address);
  }
  else {
    setReadShortCommand(mrf, (uint8_t )address);
  }
  uint8_t value = 0;
  readBlockingWithCommand(mrf, &value, 1);
  return value;
}

bool isLongAddress(uint16_t address){
  uint8_t last_short_control_register_address = 0x3F;
  return (address > last_short_control_register_address);
}

void MrfIo_readBlockingFromLongAddress(MrfIo *mrf, uint16_t register_address, uint8_t *buffer, uint8_t length) {
  setReadLongCommand(mrf, register_address);
  readBlockingWithCommand(mrf, buffer, length);
}

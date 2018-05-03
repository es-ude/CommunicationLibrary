#include "lib/src/Mac802154/MRF/MrfIo.h"

static void setWriteLongCommand(MrfIo *mrf, uint16_t address);
static void writeBlockingWithCommand(MrfIo *mrf, const uint8_t *payload, uint8_t size);

void MrfIo_writeBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint16_t address) {
  setWriteLongCommand(mrf, address);
  writeBlockingWithCommand(mrf, payload, size);
}

void setWriteLongCommand(MrfIo *mrf, uint16_t address) {
  mrf->command[0] = MRF_writeLongCommandFirstByte(address);
  mrf->command[1] = MRF_writeLongCommandSecondByte(address);
  mrf->command_size = 2;
}

void MrfIo_writeBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint8_t address) {
  mrf->command_size = 1;
  mrf->command[0] = MRF_writeShortCommand(address);
  writeBlockingWithCommand(mrf, payload, size);
}

void writeBlockingWithCommand(MrfIo *mrf, const uint8_t *payload, uint8_t size) {
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  PeripheralInterface_writeBlocking(mrf->interface, mrf->command, mrf->command_size);
  PeripheralInterface_writeBlocking(mrf->interface, payload, size);
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
}

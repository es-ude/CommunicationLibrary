#include <stddef.h>
#include <stdio.h>
#include "lib/src/Mac802154/MRF/MrfIo.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"

static void setWriteLongCommand(MrfIo *mrf, uint16_t address);
static void writeBlockingWithCommand(MrfIo *mrf, const uint8_t *payload, uint8_t size);
static void setWriteShortCommand(MrfIo *mrf, uint8_t address);
static void clearPeripheralWriteCallback(PeripheralInterface interface);
static void clearMrfIoWriteCallback(MrfIo *mrf);
static void callbackForDeselect(void *mrf);
static void callbackForWritingData(void *mrf);
extern void debug(const uint8_t *text);

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

void MrfIo_writeBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint8_t address) {
  setWriteShortCommand(mrf, address);
  writeBlockingWithCommand(mrf, payload, size);
}

void callbackForWritingData(void *arg) {
  MrfIo *mrf = (MrfIo *) arg;
  PeripheralCallback callback = {
          .function = callbackForDeselect,
          .argument = mrf,
  };
  PeripheralInterface_setWriteCallback(mrf->interface, callback);
  PeripheralInterface_writeNonBlocking(mrf->interface, mrf->output_buffer, mrf->length);
}

void callbackForDeselect(void *arg) {
  MrfIo *mrf = (MrfIo *) arg;

  clearPeripheralWriteCallback(mrf->interface);
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
  if(mrf->callback.function != NULL)
    mrf->callback.function(mrf->callback.argument);
  clearMrfIoWriteCallback(mrf);
}

void clearMrfIoWriteCallback(MrfIo *mrf) {
  MrfIoCallback callback = {
          .argument = NULL,
          .function = NULL,
  };
  MrfIo_setWriteCallback(mrf, callback);
}

void clearPeripheralWriteCallback(PeripheralInterface interface) {
  PeripheralCallback callback = {
          .function = NULL,
          .argument = NULL,
  };
  PeripheralInterface_setWriteCallback(interface, callback);
}

void MrfIo_writeNonBlockingToShortAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint8_t address) {
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  mrf->command[0] = MRF_writeShortCommand(address);
  mrf->command_size = 1;
  mrf->output_buffer = payload;
  mrf->length = size;
  PeripheralCallback callback = {
          .function = callbackForWritingData,
          .argument = mrf,
  };
  PeripheralInterface_setWriteCallback(mrf->interface, callback);
  PeripheralInterface_writeNonBlocking(mrf->interface, mrf->command, mrf->command_size);
}

void MrfIo_writeNonBlockingToLongAddress(MrfIo *mrf, const uint8_t *payload, uint8_t size, uint16_t address) {
  mrf->command_size = 2;
  mrf->command[0] = MRF_writeLongCommandFirstByte(address);
  mrf->command[1] = MRF_writeLongCommandSecondByte(address);
  mrf->output_buffer = payload;
  mrf->length = size;
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  PeripheralCallback callback = {
          .argument = mrf,
          .function = callbackForWritingData,
  };
  PeripheralInterface_setWriteCallback(mrf->interface, callback);
  PeripheralInterface_writeNonBlocking(mrf->interface, mrf->command, mrf->command_size);
}

void setWriteShortCommand(MrfIo *mrf, uint8_t address) {
  mrf->command_size = 1;
  mrf->command[0] = MRF_writeShortCommand(address);
}

void writeBlockingWithCommand(MrfIo *mrf, const uint8_t *payload, uint8_t size) {
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  PeripheralInterface_writeBlocking(mrf->interface, mrf->command, mrf->command_size);
  PeripheralInterface_writeBlocking(mrf->interface, payload, size);
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
}

void MrfIo_setWriteCallback(MrfIo *mrf, MrfIoCallback callback) {
  mrf->callback = callback;
}

void MrfIo_setControlRegister(MrfIo *mrf, uint16_t address, uint8_t value) {
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);

  if (isLongAddress(address)) {
    uint8_t command[] = {
            MRF_writeLongCommandFirstByte(address),
            MRF_writeLongCommandSecondByte(address),
    };
    PeripheralInterface_writeBlocking(mrf->interface, command, 2);

  } else {
    uint8_t command = MRF_writeShortCommand((uint8_t) address);
    PeripheralInterface_writeBlocking(mrf->interface, &command, 1);
  }
  PeripheralInterface_writeBlocking(mrf->interface, &value, 1);
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
}

uint8_t MrfIo_readControlRegister(MrfIo *mrf, uint16_t address) {
  PeripheralInterface_selectPeripheral(mrf->interface, mrf->device);
  if (isLongAddress(address)) {
    uint8_t command[] = {
            MRF_readLongCommandFirstByte(address),
            MRF_readLongCommandSecondByte(address),
    };
    PeripheralInterface_writeBlocking(mrf->interface, command, 2);
  } else {
    uint8_t command = MRF_readShortCommand((uint8_t) address);
    PeripheralInterface_writeBlocking(mrf->interface, &command, 1);
  }
  uint8_t value = 0;
  PeripheralInterface_readBlocking(mrf->interface, &value, 1);
  PeripheralInterface_deselectPeripheral(mrf->interface, mrf->device);
  return value;
}

bool isLongAddress(uint16_t address){
  uint8_t last_short_control_register_address = 0x3F;
  return (address > last_short_control_register_address);
}
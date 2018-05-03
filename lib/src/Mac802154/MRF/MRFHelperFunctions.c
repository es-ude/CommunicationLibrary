#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"

bool isLongAddress(uint16_t address){
  uint8_t last_short_control_register_address = 0x3F;
  return (address > last_short_control_register_address);
}

void MRF_setControlRegister(MRF *impl, uint16_t address, uint8_t value) {
  PeripheralInterface_selectPeripheral(impl->interface, impl->device);
  if (isLongAddress(address)) {
    uint8_t command[] = {
            MRF_writeLongCommandFirstByte(address),
            MRF_writeLongCommandSecondByte(address),
    };
    PeripheralInterface_writeBlocking(impl->interface, command, 2);
    PeripheralInterface_writeBlocking(impl->interface, &value, 1);

  } else {
    uint8_t command = MRF_writeShortCommand((uint8_t) address);
    PeripheralInterface_writeBlocking(impl->interface, &command, 1);
    PeripheralInterface_writeBlocking(impl->interface, &value, 1);
  }
  PeripheralInterface_deselectPeripheral(impl->interface, impl->device);
}

void MRF_writeBlockingToShortAddress(MRF *impl, uint8_t address, const uint8_t *buffer, uint8_t size) {
  PeripheralInterface_selectPeripheral(impl->interface, impl->device);
  uint8_t command = MRF_writeShortCommand(address);
  PeripheralInterface_writeBlocking(impl->interface, &command, 1);
  PeripheralInterface_writeBlocking(impl->interface, buffer, size);
  PeripheralInterface_deselectPeripheral(impl->interface, impl->device);
}

static void write_txTriggerBit(void *arg){
  uint8_t value = 1;
  MRF *impl = arg;
  PeripheralCallback callback = {.argument = NULL, .function = NULL};
  PeripheralInterface_setWriteCallback(impl->interface, callback);
  PeripheralInterface_writeNonBlocking(impl->interface, &value, 1);
  // deselecting here is okay, because we're only writing one byte which happens instantly
  // since we were triggered by the previous callback. We might even consider to write that last byte
  // in blocking manner. In future we might want to be notified when the last byte was written,
  // in this case using the non blocking call makes sense.
  PeripheralInterface_deselectPeripheral(impl->interface, impl->device);
}

void MRF_triggerTXSendNonBlocking(MRF *impl) {
  uint8_t command = MRF_writeShortCommand(mrf_register_tx_normal_fifo_control);
  PeripheralInterface_selectPeripheral(impl->interface, impl->device);
  PeripheralCallback callback = {.function = write_txTriggerBit, .argument = impl};
  PeripheralInterface_setWriteCallback(impl->interface, callback);
  PeripheralInterface_writeNonBlocking(impl->interface, &command, 1);
}


#ifndef COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H
#define COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H

#include <stdint.h>
#include "lib/include/Peripheral.h"
#include "lib/include/Mac802154.h"
#include "lib/include/RuntimeLibraryInterface.h"
#include "lib/src/Mac802154/MRF/MrfIo.h"

typedef struct Mrf Mrf;
struct Mrf {
  Mac802154 mac;
  MrfIo io;
  PeripheralInterface *interface;
  Peripheral *device;
  Deallocator deallocate;
  DelayFunction delay_microseconds;
  const uint8_t *payload;
};

static inline uint16_t MRF_readLongCommand(uint16_t address) {
  return (uint16_t)(1 << 15 | address << 5);
}

static inline uint16_t MRF_writeLongCommand(uint16_t address) {
  return (uint16_t)(MRF_readLongCommand(address) | 1 << 4);
}

static inline uint8_t MRF_writeShortCommand(uint8_t address) {
  return (uint8_t)((address << 1 & ~(1 << 7)) | 1);
}

static inline uint8_t MRF_writeLongCommandFirstByte(uint16_t address) {
  return (uint8_t) (MRF_writeLongCommand(address) >> 8);
}

static inline uint8_t MRF_writeLongCommandSecondByte(uint16_t address) {
  return (uint8_t ) MRF_writeLongCommand(address);
}

static inline uint8_t MRF_getRegisterValueForChannelNumber(uint8_t channel_number) {
  return (uint8_t)(((channel_number - 11) << 4) | 0x03);
}

#endif //COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H

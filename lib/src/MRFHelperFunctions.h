
#ifndef COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H
#define COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H

#include <stdint.h>
#include "lib/include/Peripheral.h"
#include "lib/include/Mac802154.h"
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct MRF MRF;
struct MRF {
  Mac802154 mac;
  PeripheralInterface *interface;
  Peripheral *device;
  Deallocator deallocate;
  DelayFunction delay_microseconds;
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

static inline uint8_t MRF_writeLongCommandHighByte(uint16_t address) {
  return (uint8_t) (MRF_writeLongCommand(address) >> 8);
}

static inline uint8_t MRF_writeLongCommandLowByte(uint16_t address) {
  return (uint8_t ) MRF_writeLongCommand(address);
}

void MRF_setControlRegister(MRF *impl, uint16_t address, uint8_t value);

#endif //COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H

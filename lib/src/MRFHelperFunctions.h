
#ifndef COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H
#define COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H

#include <stdint.h>


static inline uint16_t MRF_readLongCommand(uint16_t address) {
  return (uint16_t)(1 << 15 | address << 5);
}

static inline uint16_t MRF_writeLongCommand(uint16_t address) {
  return (uint16_t)(MRF_readLongCommand(address) | 1 << 4);
}

static inline uint8_t MRF_writeShortCommand(uint8_t address) {
  return (uint8_t)((address << 1 & ~(1 << 7)) | 1);
}

#endif //COMMUNICATIONMODULE_MRFHELPERFUNCTIONS_H
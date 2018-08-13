#ifndef COMMUNICATIONMODULE_BITMANIPULATION_H
#define COMMUNICATIONMODULE_BITMANIPULATION_H

#include <stdint.h>
#include <stdbool.h>

static inline void BitManipulation_setByte(uint8_t *field, uint8_t bitmask, uint8_t offset, uint8_t value) {
  uint8_t byte_index = offset / 8;
  uint8_t local_offset = offset % 8;
  uint8_t lower_bitmask = bitmask << local_offset;
  uint8_t upper_bitmask = bitmask >> (8 - local_offset);
  uint8_t lower_value = value << local_offset;
  uint8_t upper_value = value >> (8- local_offset);
  if (upper_bitmask != 0) {
    field[byte_index+1] |= (upper_bitmask & upper_value);
    field[byte_index+1] &= (~upper_bitmask | upper_value);
  }
  field[byte_index] |= (lower_bitmask & lower_value);
  field[byte_index] &= (~lower_bitmask |lower_value);
}

static inline uint8_t BitManipulation_getByte(const uint8_t *field, uint8_t bitmask, uint8_t offset) {
  uint8_t byte_index = offset / 8;
  uint8_t local_offset = offset % 8;
  uint8_t lower_bitmask = bitmask << local_offset;
  uint8_t upper_bitmask = bitmask >> (8 - local_offset);
  uint8_t value = (field[byte_index] & lower_bitmask) >> local_offset;
  if (upper_bitmask != 0) {
    value |= (field[byte_index + 1] & upper_bitmask) << (8 - local_offset);
  }
  return value;
}

static inline void BitManipulation_setBit(uint8_t *field, uint8_t offset){
  uint8_t byte_index = offset / 8;
  uint8_t local_offset = offset % 8;
  field[byte_index] |= (1 << local_offset);
}

static inline void BitManipulation_clearBit(uint8_t *field, uint8_t offset) {
  uint8_t byte_index = offset / 8;
  uint8_t local_offset = offset % 8;
  field[byte_index] &= ~(1 << local_offset);
}

static inline bool BitManipulation_bitIsSet(const uint8_t *field, uint8_t offset) {
  uint8_t byte_index = offset / 8;
  uint8_t local_offset = offset % 8;
  return ((field[byte_index] >> local_offset) & 1) == 1;
}

static inline void BitManipulation_fillByteArrayWith64BitLittleEndian(uint8_t *array, uint64_t value) {
  for (uint8_t index = 0; index < 8; index++)
  {
    *array = (uint8_t) (value >> index * 8);
    array++;
  }
}

static inline void BitManipulation_fillByteArrayWith16BitBigEndian(uint8_t *array, uint16_t value) {
  for (uint8_t index = 0; index < 2; index++)
  {
    *array = (uint8_t) (value >> index * 8);
    array++;
  }
}

#endif //COMMUNICATIONMODULE_BITMANIPULATION_H

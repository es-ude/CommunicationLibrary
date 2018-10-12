#ifndef COMMUNICATIONMODULE_BITMANIPULATION_H
#define COMMUNICATIONMODULE_BITMANIPULATION_H

#include <stdint.h>
#include <stdbool.h>

/**
 *
 * @param field pointer to a byte array
 * @param bitmask set the bits that you don't want to be changed to zero
 * @param offset
 * @param value
 */

static void BitManipulation_setByteOnArray(volatile uint8_t *field, uint8_t bitmask, uint8_t offset, uint8_t value) {
  uint8_t byte_index = (uint8_t) (offset / 8);
  uint8_t local_offset = (uint8_t) (offset % 8);
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

static uint8_t BitManipulation_getByteOnArray(volatile const uint8_t *field, uint8_t bitmask, uint8_t offset) {
  uint8_t byte_index = (uint8_t) (offset / 8);
  uint8_t local_offset = (uint8_t) (offset % 8);
  uint8_t lower_bitmask = bitmask << local_offset;
  uint8_t upper_bitmask = bitmask >> (8 - local_offset);
  uint8_t value = (field[byte_index] & lower_bitmask) >> local_offset;
  if (upper_bitmask != 0) {
    value |= (field[byte_index + 1] & upper_bitmask) << (8 - local_offset);
  }
  return value;
}

static void BitManipulation_setBitOnArray(volatile uint8_t *field, uint8_t offset){
  uint8_t byte_index = (uint8_t) (offset / 8);
  uint8_t local_offset = (uint8_t) (offset % 8);
  field[byte_index] |= (1 << local_offset);
}

static void BitManipulation_clearBitOnArray(volatile uint8_t *field, uint8_t offset) {
  uint8_t byte_index = (uint8_t) (offset / 8);
  uint8_t local_offset = (uint8_t) (offset % 8);
  field[byte_index] &= ~(1 << local_offset);
}

static bool BitManipulation_bitIsSetOnArray(volatile const uint8_t *field, uint8_t offset) {
  uint8_t byte_index = (uint8_t) (offset / 8);
  uint8_t local_offset = (uint8_t) (offset % 8);
  return ((field[byte_index] >> local_offset) & 1) == 1;
}

static void BitManipulation_fillByteArrayWith64BitLittleEndian(volatile uint8_t *array, uint64_t value) {
  for (uint8_t index = 0; index < 8; index++)
  {
    *array = (uint8_t) (value >> index * 8);
    array++;
  }
}

static void BitManipulation_fillByteArrayWith16BitLittleEndian(volatile uint8_t *array, uint16_t value) {
  for (uint8_t index = 0; index < 2; index++)
  {
    *array = (uint8_t) (value >> index * 8);
    array++;
  }
}

static void BitManipulation_clearBit(volatile uint8_t *byte_ptr, uint8_t offset) {
  *(byte_ptr) &= ~(1 << offset);
}

static void BitManipulation_setBit(volatile uint8_t *byte_ptr, uint8_t offset) {
  *(byte_ptr) |= (1 << offset);
}

static void BitManipulation_setByte(volatile uint8_t *byte_ptr, uint8_t bitmask, uint8_t value) {
  *byte_ptr = (~bitmask & *byte_ptr) | (bitmask & value);
}

#endif //COMMUNICATIONMODULE_BITMANIPULATION_H

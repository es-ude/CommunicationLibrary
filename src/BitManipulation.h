#ifndef COMMUNICATIONMODULE_BITMANIPULATION_H
#define COMMUNICATIONMODULE_BITMANIPULATION_H

#include <stdint.h>
#include <stdbool.h>

static bool BitManipulation_platformIsBigEndian(void) {
  uint16_t value = 1;
  uint8_t *ptr = (uint8_t*) &value;
  return ptr[0] == 1;
}

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

static void
BitManipulation_copyBytes(volatile const uint8_t *source, volatile uint8_t *destination, uint16_t length)
{
  while(length > 0)
    {
      *destination = *source;
      destination++;
      source++;
      length--;
    }
}

static void
BitManipulation_fillArray(uint8_t *array, uint8_t value, uint8_t length)
{
  while(length > 0)
    {
      *array = value;
      array++;
      length--;
    }
}

static void
BitManipulation_fillByteArrayWith64BitBigEndian(volatile uint8_t *array, uint64_t value)
{
  uint8_t *value_ptr = (uint8_t *) &value;
  if (BitManipulation_platformIsBigEndian())
    {
      for (uint8_t i=8; i > 0; i--)
        {
          array[i-1] = value_ptr[i-1];
        }
    }
  else
    {
      for (uint8_t i=8; i > 0; i--)
        {
          array[i-1] = value_ptr[8-(i-1)];
        }
    }
}

static void
BitManipulation_fillByteArrayWith16BitBigEndian(volatile uint8_t *array, uint16_t value)
{
  uint8_t *value_ptr = (uint8_t *) &value;
  if (BitManipulation_platformIsBigEndian())
    {
      for (uint8_t i = 2; i > 0; i--)
        {
          array[i - 1] = value_ptr[i - 1];
        }
    }
  else
    {
      for (uint8_t i = 2; i > 0; i--)
        {
          array[i - 1] = value_ptr[8 - (i - 1)];
        }
    }
}

static uint16_t BitManipulation_get16BitFromBigEndianByteArray(volatile const uint8_t *array) {
  uint16_t value;
  uint8_t *value_ptr = (uint8_t *) &value;
  if (BitManipulation_platformIsBigEndian())
    {
      value_ptr[0] = array[0];
      value_ptr[1] = array[1];
    }
  else
    {
      value_ptr[0] = array[1];
      value_ptr[1] = array[0];
    }
  return value;
}

static uint64_t BitManipulation_get64BitFromBigEndianByteArray(volatile const uint8_t *array) {
  uint64_t value;
  uint8_t *value_ptr = (uint8_t *) &value;
  if (BitManipulation_platformIsBigEndian())
    {
      for (uint8_t i = 8; i > 0; i--)
        {
          value_ptr[i - 1] = array[i - 1];
        }
    }
  else
    {
      for (uint8_t i = 8; i > 0; i--)
        {
          value_ptr[i - 1] = array[8 - (i - 1)];
        }
    }
  return value;
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



static uint8_t
BitManipulation_getFirstSetBitsOffsetFromByte(uint8_t byte)
{
  uint8_t offset = 8;
  uint8_t index = 0;
  while (index < 7 && offset == 8)
  {
    if (byte >> index & 1)
    {
      offset = index;
    }
  }
  return offset;
}

static uint8_t
BitManipulation_getLengthOfContinuousSetBitsFromByte(uint8_t byte)
{
  bool set_bit = false;
  while (!set_bit)
  {
    set_bit = ((byte >> 1) & 1) == 1;
  }
  uint8_t length = 0;
  while (set_bit)
  {
    set_bit = ((byte >> 1) & 1) == 1;
    length++;
  }
  return length;
}
#endif //COMMUNICATIONMODULE_BITMANIPULATION_H

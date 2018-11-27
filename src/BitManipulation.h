#ifndef COMMUNICATIONMODULE_BITMANIPULATION_H
#define COMMUNICATIONMODULE_BITMANIPULATION_H

#include <stdint.h>
#include <stdbool.h>

bool BitManipulation_platformIsBigEndian(void);

/**
 *
 * @param field pointer to a byte array
 * @param bitmask set the bits that you don't want to be changed to zero
 * @param offset
 * @param value
 */

void BitManipulation_setByteOnArray(volatile uint8_t *field, uint8_t bitmask, uint8_t offset, uint8_t value);

uint8_t BitManipulation_getByteOnArray(volatile const uint8_t *field, uint8_t bitmask, uint8_t offset);

void BitManipulation_setBitOnArray(volatile uint8_t *field, uint8_t offset);

void BitManipulation_clearBitOnArray(volatile uint8_t *field, uint8_t offset);

bool BitManipulation_bitIsSetOnArray(volatile const uint8_t *field, uint8_t offset);

void
BitManipulation_copyBytes(volatile const uint8_t *source, volatile uint8_t *destination, uint16_t length);

void
BitManipulation_fillArray(uint8_t *array, uint8_t value, uint8_t length);


void
BitManipulation_fillByteArrayWith64BitBigEndian(volatile uint8_t *array, uint64_t value);

void
BitManipulation_fillByteArrayWith16BitBigEndian(volatile uint8_t *array, uint16_t value);

uint16_t BitManipulation_get16BitFromBigEndianByteArray(volatile const uint8_t *array);

uint64_t BitManipulation_get64BitFromBigEndianByteArray(volatile const uint8_t *array);

void BitManipulation_clearBit(volatile uint8_t *byte_ptr, uint8_t offset);

void BitManipulation_setBit(volatile uint8_t *byte_ptr, uint8_t offset);

void BitManipulation_setByte(volatile uint8_t *byte_ptr, uint8_t bitmask, uint8_t value);

uint8_t
BitManipulation_getFirstSetBitsOffsetFromByte(uint8_t byte);

uint8_t
BitManipulation_getLengthOfContinuousSetBitsFromByte(uint8_t byte);

#endif //COMMUNICATIONMODULE_BITMANIPULATION_H

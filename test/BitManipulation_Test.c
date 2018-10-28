#include <memory.h>
#include "unity.h"
#include "src/BitManipulation.h"
#include "include/Mac802154.h"

void debug(const uint8_t *msg){}

void checkByteValue(uint8_t field, uint8_t field_copy, uint8_t offset, uint8_t bitmask, uint8_t value) {
  BitManipulation_setByteOnArray(&field, bitmask, offset, value);
  TEST_ASSERT_BITS(bitmask << offset, value << offset, field);
  TEST_ASSERT_BITS(~(bitmask << offset), field_copy, field);
}

void test_SetByte(void) {
  uint8_t field = 0xAB;
  uint8_t field_copy = field;
  uint8_t offset = 1;
  uint8_t bitmask = 3;
  uint8_t value = 2;
  checkByteValue(field, field_copy, offset, bitmask, value);
}

void test_SetByte2(void) {
  uint8_t field = 0xFA;
  uint8_t field_copy = field;
  uint8_t offset = 1;
  uint8_t bitmask = 3;
  uint8_t value = 0xD5;
  checkByteValue(field, field_copy, offset, bitmask, value);
}

void test_SetByte3(void) {
  uint8_t field = 0x00;
  uint8_t field_copy = field;
  uint8_t offset = 0;
  uint8_t bitmask = 0x00;
  uint8_t value = 0xFF;
  checkByteValue(field, field_copy, offset, bitmask, value);
}

void test_getByte(void) {
  uint8_t field = 0b01110000;
  uint8_t expected_value = 0b111;
  uint8_t bitmask = 0b111;
  uint8_t offset = 4;
  TEST_ASSERT_EQUAL_HEX8(expected_value, BitManipulation_getByteOnArray(&field, bitmask, offset));
}

void test_setByteSpanningTwoArrayIndices(void) {
  uint8_t field[2] = {0, 0x00};
  uint8_t expected[2] = {0xF0, 0x0F};
  uint8_t bitmask = 0xFF;
  uint8_t offset = 4;
  uint8_t value = 0xFF;
  BitManipulation_setByteOnArray(field, bitmask, offset, value);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, field, 2);
}

void test_getByteSpanningTwoArrayIndices(void) {
  uint8_t field[2] = {0xF0, 0x0F};
  uint8_t bitmask = 0xFF;
  uint8_t offset = 4;
  uint8_t expected = 0xFF;
  TEST_ASSERT_EQUAL_HEX8(expected, BitManipulation_getByteOnArray(field, bitmask, offset));
}

void test_getByte3(void) {
  uint8_t field[2] = {0x00, 2 << 6};
  uint8_t bitmask = 0b11;
  uint8_t offset = 14;
  uint8_t expected = ADDRESSING_MODE_SHORT_ADDRESS;
  TEST_ASSERT_EQUAL_HEX8(expected, BitManipulation_getByteOnArray(field, bitmask, offset));
}

void test_fillArrayWith64BitBigEndian(void) {
  uint64_t value = 0x1234;
  uint8_t array[8];
  uint8_t expected[8];
  memset(array, 0, 8);
  memset(expected, 0, 8);
  expected[0] = 0x34;
  expected[1] = 0x12;
  BitManipulation_fillByteArrayWith64BitBigEndian(array, value);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, array, 8);
}

void test_get16BitIntegerFromBigEndianArray(void) {
  uint16_t expected = 0x1122;
  uint8_t array[2] = {0x22, 0x11};
  uint16_t actual = BitManipulation_get16BitFromBigEndianByteArray(array);
  TEST_ASSERT_EQUAL_HEX16(expected, actual);

  expected = 0x6655;
  array[0] = 0x55;
  array[1] = 0x66;
  actual = BitManipulation_get16BitFromBigEndianByteArray(array);
  TEST_ASSERT_EQUAL_HEX16(expected, actual);
}

void test_get64BitIntegerFromBigEndianArray(void) {
  uint64_t expected = 0x1122334455667788;
  uint8_t array[8] = {
      0x88, 0x77, 0x66,
      0x55, 0x44, 0x33,
      0x22, 0x11, 0x00,
  };
  uint64_t actual = BitManipulation_get64BitFromBigEndianByteArray(array);
  TEST_ASSERT_EQUAL_HEX64(expected, actual);
}
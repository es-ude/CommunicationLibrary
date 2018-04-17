#include "unity.h"
#include "lib/src/BitManipulation.h"
#include "lib/include/Mac802154.h"

void checkByteValue(uint8_t field, uint8_t field_copy, uint8_t offset, uint8_t bitmask, uint8_t value) {
  BitManipulation_setByte(&field, bitmask, offset, value);
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
  TEST_ASSERT_EQUAL_HEX8(expected_value, BitManipulation_getByte(&field, bitmask, offset));
}

void test_setByteSpanningTwoArrayIndices(void) {
  uint8_t field[2] = {0, 0x00};
  uint8_t expected[2] = {0xF0, 0x0F};
  uint8_t bitmask = 0xFF;
  uint8_t offset = 4;
  uint8_t value = 0xFF;
  BitManipulation_setByte(field, bitmask, offset, value);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, field, 2);
}

void test_getByteSpanningTwoArrayIndices(void) {
  uint8_t field[2] = {0xF0, 0x0F};
  uint8_t bitmask = 0xFF;
  uint8_t offset = 4;
  uint8_t expected = 0xFF;
  TEST_ASSERT_EQUAL_HEX8(expected, BitManipulation_getByte(field, bitmask, offset));
}

void test_getByte3(void) {
  uint8_t field[2] = {0x00, 2 << 6};
  uint8_t bitmask = 0b11;
  uint8_t offset = 14;
  uint8_t expected = ADDRESSING_MODE_SHORT_ADDRESS;
  TEST_ASSERT_EQUAL_HEX8(expected, BitManipulation_getByte(field, bitmask, offset));
}
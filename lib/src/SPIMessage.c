#include "lib/include/SPIMessage.h"

static bool equalByteArrays(const uint8_t *left, const uint8_t *right, uint8_t lenght);

bool SPIMessage_equal(const SPIMessage *left, const SPIMessage *right) {
  if (left == right) {
    return true;
  }
  else {
    return ((left != NULL && right != NULL) &&
            (left->length == right->length) &&
            equalByteArrays(left->incoming_data, right->incoming_data, left->length) &&
            equalByteArrays(left->outgoing_data, right->outgoing_data, left->length) &&
            SPIMessage_equal(left->next, right->next));
  }
}

static bool equalByteArrays(const uint8_t *left, const uint8_t *right, uint8_t length) {
  if (left == right) return true;
  if ((left != NULL && right == NULL) || (left == NULL && right != NULL)) return false;
  for (uint8_t i = 0; i < length; i++ ) {
    if ( *left != *right ) return false;
    left++;
    right++;
  }
  return true;
}
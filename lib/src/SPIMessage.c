#include "lib/include/SPIMessage.h"

static bool equalByteArrays(const uint8_t *left, const uint8_t *right, uint8_t lenght);
static uint8_t calculateTotalLength(const SPIMessage *message);
static const uint8_t* getNextOutgoingDataByte(const SPIMessage *message, const uint8_t *current_byte);


bool SPIMessage_equal(const SPIMessage *left, const SPIMessage *right) {
  if (left == right) {
    return true;
  }
  else if (left != NULL && right != NULL) {
    if (left->length == right->length) {
            return (equalByteArrays(left->incoming_data, right->incoming_data, left->length) &&
            equalByteArrays(left->outgoing_data, right->outgoing_data, left->length) &&
            SPIMessage_equal(left->next, right->next));
    }
    else {
      uint8_t total_length_left = calculateTotalLength(left);
      uint8_t total_length_right = calculateTotalLength(right);
      if (total_length_left == total_length_right) {
        const uint8_t *left_byte = NULL;
        const uint8_t *right_byte = NULL;
        uint8_t iteration_number = 0;
        while (iteration_number < total_length_left) {
          left_byte = getNextOutgoingDataByte(left, left_byte);
          right_byte = getNextOutgoingDataByte(right, right_byte);
          if (*left_byte != *right_byte) return false;
          iteration_number++;
        }
        return true;
      }
    }
    return false;

  }
}

static uint8_t calculateTotalLength(const SPIMessage *message) {
  uint8_t length = message->length;
  while (message->next != NULL) {
    message = message->next;
    length += message->length;
  }
  return length;
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

static const uint8_t *getNextOutgoingDataByte(const SPIMessage *message, const uint8_t *current_byte) {
  if(current_byte == NULL) {
    return message->outgoing_data;
  }
  if (message->outgoing_data - current_byte < message->length) {
    return current_byte+1;
  }
  else {
    return message->next->outgoing_data;
  }
}
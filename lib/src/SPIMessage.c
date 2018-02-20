#include "lib/include/SPIMessage.h"

static uint8_t calculateTotalLength(const SPIMessage *message);

bool SPIMessage_equal(const SPIMessage *left, const SPIMessage *right) {
  uint8_t left_total_length = calculateTotalLength(left);
  uint8_t right_total_length = calculateTotalLength(right);
  if (left_total_length == right_total_length) {
    uint8_t left_index = 0;
    uint8_t right_index = 0;
    uint8_t iteration_counter = 0;
    while (iteration_counter < left_total_length) {
      if (left->outgoing_data[left_index] != right->outgoing_data[right_index]) {
        return false;
      }
      if (left_index < left->length-1) {
        left_index++;
      }
      else {
        left = left->next;
        left_index = 0;
      }
      if (right_index < right->length-1) {
        right_index++;
      }
      else {
        right = right->next;
        right_index = 0;
      }
      iteration_counter++;
    }
    return true;
  }
  return false;
}

static uint8_t calculateTotalLength(const SPIMessage *message) {
  uint8_t length = message->length;
  while (message->next != NULL) {
    message = message->next;
    length += message->length;
  }
  return length;
}


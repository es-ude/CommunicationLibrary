#include "lib/include/SPIMessage.h"
#include "src/unity.h"

void test_TwoMessagesWithEmptyStringAsOutgoingDataAreEqual(void) {
  uint8_t first_out[] = "";
  uint8_t second_out[] = "";
  SPIMessage first = {
          .next = NULL,
          .outgoing_data = first_out,
          .incoming_data = NULL,
          .length = 0,
  };
  SPIMessage second = {
          .next = NULL,
          .outgoing_data = second_out,
          .incoming_data = NULL,
          .length = 0,
  };
  TEST_ASSERT_TRUE(SPIMessage_equal(&first, &second));
}

void test_MessagesWithDifferentLengthAreNotEqual(void) {
  uint8_t first_out[0];
  SPIMessage first = {
          .next = NULL,
          .outgoing_data = first_out,
          .incoming_data = NULL,
          .length = 0,
  };
  SPIMessage second = {
          .next = NULL,
          .outgoing_data = first_out,
          .incoming_data = NULL,
          .length = 1,
  };
  TEST_ASSERT_FALSE(SPIMessage_equal(&first, &second));
}

void test_usingExactlyOneNullArgumentYieldsFalse(void) {
  SPIMessage *first = NULL;
  SPIMessage second = {
          .next = NULL,
          .outgoing_data = NULL,
          .incoming_data = NULL,
          .length = 1,
  };
  TEST_ASSERT_FALSE(SPIMessage_equal(first, &second));
  TEST_ASSERT_FALSE(SPIMessage_equal(&second, first));
}

void test_usingTheSameMessageTwiceYieldsTrue(void) {
  SPIMessage msg;
  SPIMessage *first = &msg;
  SPIMessage *second = &msg;
  TEST_ASSERT_TRUE(SPIMessage_equal(first, second));
}

void test_usingNULLForBothMessagesYieldsTrue(void) {
  SPIMessage *first = NULL;
  SPIMessage *second = NULL;
  TEST_ASSERT_TRUE(SPIMessage_equal(first, second));
}

void test_twoDifferentMessagesWithSameLength(void) {
  uint8_t data[1];
  SPIMessage first = {
          .length = 1,
          .incoming_data = NULL,
          .outgoing_data = data,
          .next = NULL,
  };
  SPIMessage second = {
          .length = 1,
          .incoming_data = data,
          .outgoing_data = NULL,
          .next = NULL,
  };
  TEST_ASSERT_FALSE(SPIMessage_equal(&first, &second));
}

void test_twoDifferentMessagesWithSameLength_Second(void) {
  uint8_t first_data[1];
  uint8_t second_data[1] = {first_data[0]+1};
  SPIMessage first = {
          .length = 1,
          .incoming_data = NULL,
          .outgoing_data = first_data,
          .next = NULL,
  };
  SPIMessage second = {
          .length = 1,
          .incoming_data = NULL,
          .outgoing_data = second_data,
          .next = NULL,
  };
  TEST_ASSERT_FALSE(SPIMessage_equal(&first, &second));
}

void test_twoDifferentMessagesWithSameLength_Third(void) {
  uint8_t first_data[6] = "hello";
  uint8_t second_data[6] = "hella";
  SPIMessage first = {
          .length = 6,
          .incoming_data = first_data,
          .outgoing_data = NULL,
          .next = NULL,
  };
  SPIMessage second = {
          .length = 6,
          .incoming_data = second_data,
          .outgoing_data = NULL,
          .next = NULL,
  };
  TEST_ASSERT_FALSE(SPIMessage_equal(&first, &second));
}

void test_twoEqualMessagesWithIncomingDataPointers_Second(void) {
  uint8_t first_data[6] = "hello";
  uint8_t second_data[6] = "hello";
  SPIMessage first = {
          .length = 6,
          .incoming_data = first_data,
          .outgoing_data = NULL,
          .next = NULL,
  };
  SPIMessage second = {
          .length = 6,
          .incoming_data = second_data,
          .outgoing_data = NULL,
          .next = NULL,
  };
  TEST_ASSERT_TRUE(SPIMessage_equal(&first, &second));
}

void test_twoDifferentMessageWithOnlyNextFieldDifferent(void) {
  uint8_t first_data[6] = "hello";
  SPIMessage first = {
          .length = 6,
          .outgoing_data = first_data,
          .incoming_data = NULL,
          .next = NULL,
  };
  SPIMessage second = first;
  second.next = &first;
  TEST_ASSERT_FALSE(SPIMessage_equal(&first, &second));

}


#include "test/Mocks/MockSPIImpl.h"
#include "unity.h"

void test_findTransferSequence(void) {
  SPIDeviceMockImpl mock;
  uint8_t buffer[128];
  mock.output_buffer = buffer;
  SPIMessage message_buffer[10];
  mock.message_buffer = message_buffer;
  SPIDeviceMockImpl_init(&mock);
  TEST_ASSERT_FALSE(SPIDeviceMockImpl_messageWasTransferred(&mock, NULL));

  uint8_t data[5] = "abcd";
  SPIMessage message = {
          .length = 4,
          .outgoing_data = data,
          .incoming_data = NULL
  };
  mock.device.transferSync(&mock, &message, 0);

  TEST_ASSERT_TRUE(SPIDeviceMockImpl_messageWasTransferred(&mock, &message));

  message.outgoing_data = "efgh";
  TEST_ASSERT_FALSE(SPIDeviceMockImpl_messageWasTransferred(&mock, &message));
}

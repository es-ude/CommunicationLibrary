#include "lib/CommunicationModule.h"
#include "test/Mocks/RuntimeLibraryMock.h"
#include "unity.h"

void test_sendBlockingMessageUseCase(void) {
  uint8_t memory_for_communication_module[sizeof(CommunicationModule)];
  MockAllocateConfig config = {
    .size_allocated = 0,
    .returned_address = memory_for_communication_module,
  };

  configureMockAllocate(&config);

  CommunicationModule *communication_module = CommunicationModule_create(mockAllocate);

  TEST_ASSERT_EQUAL_UINT16(config.size_allocated, sizeof(CommunicationModule));
  TEST_ASSERT_EQUAL_PTR(memory_for_communication_module, communication_module);
  Message message;
  message.payload = (uint8_t*) "hello there";
  message.address[0] = 0;
  message.address[1] = 1;
  communication_module->sendNonBlocking(&message);
}

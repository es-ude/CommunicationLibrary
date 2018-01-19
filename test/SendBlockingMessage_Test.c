#include "lib/CommunicationModule.h"
#include "test/Mocks/RuntimeLibraryMock.h"
#include "unity.h"

void test_sendBlockingMessageUseCase(void) {
  size_t observed_allocation_size = 0;
  void *expected_allocated_address = (void*) 1;

  MockAllocateConfig config = {
    .size_allocated = 0,
    .returned_address = (void*) 1
  };

  configureMockAllocate(&config);
  setMockAllocateInputValue(&observed_allocation_size);
  setMockAllocateReturnValue(expected_allocated_address);

  CommunicationModule *communication_module = CommunicationModule_create(mockAllocate);

  TEST_ASSERT_GREATER_THAN_UINT16(0, observed_allocation_size);
  TEST_ASSERT_NOT_NULL(communication_module);
  /* Message message; */
  /* message.payload = "hello there"; */
  /* message.address = {0, 1}; */
  /* communication_module->send(message); */
}

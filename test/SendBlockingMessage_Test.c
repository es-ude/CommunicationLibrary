#include "lib/include/CommunicationModule.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "unity.h"

uint8_t memory_for_communication_module[sizeof(CommunicationModule)];
MockAllocateConfig config;

void setUp(void) {
  config.size_allocated = 0;
  config.returned_address = memory_for_communication_module;
  MockAllocate_configure(&config);

}

void test_CommunicationModuleAllocationIsCorrect(void) {
  CommunicationModule *communication_module = CommunicationModule_create(MockAllocate_allocate);

  TEST_ASSERT_EQUAL_UINT16(config.size_allocated, sizeof(CommunicationModule));
  TEST_ASSERT_EQUAL_PTR(memory_for_communication_module, communication_module);
}

void test_sendNonBlockingMessageCanBeCalled(void) {
  CommunicationModule *communication_module = CommunicationModule_create(MockAllocate_allocate);
  Message message;
  message.payload = (uint8_t*) "hello there";
  message.address[0] = 0;
  message.address[1] = 1;
  communication_module->sendNonBlocking(&message);
}

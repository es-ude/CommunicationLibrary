#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "src/unity.h"

MemoryManagement *mock;

void setUp(void) {
  mock = MemoryManagement_createMockImpl();
}

void test_AllocateNothing(void) {
  TEST_ASSERT_EQUAL_UINT8(0, MockMemoryManagement_numberOfAllocatedObjects(mock));
  void *some_pointer;
  TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_size(mock, some_pointer));
}

void test_AllocateOneObject(void) {
  uint64_t *some_pointer = mock->allocate(sizeof(uint64_t));
  *some_pointer = 34;
  TEST_ASSERT_EQUAL_UINT8(1, MockMemoryManagement_numberOfAllocatedObjects(mock));
  TEST_ASSERT_EQUAL_UINT8(34, *some_pointer);
  TEST_ASSERT_EQUAL_UINT8(8, MockMemoryManagement_size(mock, some_pointer));
  mock->deallocate(some_pointer);
  TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(mock));
}

void test_AllocateTwoObjects(void) {
  uint64_t *some_pointer = mock->allocate(sizeof(*some_pointer));
  *some_pointer = 23;
  uint32_t *some_other_pointer = mock->allocate(sizeof(*some_other_pointer));
  *some_other_pointer = 54;
  TEST_ASSERT_EQUAL_UINT32(54, *some_other_pointer);
  TEST_ASSERT_EQUAL_UINT(4, MockMemoryManagement_size(mock, some_other_pointer));
  TEST_ASSERT_EQUAL_UINT64(23, *some_pointer);
  TEST_ASSERT_EQUAL_UINT(2, MockMemoryManagement_numberOfAllocatedObjects(mock));
  mock->deallocate(some_other_pointer);
  mock->deallocate(some_pointer);
  TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_numberOfAllocatedObjects(mock));
}

void test_deallocate(void) {
  uint64_t *pointer = mock->allocate(sizeof(*pointer));
  mock->deallocate(pointer);
  TEST_ASSERT_EQUAL_UINT(0, MockMemoryManagement_size(mock, pointer));
}
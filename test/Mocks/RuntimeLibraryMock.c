#include "RuntimeLibraryMock.h"
#include <stddef.h>

size_t *mock_allocate_input_value;

void *returned_pointer;

void * mockAllocate(size_t bytes_allocated) {
  *mock_allocate_input_value = 1;
  return (void*)1;
}

void setMockAllocateInputValue(size_t *memory) {
  mock_allocate_input_value = memory;
}

void setMockAllocateReturnValue(void *pointer) {
}

void configureMockAllocate(MockAllocateConfig* config){}

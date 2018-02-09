#include "test/Mocks/MockRuntimeLibraryImpl.h"


size_t *input_value;

void *returned_pointer;

void * MockAllocate_allocate(size_t bytes_allocated) {
  *input_value = bytes_allocated;
  return returned_pointer;
}

void MockAllocate_configure(MockAllocateConfig *config){
  input_value = &config->size_allocated;
  returned_pointer = config->returned_address;
}


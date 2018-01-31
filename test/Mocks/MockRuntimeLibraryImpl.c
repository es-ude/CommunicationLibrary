#include "MockRuntimeLibraryImpl.h"

size_t *input_value;

void *returned_pointer;

void * mockAllocate(size_t bytes_allocated) {
  *input_value = bytes_allocated;
  return returned_pointer;
}

void configureMockAllocate(MockAllocateConfig* config){
  input_value = &config->size_allocated;
  returned_pointer = config->returned_address;
}

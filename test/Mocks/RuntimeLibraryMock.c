#include "RuntimeLibraryMock.h"
#include <stddef.h>

size_t *input_value;

void *returned_address;

void * mockAllocate(size_t bytes_allocated) {
  *input_value = bytes_allocated;
  return returned_address;
}

void configureMockAllocate(MockAllocateConfig* config){
  input_value = &config->size_allocated;
  returned_address = config->returned_address;
}

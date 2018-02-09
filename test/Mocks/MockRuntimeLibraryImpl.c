#include "test/Mocks/MockRuntimeLibraryImpl.h"


size_t *input_value;

void *returned_pointer;

double last_delay_microseconds_arg = 0;

void * MockAllocate_allocate(size_t bytes_allocated) {
  *input_value = bytes_allocated;
  return returned_pointer;
}

void MockAllocate_configure(MockAllocateConfig *config){
  input_value = &config->size_allocated;
  returned_pointer = config->returned_address;
}

void MockRuntime_delayMicroseconds(double microseconds) {
  last_delay_microseconds_arg = microseconds;
}

double MockRuntime_lastDelayMicrosecondsArg(void) {
  return last_delay_microseconds_arg;
}

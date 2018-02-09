#ifndef RUNTIMELIBRARYMOCK_H
#define RUNTIMELIBRARYMOCK_H


#include <stdint.h>
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct MockAllocateConfig {
  size_t size_allocated;
  void *returned_address;
} MockAllocateConfig;

void * MockAllocate_allocate(size_t bytes_allocated);

void MockAllocate_configure(MockAllocateConfig *config);

void MockRuntime_delayMicroseconds(double microseconds);

double MockRuntime_lastDelayMicrosecondsArg(void);

#endif /* RUNTIMELIBRARYMOCK_H */

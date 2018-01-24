#ifndef RUNTIMELIBRARYMOCK_H
#define RUNTIMELIBRARYMOCK_H


#include "lib/RuntimeLibraryInterface.h"

typedef struct MockAllocateConfig {
  size_t size_allocated;
  void *returned_address;
} MockAllocateConfig;

void * mockAllocate(size_t bytes_allocated);

void configureMockAllocate(MockAllocateConfig*);


#endif /* RUNTIMELIBRARYMOCK_H */

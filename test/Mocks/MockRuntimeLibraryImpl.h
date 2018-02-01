#ifndef RUNTIMELIBRARYMOCK_H
#define RUNTIMELIBRARYMOCK_H


#include "lib/include/RuntimeLibraryInterface.h"

typedef struct MockAllocateConfig {
  size_t size_allocated;
  void *returned_address;
} MockAllocateConfig;

void * MockAllocate_allocate(size_t bytes_allocated);

void MockAllocate_configure(MockAllocateConfig *config);


#endif /* RUNTIMELIBRARYMOCK_H */

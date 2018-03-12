#ifndef RUNTIMELIBRARYMOCK_H
#define RUNTIMELIBRARYMOCK_H


#include <stdint.h>
#include <stdbool.h>
#include "lib/include/RuntimeLibraryInterface.h"

typedef struct MockAllocateConfig {
    size_t size_allocated;
    void *returned_address;
} MockAllocateConfig;

void * MockAllocate_allocate(size_t bytes_allocated);

void MockAllocate_configure(MockAllocateConfig *config);

void MockRuntime_delayMicroseconds(double microseconds);

double MockRuntime_lastDelayMicrosecondsArg(void);

MemoryManagement *MemoryManagement_createMockImpl(void);

uint16_t MockMemoryManagement_numberOfAllocatedObjects(MemoryManagement *self);
size_t MockMemoryManagement_size(MemoryManagement *self, void *pointer);

#endif /* RUNTIMELIBRARYMOCK_H */
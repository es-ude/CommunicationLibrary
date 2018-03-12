#ifndef RUNTIMELIBRARYINTERFACE_H
#define RUNTIMELIBRARYINTERFACE_H
#include <stddef.h>


typedef void * (*Allocator) (size_t bytes_allocated);
typedef void (*Deallocator) (void *memory);

typedef struct MemoryManagement {
    Allocator allocate;
    Deallocator deallocate;
} MemoryManagement;

typedef void (*DelayFunction) (double amount);

#endif /* RUNTIMELIBRARYINTERFACE_H */
#ifndef RUNTIMELIBRARYINTERFACE_H
#define RUNTIMELIBRARYINTERFACE_H
#include <stdint.h>
#include <stddef.h>

typedef void * (*AllocationFunction) (size_t bytes_allocated);
typedef void * (*Allocator) (size_t number_of_bytes);

#endif /* RUNTIMELIBRARYINTERFACE_H */

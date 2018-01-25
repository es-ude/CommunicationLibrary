#ifndef RUNTIMELIBRARYINTERFACE_H
#define RUNTIMELIBRARYINTERFACE_H
#include <stdint.h>
#include <stddef.h>

typedef void * (*Allocator) (size_t bytes_allocated);

#endif /* RUNTIMELIBRARYINTERFACE_H */

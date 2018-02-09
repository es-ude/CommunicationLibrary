#ifndef RUNTIMELIBRARYINTERFACE_H
#define RUNTIMELIBRARYINTERFACE_H
#include <stddef.h>

typedef void * (*Allocator) (size_t bytes_allocated);
typedef void (*DelayFunction) (double amount);

#endif /* RUNTIMELIBRARYINTERFACE_H */

#include "Mutex.h"
#include <stddef.h>

void
Mutex_init(Mutex *self)
{
  self->key = NULL;
}

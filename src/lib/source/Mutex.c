#include "Mutex.h"
#include <stddef.h>

void
Mutex_init(Mutex *self)
{
  self->key = NULL;
}

inline bool
Mutex_isLocked(Mutex *self)
{
  return (self->key != NULL);
}

bool
Mutex_lockWithKey(Mutex *self, void *key)
{
  if (self->key != NULL)
    {
      return false;
    }
  else
    {
      self->key = key;
      return true;
    }
}

bool
Mutex_unlockWithKey(Mutex *self, void *key)
{
  if (self->key == key)
    {
      self->key = NULL;
      return true;
    }
  return false;
}

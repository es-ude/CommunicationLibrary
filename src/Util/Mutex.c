#include "Mutex.h"
#include <util/atomic.h>
#include "CException.h"
#include <stdbool.h>
#include <stddef.h>

void
lockMutex(Mutex *self, void *lock)
{
  bool locked = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    if (self->lock == NULL)
    {
      self->lock = lock;
      locked = true;
    }
  }
  if (!locked)
  {
    Throw(MUTEX_ALREADY_LOCKED_EXCEPTION);
  }
}

void
unlockMutex(Mutex *self, void *lock)
{
  bool locked = true;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    if (self->lock == lock)
    {
      self->lock = NULL;
      locked = false;
    }
  }
  if (locked)
  {
    Throw(WRONG_MUTEX_LOCK_EXCEPTION);
  }
}
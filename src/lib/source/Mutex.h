#ifndef MUTEX_H
#define MUTEX_H

#include <stdbool.h>

typedef struct Mutex
{
  void *key;
} Mutex;

void
Mutex_init(Mutex *self);

bool
Mutex_isLocked(Mutex *self);

bool
Mutex_lockWithKey(Mutex *self, void *key);

bool
Mutex_unlockWithKey(Mutex *self, void *key);

#endif /* MUTEX_H */

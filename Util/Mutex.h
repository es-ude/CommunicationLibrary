#ifndef COMMUNICATIONMODULE_MUTEX_H
#define COMMUNICATIONMODULE_MUTEX_H

#include <stdint.h>

typedef struct Mutex Mutex;

struct Mutex
{
  void *lock;
};

static const uint8_t MUTEX_WAS_NOT_LOCKED = 0x01;
static const uint8_t MUTEX_WAS_NOT_UNLOCKED = 0x02;

void
lockMutex(Mutex *self, void *lock);

void
unlockMutex(Mutex *self, void *lock);

void
initMutex(Mutex *self);

#endif //COMMUNICATIONMODULE_MUTEX_H

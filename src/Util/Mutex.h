#ifndef COMMUNICATIONMODULE_MUTEX_H
#define COMMUNICATIONMODULE_MUTEX_H

#include <stdint.h>

typedef struct Mutex Mutex;

struct Mutex
{
  void *lock;
};

static const uint8_t MUTEX_ALREADY_LOCKED_EXCEPTION = 0x01;
static const uint8_t WRONG_MUTEX_LOCK_EXCEPTION = 0x02;

void
lockMutex(Mutex *self, void *lock);

void
unlockMutex(Mutex *self, void *lock);

#endif //COMMUNICATIONMODULE_MUTEX_H

#include "Util/Mutex.h"
#include "Util/Atomic.h"
#include "CException.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct CallbackArgs {
    bool success;
    void *lock;
    Mutex *mutex;
} CallbackArgs;

static void
doLockMutex(void *args)
{
    CallbackArgs *callback_args = (CallbackArgs*) args;
    Mutex *self = callback_args->mutex;
    if (self->lock == NULL)
    {
      self->lock = callback_args->lock;
      callback_args->success = true;
    }

}

static void
doUnlockMutex(void *args)
{
    CallbackArgs *callback_args = (CallbackArgs*)args;
    Mutex *self = callback_args->mutex;
    if (self->lock == callback_args->lock)
    {
      self->lock = NULL;
      callback_args->success = true;
    }

}

void
unlockMutex(Mutex *self, void *lock)
{
    CallbackArgs args = {
            .lock = lock,
            .success = false,
            .mutex = self
    };
    executeAtomically((GenericCallback){
        .argument=&args,
        .function=doUnlockMutex
    });
    if (!args.success)
    {
        Throw(MUTEX_WAS_NOT_UNLOCKED);
    }
}

void
lockMutex(Mutex *self, void *lock)
{
    CallbackArgs args = {
            .lock = lock,
            .success = false,
            .mutex = self
    };
    executeAtomically((GenericCallback){
        .function=doLockMutex,
        .argument=&args
    });
    if (!args.success)
    {
        Throw(MUTEX_WAS_NOT_LOCKED);
    }
}

void
initMutex(Mutex *self)
{
    self->lock = NULL;
}
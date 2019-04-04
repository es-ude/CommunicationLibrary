#include "Util/Atomic.h"
#include "Util/Mutex.h"
#include <CException.h>
#include <stdbool.h>
#include <unity.h>

static bool execute_callback = true;
static Mutex mutex;

void
setUp(void)
{
  execute_callback = true;
  initMutex(&mutex);
}

void
executeAtomically(GenericCallback callback)
{
  if (execute_callback)
    callback.function(callback.argument);
}

void
test_initMutex(void)
{
  Mutex mutex = {.lock = (void*) 1};
  initMutex(&mutex);
  TEST_ASSERT_NULL(mutex.lock);
}

void
test_lockSuccess(void)
{
  lockMutex(&mutex, (void*)1);
}

void
test_lockTwiceThrowsException(void)
{
  CEXCEPTION_T e = CEXCEPTION_NONE;
  lockMutex(&mutex, (void *)2);
  Try
  {
    lockMutex(&mutex, (void *)2);
    TEST_FAIL_MESSAGE("Exception not thrown");
  }
  Catch (e)
  {
    TEST_ASSERT_EQUAL(MUTEX_WAS_NOT_LOCKED, e);
  }
}

void
test_makeSureLockingIsPerformedInsideOfAtomicBlock(void)
{
  execute_callback = false;
  CEXCEPTION_T exception = CEXCEPTION_NONE;
  Try
  {
    lockMutex(&mutex, (void *) 2);
  }
  Catch(exception)
  {
    TEST_ASSERT_EQUAL_UINT8(MUTEX_WAS_NOT_LOCKED, exception);
  }
}

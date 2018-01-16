#include <unity.h>
#include "lib/Mutex.h"
#include <stddef.h>

void
test_creating_a_mutex(void)
{
  Mutex m;
  // set key to arbitrary value simulating mutex not initialized automatically
  int some_memory;
  m.key = &some_memory;
  Mutex_init(&m);
  TEST_ASSERT_EQUAL_PTR(NULL, m.key);
}

void
test_isLocked_returns_false_for_unlocked_mutex(void)
{
  Mutex m;
  Mutex_init(&m);
  TEST_ASSERT_FALSE(Mutex_isLocked(&m));
}

void
test_locking_unlocked_mutex_returns_true(void)
{
  Mutex m;
  Mutex_init(&m);
  int key;
  TEST_ASSERT_TRUE(Mutex_lockWithKey(&m, &key));
}

void
test_locking_an_already_locked_mutex_returns_false(void)
{
  Mutex m;
  Mutex_init(&m);
  int key;
  Mutex_lockWithKey(&m, &key);
  TEST_ASSERT_FALSE(Mutex_lockWithKey(&m, &key));
}

void
test_isLocked_returns_true_on_locked_mutex(void)
{
  Mutex m;
  Mutex_init(&m);
  int key;
  Mutex_lockWithKey(&m, &key);
  TEST_ASSERT_TRUE(Mutex_isLocked(&m));
}

void
test_unlocking_a_mutex_with_wrong_key_returns_false(void)
{
  Mutex m;
  Mutex_init(&m);
  int right_key;
  int wrong_key;
  Mutex_lockWithKey(&m, &right_key);
  TEST_ASSERT_FALSE(Mutex_unlockWithKey(&m, &wrong_key));
}

void
test_unlocking_mutex_with_right_key_returns_true(void)
{
  Mutex m;
  Mutex_init(&m);
  int key;
  Mutex_lockWithKey(&m, &key);
  TEST_ASSERT_TRUE(Mutex_unlockWithKey(&m, &key));
}

void
test_mutex_is_unlocked_after_calling_unlock_with_correct_key(void)
{
  Mutex m;
  Mutex_init(&m);
  int key;
  Mutex_lockWithKey(&m, &key);
  Mutex_unlockWithKey(&m, &key);
  TEST_ASSERT_FALSE(Mutex_isLocked(&m));
}

void
test_mutex_is_locked_after_calling_unlock_with_wrong_key(void)
{
  Mutex m;
  int correct_key;
  int wrong_key;
  Mutex_init(&m);
  Mutex_lockWithKey(&m, &correct_key);
  Mutex_unlockWithKey(&m, &wrong_key);
  TEST_ASSERT_TRUE(Mutex_isLocked(&m));
}

int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_creating_a_mutex);
  RUN_TEST(test_isLocked_returns_false_for_unlocked_mutex);
  RUN_TEST(test_locking_unlocked_mutex_returns_true);
  RUN_TEST(test_locking_an_already_locked_mutex_returns_false);
  RUN_TEST(test_isLocked_returns_true_on_locked_mutex);
  RUN_TEST(test_unlocking_a_mutex_with_wrong_key_returns_false);
  RUN_TEST(test_unlocking_mutex_with_right_key_returns_true);
  RUN_TEST(test_mutex_is_unlocked_after_calling_unlock_with_correct_key);
  return UNITY_END();
}

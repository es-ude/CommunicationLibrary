#include <unity.h>
#include "../lib/source/Mutex.h"
#include <stddef.h>

void
test_creating_a_mutex(void)
{
  Mutex m;
  // set key to some value simulating mutex not initialized automatically
  int some_memory;
  m.key = &some_memory;
  Mutex_init(&m);
  TEST_ASSERT_EQUAL_PTR(NULL, m.key);
}



int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_creating_a_mutex);
  return UNITY_END();
}

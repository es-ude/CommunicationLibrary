#include "test/Mocks/MockPeripheralImpl.h"
#include "lib/include/PeripheralAPI.h"
#include "unity.h"

void test_writeOneByte(void) {
  MockPeripheralImpl implementation;
  Peripheral* interface = (Peripheral*) &implementation;
  MockPeripheralImpl_init(interface);
  Peripheral_writeByte(interface, 0xA2);
  TEST_ASSERT_EQUAL_UINT8(0xA2, implementation.written_bytes[0]);
}


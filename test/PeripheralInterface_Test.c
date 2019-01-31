#include "Peripheral/PeripheralInterface.h"
#include "unity.h"
#include "src/Util/MockMutex.h"

PeripheralInterface interface;
PeripheralInterface *interface_ptr = &interface;

struct DummyInterfaceImpl {
  uint8_t counter;
  uint8_t buffer[256];
};


void
writeByteBlockingDummy(PeripheralInterface *self,
                       uint8_t byte){

}


void
test_something(void)
{

}
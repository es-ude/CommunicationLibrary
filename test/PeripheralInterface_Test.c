#include "Peripheral/PeripheralInterface.h"
#include "unity.h"
#include "Util/Atomic.h"
#include "Util/MockMutex.h"
#include "CException.h"

enum {
    BUFFER_SIZE = 255
};

struct DummyInterfaceImpl {
    PeripheralInterface functions;
    uint8_t counter;
    uint8_t buffer[BUFFER_SIZE];
};

typedef struct DummyInterfaceImpl DummyInterfaceImpl;

static DummyInterfaceImpl interface;
static PeripheralInterface *interface_ptr;
static bool is_atomic = false;
static bool peripheral_de_selected = false;


static void
writeByteBlocking(PeripheralInterface *self,
                  uint8_t byte);

static void
selectPeripheral(PeripheralInterface *self,
                 Peripheral *device);

void
setUp(void)
{
  interface.counter = 0;
  for (uint8_t i = 0; i < BUFFER_SIZE; i++)
  {
    interface.buffer[i] = 0;
  }
  peripheral_de_selected = false;
  interface_ptr = (PeripheralInterface *) &interface;
  interface.functions.writeByteBlocking = writeByteBlocking;
  interface.functions.selectPeripheral = selectPeripheral;
  interface.functions.deselectPeripheral = selectPeripheral;
  is_atomic = 0;
  interface.functions.mutex.lock = NULL;
}

void
writeByteBlocking(PeripheralInterface *self,
                  uint8_t byte)
{
  interface.buffer[interface.counter] = byte;
  interface.counter++;
}

void
selectPeripheral(PeripheralInterface *self,
                 Peripheral *device)
{
  peripheral_de_selected = true;
}

void
tearDown(void)
{

}

void
test_writeZeroBytes(void)
{
  PeripheralInterface_writeBlocking(interface_ptr, (uint8_t *)"", 0);
}

void
test_writeOneByte(void)
{
  PeripheralInterface_writeBlocking(interface_ptr, (uint8_t *)"a", 1);
  TEST_ASSERT_EQUAL_UINT8('a', interface.buffer[0]);
}

void
test_writeString(void)
{
  PeripheralInterface_writeBlocking(interface_ptr, (uint8_t*)"hello", 6);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t*)"hello", interface.buffer, 6);
}

void
test_selectPeripheral(void)
{
  lockMutex_Expect(&interface.functions.mutex, NULL);
  PeripheralInterface_selectPeripheral(interface_ptr, NULL);

  TEST_ASSERT_TRUE(peripheral_de_selected);
}

void
test_selectPeripheralButUnableToLockMutex(void)
{
  lockMutex_ExpectAndThrow(&interface.functions.mutex, NULL, MUTEX_WAS_NOT_LOCKED);
  CEXCEPTION_T exception = PERIPHERALINTERFACE_NO_EXCEPTION;
  Try
  {
    PeripheralInterface_selectPeripheral(interface_ptr, NULL);
    TEST_FAIL_MESSAGE("Exception not thrown");
  } Catch (exception)
  {
    TEST_ASSERT_EQUAL(PERIPHERALINTERFACE_BUSY_EXCEPTION, exception);
  }
  TEST_ASSERT_FALSE(peripheral_de_selected);
}

void
test_selectPeripheralWithDeviceOne(void)
{
  Peripheral *device = (Peripheral *) 1;
  lockMutex_Expect(&interface.functions.mutex, device);
  PeripheralInterface_selectPeripheral(interface_ptr, device);
}

void
test_deselectPeripheral(void)
{
  lockMutex_Expect(&interface.functions.mutex, NULL);
  PeripheralInterface_deselectPeripheral(interface_ptr, NULL);
  TEST_ASSERT_TRUE(peripheral_de_selected);
}

void
test_deselectPeripheralButUnableToLockMutex(void)
{
  lockMutex_ExpectAndThrow(&interface.functions.mutex, NULL, MUTEX_WAS_NOT_LOCKED);
  CEXCEPTION_T exception = PERIPHERALINTERFACE_NO_EXCEPTION;
  Try
  {
    PeripheralInterface_deselectPeripheral(interface_ptr, NULL);
    TEST_FAIL_MESSAGE("Exception not thrown");
  }
  Catch (exception)
  {
    TEST_ASSERT_EQUAL(PERIPHERALINTERFACE_BUSY_EXCEPTION, exception);
  }
  TEST_ASSERT_FALSE(peripheral_de_selected);
}

void
test_deselectPeripheralDeviceOne(void)
{
  Peripheral *device = (Peripheral *) 1;
  lockMutex_Expect(&interface.functions.mutex, device);
  PeripheralInterface_deselectPeripheral(interface_ptr, device);
}
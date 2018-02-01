#include "test/Mocks/MockPeripheralImpl.h"
#include "lib/include/NetworkHardwareMRFImpl.h"
#include "test/Mocks/MockRuntimeLibraryImpl.h"
#include "src/unity.h"

void test_HardwareModuleInterfaceSetup(void) {
  MockPeripheralImpl peripheral;
  MockPeripheralImpl_init((Peripheral*) &peripheral);
  NetworkHardware *hardware = NetworkHardware_createMRF((Peripheral *) &peripheral, MockAllocate_allocate);
}

void test_setAddress16Bit(void) {
  MockPeripheralImpl peripheral;
  MockPeripheralImpl_init((Peripheral*) &peripheral);
  NetworkHardware *hardware = NetworkHardware_createMRF((Peripheral *) &peripheral, MockAllocate_allocate);

  uint16_t address16bit = 0xFFFF;


}



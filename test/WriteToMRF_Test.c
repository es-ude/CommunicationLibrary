#include "test/Mocks/PeripheralInterfaceMock.h"
#include "lib/include/NetworkHardwareMRFImpl.h"

void test_HardwareModuleInterfaceSetup(void) {
    PeripheralInterfaceMock peripheral;
    initPeripheralInterfaceMock((Peripheral*) &peripheral);
    NetworkHardware *hardware = NetworkHardware_createMRF((Peripheral*) &peripheral);

}

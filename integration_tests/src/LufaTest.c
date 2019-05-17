#include <util/delay.h>
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "Util/Debug.h"
#include "integration_tests/src/Setup/DebugSetup.h"
#include "integration_tests/src/Setup/HardwareSetup.h"


int
main(void){
  setUpPrint();
  while (1)
  {
    _delay_ms(1000);
    printString("Test\n");
    _delay_ms(500);
    printString("again\n");
  }
}
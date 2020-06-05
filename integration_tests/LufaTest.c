#include <util/delay.h>

#include "EmbeddedUtilities/Debug.h"
#include "Setup/DebugSetup.h"
#include "Setup/HardwareSetup.h"


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
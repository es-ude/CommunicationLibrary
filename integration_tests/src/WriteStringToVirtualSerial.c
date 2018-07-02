#include "integration_tests/LUFA-Setup/Helpers.h"
#include <util/delay.h>

int main(void) {
  setUpUsbSerial();

  int a = 0;
  char hello[] = "Ahello world\n";

  for (;;)
  {

    if (a ==0){
      hello[0]++;
      usbWriteString(hello);
      if (hello[0] == 'Z'){
        hello[0] = 'A'-1;
      }

    }
    /* Must throw away unused bytes from the host, or it will lock up while waiting for the device */

    periodicUsbTask();
  }
}
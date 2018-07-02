#include "app/LUFA-Setup/Helpers.h"
#include <util/delay.h>

int main(void) {
  while(true) {
    _delay_ms(2000);
    usbWriteString("hello, world!");
  }
}
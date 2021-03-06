#include "EmbeddedUtilities/Debug.h"
#include "Setup/LUFAHelpers.h"
#include <stdio.h>


void
setUpPrint(void)
{
  setUpUsbSerial();
}

void
printString(const char *string)
{
  usbWriteString((const uint8_t*)string);
}

void
printUInt16(uint16_t number)
{
  char text[10];
  sprintf(text, "%i", number);
  printString(text);
}
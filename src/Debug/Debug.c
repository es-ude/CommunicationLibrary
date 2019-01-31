#include "Debug/Debug.h"
#include "Peripheral/PeripheralInterface.h"
#include "CException.h"
#include <stdio.h>

PeripheralInterface *peripheral_interface_for_debugging = NULL;
Peripheral *terminal = NULL;

void
initDebug(PeripheralInterface *interface_for_debugging,
          Peripheral *device_for_debugging)
{
    terminal = device_for_debugging;
    peripheral_interface_for_debugging = interface_for_debugging;
}

void
printString(const char *message)
{
  uint16_t size = 0;
  while (message[size] != '\0')
    {
      size++;
    }
  PeripheralInterface_selectPeripheral(peripheral_interface_for_debugging,
                                       terminal);
  PeripheralInterface_writeBlocking(
    peripheral_interface_for_debugging, (const uint8_t *)message, size);
  PeripheralInterface_deselectPeripheral(peripheral_interface_for_debugging,
                                         terminal);
}

void
printSizedString(const char *message, uint16_t size)
{
  PeripheralInterface_selectPeripheral(peripheral_interface_for_debugging,
      terminal);
  PeripheralInterface_writeBlocking(
      peripheral_interface_for_debugging,
      (const uint8_t *)message,
      size
      );
  PeripheralInterface_deselectPeripheral(peripheral_interface_for_debugging,
      terminal);
}

void
printUInt16(uint16_t integer)
{
  char message[6];
  sprintf(message, "%u", integer);
  printString(message);
}

void
printHex16(uint16_t integer)
{
  char message[10];
  sprintf(message, "0x%x", integer);
  printString(message);
}

void
printDec16(uint16_t integer)
{
  printUInt16(integer);
}

void
printHex32(uint32_t integer)
{
  char message[11];
  sprintf(message, "0x%08lX", integer);
  printString(message);
}

void
printDec32(uint32_t number)
{
  char message[10];
  sprintf(message, "%lu", number);
  printString(message);
}

void
printDec32Signed(int32_t number)
{
  char message[10];
  sprintf(message, "%li", number);
  printString(message);
}

void
printChar(char symbol)
{
  PeripheralInterface_selectPeripheral(peripheral_interface_for_debugging, terminal);
  PeripheralInterface_writeBlocking(peripheral_interface_for_debugging, (uint8_t *)&symbol, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface_for_debugging, terminal);
}

void
printHex8(uint8_t number)
{
  char message[5];
  sprintf(message, "0G%02X", number);
  printString(message);
}

void
printDec8(uint8_t number)
{
  char message[4];
  sprintf(message, "%1i", number);
  printString(message);
}

void
printBin8(uint8_t number)
{
  printString("0b");
  for (uint8_t index = 0; index < 8; index++)
  {
    if ((number & (1 << index)) != 0)
    {
      printString("1");
    }
    else
    {
      printString("0");
    }
  }
}

void
printNewLine(void)
{
  printChar('\n');
}

void
printLine(const char *message)
{
  printString(message);
  printNewLine();
}

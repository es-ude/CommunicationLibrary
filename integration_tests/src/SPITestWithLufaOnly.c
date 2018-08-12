#include <stdlib.h>
#include <util/delay.h>
#include "integration_tests/LUFA-Setup/Helpers.h"

void setup(void);
uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void writeByteToShortRegisterAddress(uint8_t register_address, uint8_t byte);
void convertByteToString(uint8_t byte, uint8_t *string);
void debug(const uint8_t *string);

void select(void) {
  PORTB &= ~(_BV(PORTB4));
}

void deselect(void) {
  PORTB |= _BV(PORTB4);
}

uint8_t transfer(uint8_t byte) {
  SPDR = byte;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}

void write(uint8_t byte) {
  transfer(byte);
}

uint8_t read(void) {
  return transfer(0);
}

int main(void){
  setup();
  uint8_t output[] = "0x00 was read, and 0x75 was expected\n";
  uint8_t byte = 0xAB;
  usbWriteString((uint8_t*)"Start\n");
  periodicUsbTask();
  writeByteToShortRegisterAddress(0x2F, 0x55);
  for(;;) {
    byte = readByteFromShortAddressRegister(0x2E);
    convertByteToString(byte, output);
    usbWriteString(output);
    _delay_ms(1000);
    periodicUsbTask();
  }
}



void setup(void) {
  setUpUsbSerial();
  _delay_ms(3000);
  DDRB = (_BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB4));
  PORTB |= _BV(PORTB0) | _BV(PORTB4);
  SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR1));
  DDRC = _BV(6);
}

void writeByteToShortRegisterAddress(uint8_t register_address, uint8_t byte) {
  uint8_t command = register_address << 1 | 1;
  select();
  transfer(command);
  transfer(byte);
  deselect();
}

uint8_t readByteFromShortAddressRegister(uint8_t register_address) {
  uint8_t bit_mask = 0b01111110;
  uint8_t command = register_address << 1;
  debugPrintHex(command);
  command &= bit_mask;
  select();
  write(command);
  uint8_t result = read();
  deselect();
  return result;
}

uint8_t convertNumberToASCII(uint8_t number) {
  if ( number >= 0 && number <= 9) {
    return (uint8_t )('0' + number);
  }
  else {
    return (uint8_t )('A' + (number - 10));
  }
}

void convertByteToString(uint8_t byte, uint8_t *string) {
  uint8_t upper_half = byte >> 4;
  uint8_t lower_half = (uint8_t) (byte & 0x0F);
  string[2] = convertNumberToASCII(upper_half);
  string[3] = convertNumberToASCII(lower_half);
}

void debug(const uint8_t *string) {
  usbWriteString(string);
  periodicUsbTask();
}

void debugPrintHex(uint8_t byte) {
  uint8_t string[] = "0x00\n";
  convertByteToString(byte, string);
  debug(string);
}
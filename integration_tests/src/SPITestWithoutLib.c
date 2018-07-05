#include <stdlib.h>
#include <util/delay.h>
#include "integration_tests/LUFA-Setup/Helpers.h"

uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);

uint8_t transfer(uint8_t byte) {
  SPDR = byte;
//  while (!(SPSR & (1 << SPIF)))
//    ;
  return SPDR;
}

void write(uint8_t byte) {
  transfer(byte);
  usbWriteString("byte written\n");
}

void writeString(uint8_t *string, uint8_t size) {
  for(;size > 0; size--){
    write(*string++);
  }
}

uint8_t read(void) {
  return transfer(0);
}

void readString(uint8_t *string, uint8_t size) {
  for(;size > 0; size--) {
    *string++ = read();
  }
}

void select(void) {
  PORTB &= ~(_BV(PORTB0));
}

void deselect(void) {
  PORTB |= _BV(PORTB0);
}

int main(void) {
  setUpUsbSerial();
  SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR1));
  DDRB = (_BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2));
  DDRC = _BV(6);
  PORTB |= _BV(PORTB0);
  _delay_ms(2000);
  uint8_t output[] = "0x00\n";
  uint8_t byte = 0xAB;
  usbWriteString((uint8_t*)"Start\n");
  periodicUsbTask();
  for(;;) {
//    byte = readByteFromShortAddressRegister(0x2E);
    select();
    SPDR = 5;
    SPDR = 4;
//    convertByteToString(byte, output);
//    usbWriteString(output);
    if (SPDR != 0) {
      usbWriteString("muh\n");
    }
    if (SPSR & (1 << WCOL)) {
      usbWriteString("kaputt\n");
    } else {
      usbWriteString("meh\n");
    }
    PORTC ^= (1 << 6);
    _delay_ms(1000);
    periodicUsbTask();
  }

}

uint8_t readByteFromShortAddressRegister(uint8_t register_address) {
  uint8_t bit_mask = 0b01111110;
  uint8_t command = register_address << 1;
  command &= bit_mask;
  uint8_t buffer = 0;
  select();
  write(command);
  buffer = read();
  deselect();
  return buffer;
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

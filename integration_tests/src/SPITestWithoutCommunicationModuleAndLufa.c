#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);

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


void readString(uint8_t *string, uint8_t size) {
  for(;size > 0; size--) {
    *string++ = read();
  }
}

void setup(void) {
  DDRB = (_BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB4));
  PORTB |= _BV(PORTB0) | _BV(PORTB4);
  SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR1));
  DDRC = _BV(6);
}

int main(void) {
  setup();
  for (int i = 0; i< 10; i++) {
    PORTC ^= _BV(6);
    _delay_ms(100);
  }

  for(;;) {
    select();
    write(0x2E << 1);
    uint8_t byte = read();
    deselect();
    if (byte == 0x75){
      PORTC ^= _BV(6);
      _delay_ms(100);
    }
    else {
        PORTC ^= _BV(6);
        _delay_ms(1000);
      }
  }

}
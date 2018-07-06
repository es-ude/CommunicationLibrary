#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void convertByteToString(uint8_t byte, uint8_t *string);

uint8_t transfer(uint8_t byte) {
  SPDR = byte;
  while (!(SPSR & (1 << SPIF)))
    ;

  return SPDR;
}

void write(uint8_t byte) {
  transfer(byte);
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
  DDRB = (_BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2));
  PORTB |= _BV(PORTB0);
  SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR1));
  DDRC = _BV(6);
  _delay_ms(2000);
  for (int i = 0; i< 4; i++) {
    PORTC ^= _BV(6);
    _delay_ms(500);
  }

  for(;;) {
    write(0);
    PORTC ^= _BV(6);
    _delay_ms(1000);
  }

}
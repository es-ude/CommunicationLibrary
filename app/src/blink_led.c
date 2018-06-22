#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void setup_led(void) {
  DDRC = 1 << PC6;
}

void turn_on_led(void) {
  PORTC |= 1 << PC6;
}

void turn_off_led(void) {
  PORTC &= ~(1 << PC6);
}

int main(void) {
  setup_led();
  while (true) {
    turn_on_led();
    _delay_ms(1000);
    turn_off_led();
    _delay_ms(1000);
  }
}

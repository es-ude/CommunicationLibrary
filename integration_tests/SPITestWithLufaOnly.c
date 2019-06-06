#include <stdint.h>
#include <util/delay.h>
#include "Setup/DebugSetup.h"
#include "integration_tests/HelpersForUsageWithoutCommModule.h"
#include "Util/Debug.h"
#include <avr/io.h>

void setup(void);
uint8_t readByteFromShortAddressRegister(uint8_t register_address);
void writeByteToShortRegisterAddress(uint8_t register_address, uint8_t byte);
void convertByteToString(uint8_t byte, uint8_t *string);
void debugPrintHex(uint8_t byte);

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

static inline uint8_t MRF_writeShortCommand(uint8_t address) {
  return (uint8_t)((address << 1 & ~(1 << 7)) | 1);
}

static inline uint8_t MRF_readShortCommand(uint8_t address) {
  return (uint8_t)(address << 1);
}

void writeStringToTX(const uint8_t *data, uint8_t length)
{
  uint8_t command[] = {0x80, 0x10};
//  uint8_t command[] = {MRF_writeShortCommand(1)};
  select();
  write(command[0]);
  write(command[1]);
  while(length > 0)
  {
    write(*data);
    data++;
    length--;
  }
  deselect();
}

void readBurst(uint8_t *data,
               uint8_t length)
{
  uint8_t command[] = {0x80, 0x00};
//  uint8_t command[] = {MRF_readShortCommand(1)};
  select();
  write(command[0]);
  write(command[1]);
  while(length > 0)
  {
    *data = read();
    data++;
    length--;
  }
  deselect();
}

int main(void){
  setup();
  uint8_t output[] = "0x00 was read, and 0x75 was expected\n";
  uint8_t byte = 0xAB;
  debug(String, (uint8_t*)"Start\n");
  writeByteToShortRegisterAddress(0x2F, 0x55);
  for(;;) {
    byte = readByteFromShortAddressRegister(0x2E);
    convertByteToString(byte, output);
    debug(String, output);
    writeStringToTX("hello world", 11);
    uint8_t text[11];
    text[10] = '\0';
    readBurst(text, 11);
    debug(String, text);
    debug(String, "\n");
    _delay_ms(1000);
  }
}



void setup(void) {
  setUpDebugging();
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

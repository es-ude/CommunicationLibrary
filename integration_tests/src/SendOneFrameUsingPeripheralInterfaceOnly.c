#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "integration_tests/src/config.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"
#include "integration_tests/LUFA-Setup/Helpers.h"

/*
 * This test sends one frame containing the payload 'aa'
 * to an XBee with the address 0013A2004175A89D
 * Setup of the MRF Chip is performed as shown in it's data sheet.
 * The control bytes of the frame header are set as follows:
 * +---+-----------------------------+
 * | 0 | Reserved                    |
 * +---+-----------------------------+
 * | 1 | Pan Id Compression          |
 * +---+-----------------------------+
 * | 1 | Acknowledgement Request     | // I could notice inconsistencies without acks
 * +---+-----------------------------+
 * | 0 | Frame Pending               |
 * +---+-----------------------------+
 * | 0 | Security Enabled            |
 * +---+-----------------------------+
 * | 0 |                             |
 * | 0 | Frame Type - Data Frame     |
 * | 1 |                             |
 * +---+-----------------------------+
 * | 1 | Source Addressing Mode      |
 * | 0 | 16 bit                      |
 * +---+-----------------------------+
 * | 1 | Frame Version               |
 * | 0 | 2015                        |
 * +---+-----------------------------+
 * | 1 | Destination Addressing      |
 * | 1 | Mode 64bit                  |
 * +---+-----------------------------+
 * | 0 | Information Element Present |
 * +---+-----------------------------+
 * | 0 | Sequence Number Suppression | // 2015 version feature, not supported by XbeeS2C
 * +---+-----------------------------+
 *
 * Note that the bit order shown is reversed from the
 * order one can see in the ieee specs of 802.15.4
 * We need to work with the least significant bit in
 * last position here.
 * The Pan Id, Destination and Source Addresses are
 * represented in little endian.
 * */


void debugPrintHex(uint8_t byte);

void printTxMemory(uint8_t length) {
  uint16_t address = 0;
  uint8_t address_high = address >> 3 | 0x80;
  uint8_t address_low = address << 5;
  uint8_t read_tx_command[] = {address_high, address_low};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, read_tx_command, 2);
  uint8_t buffer[length];
  PeripheralInterface_readBlocking(peripheral_interface, buffer, length);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  debug("content: ");
  for (uint8_t i = 0; i < length; i++) {
    debugPrintHex(buffer[i]);
  }
  debug("\n");
}

void setShortRegister(uint8_t address, uint8_t value) {
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  address = (uint8_t)(address << 1 | 1);
  PeripheralInterface_writeBlocking(peripheral_interface, &address, 1);
  PeripheralInterface_writeBlocking(peripheral_interface, &value, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void setLongRegister(uint16_t address, uint8_t value) {
  uint8_t address_high = (uint8_t)(address >> 3 | 0x80);
  uint8_t address_low = (uint8_t)(address << 5 | 0x10);
  uint8_t command[] = {address_high, address_low};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, command, 2);
  PeripheralInterface_writeBlocking(peripheral_interface, &value, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void setRegister(uint16_t address, uint8_t value) {
  if (address > 0x3F) {
    setLongRegister(address, value);
  }
  else {
    setShortRegister((uint8_t)address, value);
  }
}

void printRegister(uint16_t address);

void initMrf(void) {
  uint8_t number_of_registers = 16;
  uint16_t register_value_pairs[16][2] = {
          {0x2A, 0x07},
          {0x18, 0x98},
          {0x2E, 0x95},
          {0x200, 0x03},
          {0x201, 0x01},
          {0x202, 0x80},
          {0x206, 0x90},
          {0x207, 0x80},
          {0x208, 0x10},
          {0x220, 0x21},
          {0x3A, 0x80},
          {0x3F, 0x60},
          {0x3E, 0x40},
          {0x200, 0x13},
          {0x36, 0x04},
          {0x36, 0x00},
  };
  for (uint8_t current_parameter = 0;
       current_parameter < number_of_registers;
       current_parameter++)
  {
    setRegister(register_value_pairs[current_parameter][0],
                (uint8_t )register_value_pairs[current_parameter][1]);
  }
  _delay_ms(1);
  for (uint8_t current_parameter = 0;
          current_parameter < number_of_registers;
          current_parameter++){
    printRegister(register_value_pairs[current_parameter][0]);
  }
}

void printRegister(uint16_t address) {
  uint8_t value;
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  if (address <= 0x3F) {
    uint8_t command = (uint8_t) (address << 1);
    PeripheralInterface_writeBlocking(peripheral_interface, &command, 1);
  }
  else {
    uint8_t command[] = {(uint8_t) (address >> 3| 0x80), (uint8_t)(address << 5)};
    PeripheralInterface_writeBlocking(peripheral_interface, &command, 2);
  }
  PeripheralInterface_readBlocking(peripheral_interface, &value, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  debug("Config-Werte:");
  debugPrintHex(value);
  debug("\n");
}

void sendToCoordinator(void) {
  uint8_t frame[] = {
          // frame header length, frame length
          0x0F, 0x11,
          // frame header control section
          0b01100001, 0b10101110,
          // sequence number
          0x00,
          // 16 bit source address
          0x34, 0x12,
          // 64 bit destination address
          0x9D, 0xA8,
          0x75, 0x41,
          0x00, 0xA2,
          0x13, 0x00,
          // destination pan id
          0xAA, 0xBB,
          // payload; just the string 'aa'
          0x61, 0x61,
  };
  uint8_t write_command[] = {0x80, 0x10};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, write_command, 2);
  PeripheralInterface_writeBlocking(peripheral_interface, frame, 19);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  uint8_t trigger_send = 5;
  *write_command = mrf_register_tx_normal_fifo_control << 1 |1;
  PeripheralInterface_writeBlocking(peripheral_interface, write_command, 1);
  PeripheralInterface_writeBlocking(peripheral_interface, &trigger_send, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}
int main(void) {
  setUpPeripheral();
  setUpUsbSerial();
  initMrf();

  _delay_ms(1000);
  printTxMemory(18);

  while(1) {
    _delay_ms(1000);
    sendToCoordinator();

    _delay_ms(50);
    printTxMemory(18);
  }
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

void debugPrintHex(uint8_t byte) {
  uint8_t string[] = "0x00 ";
  convertByteToString(byte, string);
  debug(string);
}
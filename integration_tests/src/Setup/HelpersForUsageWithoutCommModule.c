#include <stdint.h>
#include <util/delay.h>
#include "PeripheralInterface/PeripheralInterface.h"
#include "integration_tests/src/Setup/HardwareSetup.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "integration_tests/src/Setup/HelpersForUsageWithoutCommModule.h"
#include "Util/Debug.h"

void
debugPrintHex(uint8_t byte);

static inline uint16_t
MRF_readLongCommand(uint16_t address)
{
  return (uint16_t) (1 << 15 | address << 5);
}

static inline uint8_t
MRF_readLongCommandFirstByte(uint16_t address)
{
  return (uint8_t) (MRF_readLongCommand(address) >> 8);
}

static inline uint8_t
MRF_readLongCommandSecondByte(uint16_t address)
{
  return (uint8_t) (MRF_readLongCommand(address));
}

static inline uint8_t
MRF_readShortCommand(uint8_t address)
{
  return (uint8_t) (address << 1);
}

static inline uint16_t
MRF_writeLongCommand(uint16_t address)
{
  return (uint16_t) (MRF_readLongCommand(address) | 1 << 4);
}

static inline uint8_t
MRF_writeShortCommand(uint8_t address)
{
  return (uint8_t) ((address << 1 & ~(1 << 7)) | 1);
}

static inline uint8_t
MRF_writeLongCommandFirstByte(uint16_t address)
{
  return (uint8_t) (MRF_writeLongCommand(address) >> 8);
}

static inline uint8_t
MRF_writeLongCommandSecondByte(uint16_t address)
{
  return (uint8_t) MRF_writeLongCommand(address);
}

static inline uint8_t
MRF_getRegisterValueForChannelNumber(uint8_t channel_number)
{
  return (uint8_t) (((channel_number - 11) << 4) | 0x03);
}


uint8_t
readByteFromShortAddress(uint8_t address)
{
  uint8_t command[] = {MRF_readShortCommand(address)};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, command, 1);
  uint8_t value = 0;
  PeripheralInterface_readBlocking(peripheral_interface, &value, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  return value;
}

void
writeByteToShortAddress(uint8_t address,
                        uint8_t byte)
{
  uint8_t command[] = {MRF_writeShortCommand(address)};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, command, 1);
  PeripheralInterface_writeBlocking(peripheral_interface, &byte, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void
readByteWiseFromShortAddress(uint8_t address,
                             uint8_t *data,
                             uint8_t length)
{
  for (; length > 0; length--)
  {
    *data++ = readByteFromShortAddress(address++);
  }
}

void
writeByteWiseToShortAddress(uint8_t address,
                            uint8_t *data,
                            uint8_t length)
{
  for (; length > 0; length--)
  {
    writeByteToShortAddress(address++, *data++);
  }
}

uint8_t
readByteFromLongAddress(uint16_t address)
{
  uint8_t byte = 0;
  uint8_t command[2] = {
          MRF_readLongCommandFirstByte(address),
          MRF_readLongCommandSecondByte(address),
  };
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, command, 2);
  PeripheralInterface_readBlocking(peripheral_interface, &byte, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
  return byte;
}

void
readBurstFromLongAddress(uint16_t address,
                         uint8_t *data,
                         uint8_t length)
{

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  uint8_t command[2] = {
          MRF_readLongCommandFirstByte(address),
          MRF_readLongCommandSecondByte(address),
  };
  PeripheralInterface_writeBlocking(peripheral_interface, command, 2);
  PeripheralInterface_readBlocking(peripheral_interface, data, length);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void writeBurstToLongAddress(uint16_t address,
                             const uint8_t *data,
                             uint8_t length)
{

  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  uint8_t command[2] = {
          MRF_writeLongCommandFirstByte(address),
          MRF_writeLongCommandSecondByte(address),
  };
  PeripheralInterface_writeBlocking(peripheral_interface, command, 2);
  PeripheralInterface_writeBlocking(peripheral_interface, data, length);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void
writeByteToLongAddress(uint16_t address,
                       uint8_t value)
{
  uint8_t command[] = {
          MRF_writeLongCommandFirstByte(address),
          MRF_writeLongCommandSecondByte(address),
  };
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, command, 2);
  PeripheralInterface_writeBlocking(peripheral_interface, &value, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void
setShortRegister(uint8_t address,
                 uint8_t value)
{
  writeByteToShortAddress(address, value);
}

void
setLongRegister(uint16_t address,
                uint8_t value)
{
  writeByteToLongAddress(address, value);
}

void
setRegister(uint16_t address,
            uint8_t value)
{
  if (address > 0x3F)
  {
    setLongRegister(address, value);
  } else
  {
    setShortRegister((uint8_t) address, value);
  }
}

void
printRegister(uint16_t address);

void
enablePromiscuous(void)
{
  setShortRegister(mrf_register_receive_mac_control, 1);
}

uint8_t
getReceivedMessageSize(void)
{
  uint8_t size = 0;
  size = readByteFromLongAddress(mrf_rx_fifo_start);
  return size + 1;
}

bool
gotNewMessage(void)
{
  uint8_t value = readByteFromShortAddress(mrf_register_interrupt_status);
  bool    new_message = ((value >> 3) & 1);
  return new_message;
}

uint8_t
convertNumberToASCII(uint8_t number)
{
  if (number >= 0 && number <= 9)
  {
    return (uint8_t) ('0' + number);
  } else
  {
    return (uint8_t) ('A' + (number - 10));
  }
}

void
convertByteToString(uint8_t byte,
                    uint8_t *string)
{
  uint8_t upper_half = byte >> 4;
  uint8_t lower_half = (uint8_t) (byte & 0x0F);
  string[2] = convertNumberToASCII(upper_half);
  string[3] = convertNumberToASCII(lower_half);
}

void
debugPrintHex(uint8_t byte)
{
  char string[] = "0x00 ";
  convertByteToString(byte, string);
  debug(String, string);
}


void printRegister(uint16_t address) {
  uint8_t value;
  if (address > 0x3F)
  {
    value = readByteFromLongAddress(address);
  }
  else
  {
    value = readByteFromShortAddress(address);
  }
  debug(String, "Config-Werte:");
  debugPrintHex(value);
  debug(String, "\n");
}

void
initMrf(void)
{
  uint8_t number_of_registers = 17;
  uint16_t register_value_pairs[17][2] = {
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
          {mrf_register_interrupt_control, 0b11110110},
          {0x200, 0x13},
          {0x36, 0x04},
          {0x36, 0x00},
  };
  _delay_ms(1);
  for (uint8_t current_parameter = 0;
       current_parameter < number_of_registers;
       current_parameter++)
  {
    setRegister(register_value_pairs[current_parameter][0],
                (uint8_t) register_value_pairs[current_parameter][1]);
  }
  for (uint8_t current_parameter = 0;
  current_parameter < number_of_registers;
  current_parameter++)
  {
    printRegister(register_value_pairs[current_parameter][0]);
  }
  setRegister(mrf_register_interrupt_control, 0b11110110);
  uint8_t pan_id[] = {0x34, 0x12};
  uint8_t short_source_address[] = {0xBB, 0xBB};
  uint8_t extended_source_address[] = {
          0x08, 0x07,
          0x06, 0x05,
          0x04, 0x03,
          0x02, 0x01,
  };
  writeByteWiseToShortAddress(mrf_register_pan_id_low_byte, pan_id, 2);
  writeByteWiseToShortAddress(mrf_register_short_address_low_byte, short_source_address, 2);
  writeByteWiseToShortAddress(mrf_register_extended_address0, extended_source_address, 8);
}

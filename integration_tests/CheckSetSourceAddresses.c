#include "PeripheralInterface/PeripheralInterface.h"
#include "CommunicationModule/CommunicationModule.h"
#include "integration_tests/HelpersForUsageWithoutCommModule.h"
#include "Setup/HardwareSetup.h"
#include "Setup/DebugSetup.h"
#include "EmbeddedUtilities/Debug.h"
#include "CException.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

void
select(void)
{
  PORTB &= ~(_BV(PORTB4));
}

void
deselect(void)
{
  PORTB |= _BV(PORTB4);
}

uint8_t
transfer(uint8_t byte)
{
  SPDR = byte;
  while (!(SPSR & (1 << SPIF))) {}
  return SPDR;
}

void
write(uint8_t byte)
{
  transfer(byte);
}

uint8_t
read(void)
{
  return transfer(0);
}

static inline uint16_t MRF_readLongCommand(uint16_t address) {
  return (uint16_t)(1 << 15 | address << 5);
}

static inline uint8_t MRF_readLongCommandFirstByte(uint16_t address) {
  return (uint8_t) (MRF_readLongCommand(address) >> 8);
}

static inline uint8_t MRF_readLongCommandSecondByte(uint16_t address) {
  return (uint8_t) (MRF_readLongCommand(address));
}

static inline uint8_t MRF_readShortCommand(uint8_t address) {
  return (uint8_t)(address << 1);
}

static inline uint16_t MRF_writeLongCommand(uint16_t address) {
  return (uint16_t)(MRF_readLongCommand(address) | 1 << 4);
}

static inline uint8_t MRF_writeShortCommand(uint8_t address) {
  return (uint8_t)((address << 1 & ~(1 << 7)) | 1);
}

static inline uint8_t MRF_writeLongCommandFirstByte(uint16_t address) {
  return (uint8_t) (MRF_writeLongCommand(address) >> 8);
}

static inline uint8_t MRF_writeLongCommandSecondByte(uint16_t address) {
  return (uint8_t ) MRF_writeLongCommand(address);
}

static inline uint8_t MRF_getRegisterValueForChannelNumber(uint8_t channel_number) {
  return (uint8_t)(((channel_number - 11) << 4) | 0x03);
}


uint8_t
readByte(uint8_t address)
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
writeByte(uint8_t address,
          uint8_t byte)
{
  uint8_t command[] = {MRF_writeShortCommand(address)};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
  PeripheralInterface_writeBlocking(peripheral_interface, command, 1);
  PeripheralInterface_writeBlocking(peripheral_interface, &byte, 1);
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
}

void
readByteWise(uint8_t address,
             uint8_t *data,
             uint8_t length)
{
  for (; length > 0; length--)
  {
    *data++ = readByte(address++);
  }
}

void
writeByteWise(uint8_t address,
              uint8_t *data,
              uint8_t length)
{
  for (; length > 0; length--)
  {
    writeByte(address++, *data++);
  }
}

void
readBurstFromShortAddress(uint8_t address,
                          uint8_t *data,
                          uint8_t length)
{
//  uint8_t command[] = {0x80, 0x00};
  uint8_t command[] = {MRF_readShortCommand(address)};
  PeripheralInterface_selectPeripheral(peripheral_interface, &mrf_spi_client);
//  select();
  debug(String, "selected\n");
  PeripheralInterface_writeBlocking(peripheral_interface, command, 1);
  //  write(command[1]);
  debug(String, "wrote command\n");

  PeripheralInterface_readBlocking(peripheral_interface, data, length);
//  while (length > 0)
//  {
//    *data = read();
//    data++;
//    length--;
//  }
  PeripheralInterface_deselectPeripheral(peripheral_interface, &mrf_spi_client);
//  deselect();

}

int
main(void)
{
  setUpDebugging();

  setUpMac();
  Mac802154Config config = {
          .channel = 0x0C,
          .pan_id = {0x34, 0x12},
          .short_source_address = {0xAA, 0xAA},
          .extended_source_address = {
                  0x11, 0x22,
                  0x33, 0x44,
                  0x55, 0x66,
                  0x77, 0x88,
          },
  };
  uint8_t counter = 0;
  while (1)
  {
    _delay_ms(2000);
    debug(String, "configure\n");
    CEXCEPTION_T e;

    Try
        {
          Mac802154_configure(mac802154, &config);

          debug(String, "configured\n");
//          config.short_source_address[0] = 0xBB;
//          config.short_source_address[1] = 0xCC;

          uint8_t array[2] = {0xbb, counter};
          counter++;
//          writeByteWise(3, array, 2);
          writeBurstToLongAddress(3, array, 2);
          uint8_t data[13] = {0};
          debug(String, "reading\n");
//          readBurstFromShortAddress(1, data, 12);
          readByteWise(1, data, 12);
          debug(String, "read\n");
          for (uint8_t counter = 0; counter < 12; counter++)
          {
            uint8_t text[10];
            sprintf(text, "0x%02x ", data[counter]);
            debug(String, text);
          }
          debug(String, "\n");
        } Catch(e)
    {
      uint8_t text[10];
      sprintf(text, "%u\n", e);
      debug(String, "got exception number\n");
      debug(String, text);
    }
  }
}
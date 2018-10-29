#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "integration_tests/src/config.h"
#include "include/Mac802154MRFImpl.h"
#include "integration_tests/LUFA-Setup/Helpers.h"

int main(void) {
  setUpPeripheral();
  setUpUsbSerial();

  Mac802154Config config = {
          .interface = peripheral_interface,
          .channel = 12,
          .pan_id = 0x3332,
          .short_source_address = 0xAABB,
          .extended_source_address = 0x1122334455667788,
          .device = &mrf_spi_client,
  };
  _delay_ms(1000);
  uint8_t raw_memory[Mac802154MRF_getADTSize ()];
  Mac802154MRF_create(raw_memory, delay_microseconds);
  Mac802154 *mac = (Mac802154*) raw_memory;
  Mac802154_reconfigure(mac, &config);
  Mac802154_enablePromiscuousMode(mac);
  debug("Start\n");
  _delay_ms(500);
  while(true) {
      while(!Mac802154_newPacketAvailable(mac)) {
          _delay_ms(500);
        }
      uint8_t size = Mac802154_getReceivedPacketSize(mac);
      uint8_t packet[size];
      Mac802154_fetchPacketBlocking(mac, packet, size);
      uint16_t short_source_address = Mac802154_getPacketShortSourceAddress(mac, packet);
      Mac802154_setShortDestinationAddress(mac, short_source_address);
      char string[80];
      if (Mac802154_packetAddressIsShort(mac, packet))
        {
          debug("short packet address - ");
        }
      else if (Mac802154_packetAddressIsLong(mac, packet))
      {
        debug("packet address is long - ");
      }
      else
        {
          uint8_t size = Mac802154_getPacketSourceAddressSize(mac, packet);
          char size_string[32];
          sprintf(size_string, "packet source address size: %i\n");
          debug(size_string);
        }
       sprintf(string, "address: %#08x\n", short_source_address);
    debug(string);
      debug(packet);
      Mac802154_setPayload(mac, (char *)packet, size);
    Mac802154_sendBlocking(mac);
//    Mac802154_setPayload(mac, &short_source_address, 2);
//    Mac802154_sendBlocking(mac);
//    Mac802154_setPayload(mac, Mac802154_getPacketPayload(mac, packet), Mac802154_getPacketPayloadSize(mac, packet));
    Mac802154_sendBlocking(mac);
  }
}
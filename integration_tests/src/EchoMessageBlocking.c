#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "integration_tests/src/config.h"
#include "CommunicationModule/Mac802154MRFImpl.h"
#include "integration_tests/LUFA-Setup/Helpers.h"

int main(void) {
  setUpUsbSerial();
  setUpPeripheral();

  Mac802154Config config = {
          .channel = 12,
          .pan_id = {0x33, 0x32},
          .short_source_address = {0xAA, 0xBB},
          .extended_source_address = {
              0x11, 0x22,
              0x33, 0x44,
              0x55, 0x66,
              0x77, 0x88,
          },
  };

  MRFConfig hardware_config = {
    .reset_line = {
      .data_direction_register = NULL,
      .data_register = NULL,
    },
    .delay_microseconds = delay_microseconds,
    .interface = peripheral_interface,
    .device = &mrf_spi_client,
  };
  uint8_t raw_memory[Mac802154MRF_getADTSize ()];
  Mac802154MRF_create(raw_memory, &hardware_config);
  Mac802154 mac = (Mac802154) raw_memory;
  Mac802154_configure(mac, &config);
  char string[7];
  uint16_t counter = 0;
  while(true) {
      while(!Mac802154_newPacketAvailable(mac)) 
      {
      }
      counter++;
      
        sprintf(string, "%i\n", counter);
        debug(string);
      uint8_t size = Mac802154_getReceivedPacketSize(mac);
      uint8_t packet[size];
      Mac802154_fetchPacketBlocking(mac, packet, size);
    /* Mac802154_setPayload(mac, (char *)packet, size); */
    /* Mac802154_sendBlocking(mac); */
    /* Mac802154_setPayload(mac, &short_source_address, Mac802154_getPacketSourceAddressSize(mac, packet)); */
    /* Mac802154_sendBlocking(mac); */
      uint8_t *payload = Mac802154_getPacketPayload(mac, packet);
      if (*payload == 'e')
      {
        uint8_t *short_source_address = Mac802154_getPacketShortSourceAddress(mac, packet);
        Mac802154_setShortDestinationAddress(mac, short_source_address);
        Mac802154_setPayload(mac, Mac802154_getPacketPayload(mac, packet), Mac802154_getPacketPayloadSize(mac, packet));
        Mac802154_sendBlocking(mac);
      }
  }
}

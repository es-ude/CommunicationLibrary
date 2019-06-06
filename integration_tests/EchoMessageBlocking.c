#include <avr/io.h>
#include <avr/interrupt.h>
#include "Setup/HardwareSetup.h"
#include "CommunicationModule/Mac802154MRFImpl.h"

int main(void) {
  setUpMac();

  Mac802154Config config = {
          .channel = 12,
          .pan_id = {0xcc, 0xdd},
          .short_source_address = {0xAA, 0xAA},
          .extended_source_address = {
              0x11, 0x22,
              0x33, 0x44,
              0x55, 0x66,
              0x77, 0x88,
          },
  };

  Mac802154_configure(mac802154, &config);
  while(true) {
      while(!Mac802154_newPacketAvailable(mac802154))
      {}      
      uint8_t size = Mac802154_getReceivedPacketSize(mac802154);
      uint8_t packet[size];
      Mac802154_fetchPacketBlocking(mac802154, packet, size);

      const uint8_t *payload = Mac802154_getPacketPayload(mac802154, packet);
      const uint8_t *short_source_address = Mac802154_getPacketShortSourceAddress(mac802154, packet);
      Mac802154_setShortDestinationAddress(mac802154, short_source_address);
      Mac802154_setPayload(mac802154, Mac802154_getPacketPayload(mac802154, packet), Mac802154_getPacketPayloadSize(mac802154, packet));
      Mac802154_sendBlocking(mac802154);
  }
}

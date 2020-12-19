#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "Setup/HardwareSetup.h"
#include "EmbeddedUtilities/Debug.h"
#include "Setup/DebugSetup.h"
#include "CommunicationModule/Mac802154MRFImpl.h"
#include <stdio.h>

int main(void) {
  setUpDebugging();
  setUpMac();
  Mac802154Config config = {
          .channel = 0x12,
          .pan_id = {0x34, 0x12},
          .short_source_address = {0xAA, 0xAA},
          .extended_source_address = {
                  0x11, 0x22,
                  0x33, 0x44,
                  0x55, 0x66,
                  0x77, 0x88,
          },
  };
  _delay_ms(1000);
  Mac802154_configure(mac802154, &config);
  char payload[32];
  Mac802154_setExtendedDestinationAddress(mac802154, (uint8_t[]) {
          0x92, 0x53,
          0x9B, 0x41,
          0x00, 0xA2,
          0x13, 0x00,
  });
  uint16_t number = 0;

  while(true) {

    number++;
    _delay_ms(1000);
    sprintf(payload, "%d\n", number);
    debug(String, payload);
    uint8_t payload_length = (uint8_t) strlen(payload);
    Mac802154_setPayload(mac802154, (uint8_t *) payload, payload_length);
    Mac802154_sendBlocking(mac802154);

    while (!Mac802154_newPacketAvailable(mac802154)) {
    }
    uint8_t packet_size = Mac802154_getReceivedPacketSize(mac802154);
    uint8_t packet[packet_size];
    Mac802154_fetchPacketBlocking(mac802154, packet, packet_size);
    debug(String, "New Message:\n\t");
    debug(String, "tnumber:\n\t");

    printAddress(Mac802154_getPacketSourceAddressSize,
                    Mac802154_getPacketShortSourceAddress,
                    packet);
  }
}

void
printAddress(uint8_t (*getAddressSize)(Mac802154 *, const uint8_t *),
             const uint8_t *(*getAddress)(const Mac802154 *, const uint8_t *),
             const uint8_t *packet)
{
    uint8_t address_size = getAddressSize(mac802154, packet);
    const uint8_t *address = getAddress(mac802154, packet);
    debug(String, "\n\tsource address size in byte:\n\t\t");
    char size_string[16];
    sprintf(size_string, "%i", address_size);
    debug(String, size_string);
    debug(String, "\n\tsource address: \n\t\t");
    printBytesAsHex(address, address_size);
}

void
printBytesAsHex(const uint8_t *bytes, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        char hex[6];
        sprintf(hex, "0x%02x ", bytes[i]);
        debug(String, hex);
        if ((i + 1) % 8 == 0 && i < count - 1) {
            debug(String, "\n\t\t");
        }
    }

}
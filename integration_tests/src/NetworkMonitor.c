#include <stdint.h>
#include "integration_tests/src/Setup/HardwareSetup.h"
#include "integration_tests/src/Setup/DebugSetup.h"
#include "Util/Debug.h"
#include <util/delay.h>
#include <stdio.h>

void
printAddress(uint8_t (*getAddressSize)(Mac802154 *, const uint8_t*),
             const uint8_t *(*getAddress)(Mac802154 *, const uint8_t *),
             const uint8_t *packet);

void
printBytesAsHex(const uint8_t *bytes, uint8_t count);

int
main(void)
{
  setUpDebugging();

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

  Mac802154_enablePromiscuousMode(mac802154);
  _delay_ms(2000);
  printString("Startup\n");
  uint16_t counter = 0;
  while (1)
  {
    printString("waiting...\n");

    while (!Mac802154_newPacketAvailable(mac802154))
    {
    }
    uint8_t packet_size = Mac802154_getReceivedPacketSize(mac802154);
    uint8_t packet[packet_size];
    Mac802154_fetchPacketBlocking(mac802154, packet, packet_size);
    debug(String, "New Message:\n\tnumber:");
    debug(Uint16, counter);
    counter++;
    printAddress(Mac802154_getPacketSourceAddressSize,
                 Mac802154_getPacketShortSourceAddress,
                 packet);
    debug(String, "\n\tpayload: \n\t\t");
    uint8_t payload_size = Mac802154_getPacketPayloadSize(mac802154, packet);
    const uint8_t *payload = Mac802154_getPacketPayload(mac802154, packet);
    debug(String, (char *)payload);
    debug(String, "\n\tpayload in hex: \n\t\t");
    printBytesAsHex(payload, payload_size);
    debug(String, "\n\twhole packet in hex: \n\t\t");
    printBytesAsHex(packet, packet_size);
    debug(String,"\n\n");
  }
}

void
printAddress(uint8_t (*getAddressSize)(Mac802154*, const uint8_t*),
             const uint8_t *(*getAddress)(Mac802154*, const uint8_t *),
             const uint8_t *packet)
{
  uint8_t         address_size = getAddressSize(mac802154, packet);
  const uint8_t * address      = getAddress(mac802154, packet);
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
  for (uint8_t i=0; i<count; i++)
  {
    char hex[6];
    sprintf(hex, "0x%02x ", bytes[i]);
    debug(String, hex);
    if ((i+1)%8==0 && i < count-1)
    {
      debug(String, "\n\t\t");
    }
  }

}
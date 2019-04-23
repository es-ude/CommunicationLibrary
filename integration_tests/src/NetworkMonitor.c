#include "integration_tests/src/Setup/HardwareSetup.h"
#include "integration_tests/LUFA-Setup/Helpers.h"
#include "integration_tests/src/Setup/DebugSetup.h"
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
  setUpMac();
  setUpDebugging();

  Mac802154_enablePromiscuousMode(mac802154);
  while (1)
  {
    debug("waiting...\n");
    while (!Mac802154_newPacketAvailable(mac802154))
    {}
    uint8_t packet_size = Mac802154_getReceivedPacketSize(mac802154);
    uint8_t packet[packet_size];
    Mac802154_fetchPacketBlocking(mac802154, packet, packet_size);
    debug("New Message:");
    printAddress(Mac802154_getPacketSourceAddressSize,
                 Mac802154_getPacketShortSourceAddress,
                 packet);
    debug("\tpayload: \n\t\t");
    uint8_t payload_size = Mac802154_getPacketPayloadSize(mac802154, packet);
    const uint8_t *payload = Mac802154_getPacketPayload(mac802154, packet);
    debugSized((char *)payload, payload_size);
    debug("\n\tpayload in hex: \n\t\t");
    printBytesAsHex(payload, payload_size);
    debug("\n\twhole packet in hex: \n\t\t");
    printBytesAsHex(packet, packet_size);
    debug("\n\n");
    periodicUsbTask();
  }
}

void
printAddress(uint8_t (*getAddressSize)(Mac802154*, const uint8_t*),
             const uint8_t *(*getAddress)(Mac802154*, const uint8_t *),
             const uint8_t *packet)
{
  uint8_t         address_size = getAddressSize(mac802154, packet);
  const uint8_t * address      = getAddress(mac802154, packet);
  debug("\n\tsource address size in byte:\n\t\t");
  char size_string[16];
  sprintf(size_string, "%i", address_size);
  debug(size_string);
  debug("\n\tsource address: \n\t\t");
  printBytesAsHex(address, address_size);
  debug("\n\tdestination address: \n\t\t");
}

void
printBytesAsHex(const uint8_t *bytes, uint8_t count)
{
  for (uint8_t i=0; i<count; i++)
  {
    char hex[6];
    sprintf(hex, "0x%02x ", bytes[i]);
    debug(hex);
    if ((i+1)%8==0 && i < count-1)
    {
      debug("\n\t\t");
    }
  }

}
#include "Debug/Debug.h"
#include "integration_tests/src/Setup/HardwareSetup.h"
#include "integration_tests/src/Setup/DebugSetup.h"
#include <util/delay.h>
#include <stdbool.h>

int
main(void)
{
  setUpMac();
  setUpDebugging();
  Mac802154Config config =
      {
      .channel = 0x0C,
      .extended_source_address = {0x08, 0x07, 0x06,
                                  0x05, 0x04, 0x03,
                                  0x02, 0x01},
      .short_source_address = {0xAA, 0xAA},
      .pan_id = {0xCC, 0xDD},
      };
  Mac802154_configure(mac802154, &config);
  debugLine("Start");
  while (true)
  {
    _delay_ms(1000);
    uint8_t packet_size = Mac802154_getReceivedPacketSize(mac802154);
    uint8_t packet[packet_size+1];
    packet[packet_size] = '\0';
    debugDec16(packet_size);
    debugNewLine();
    Mac802154_fetchPacketBlocking(mac802154, packet, packet_size);
    debugSizedString((const char*)Mac802154_getPacketPayload(mac802154,packet),
            Mac802154_getPacketPayloadSize(mac802154,
                packet));
    debugNewLine();
    if (*Mac802154_getPacketPayload(mac802154, packet) == 'e')
    {
      Mac802154_setPayload(mac802154,
          Mac802154_getPacketPayload(mac802154, packet),
          Mac802154_getPacketPayloadSize(mac802154, packet));
      Mac802154_setShortDestinationAddress(mac802154,
          Mac802154_getPacketShortSourceAddress(mac802154, packet));
      Mac802154_sendBlocking(mac802154);
    }
  }
}
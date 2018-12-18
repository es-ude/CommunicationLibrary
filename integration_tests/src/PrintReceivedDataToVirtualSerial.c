#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "integration_tests/src/config.h"
#include "CommunicationModule/Mac802154MRFImpl.h"
#include "integration_tests/LUFA-Setup/Helpers.h"

int main(void)
{
  setUpPeripheral();
  setUpUsbSerial();
  Mac802154Config config = {
      .channel = 12,
      .pan_id = {0x33,0x32},
      .short_source_address = { 0xAA, 0xBB },
      .extended_source_address = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88},
  };
  _delay_ms(1000);
  uint8_t raw_memory[Mac802154MRF_getADTSize()];
  Mac802154MRF_create((Mac802154) raw_memory, delay_microseconds, peripheral_interface, &mrf_spi_client);
  Mac802154 mac = (Mac802154 ) raw_memory;
  Mac802154_configure(mac, &config);
  Mac802154_enablePromiscuousMode(mac);
  _delay_ms(500);
  debugSized("Start\n", 6);
  while(true)
    {
      while(!Mac802154_newPacketAvailable(mac))
        {}
      uint8_t size = Mac802154_getReceivedPacketSize(mac);
      uint8_t packet[size];
      Mac802154_fetchPacketBlocking(mac, packet, size);
      debugSized(packet, size);
      debug("\n");
    }
}

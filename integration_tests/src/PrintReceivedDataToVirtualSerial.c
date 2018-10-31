#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "integration_tests/src/config.h"
#include "include/Mac802154MRFImpl.h"
#include "integration_tests/LUFA-Setup/Helpers.h"

int main(void)
{
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
  uint8_t raw_memory[Mac802154MRF_getADTSize()];
  Mac802154MRF_create(raw_memory, delay_microseconds);
  Mac802154 *mac = (Mac802154 *) raw_memory;
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
      uint16_t short_source_address = Mac802154_getPacketShortSourceAddress(mac, packet);
      debugSized(packet, size);
      debug("\n");
    }
}
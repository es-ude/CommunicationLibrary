#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stddef.h>
#include "Setup/HardwareSetup.h"
#include "CommunicationModule/Mac802154MRFImpl.h"

#include "Setup/DebugSetup.h"
#include "EmbeddedUtilities/Debug.h"

int main(void)
{
  setUpMac();
  setUpDebugging();
  Mac802154Config config = {
      .channel = 12,
      .pan_id = {0x33,0x32},
      .short_source_address = { 0xAA, 0xBB },
      .extended_source_address = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88},
  };
  MRFConfig hardware_config = {
  
    .reset_line = {
      .data_direction_register = NULL,
    },
    .delay_microseconds = delay_microseconds,
    .interface = peripheral_interface,
    .device = &mrf_spi_client,
  };
  _delay_ms(1000);

  Mac802154_configure(mac802154, &config);
  Mac802154_enablePromiscuousMode(mac802154);
  _delay_ms(500);
  debug(String, "Start\n");
  while(true)
    {
      while(!Mac802154_newPacketAvailable(mac802154))
        {}
      uint8_t size = Mac802154_getReceivedPacketSize(mac802154);
      uint8_t packet[size+1];
      packet[size] = '\0';
      Mac802154_fetchPacketBlocking(mac802154, packet, size);
      debug(String, packet);
      debug(String, "\n");
    }
}

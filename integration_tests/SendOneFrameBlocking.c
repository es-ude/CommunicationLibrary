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
          .channel = 0x0C,
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
          0x08, 0x07,
          0x06, 0x05,
          0x04, 0x03,
          0x02, 0x01,
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
  }
}
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "integration_tests/src/Setup/HardwareSetup.h"
#include "CommunicationModule/Mac802154MRFImpl.h"
#include <stdio.h>

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
  _delay_ms(1000);
  Mac802154_configure(mac802154, &config);
  char payload[32];
  Mac802154_setExtendedDestinationAddress(mac802154, (uint8_t []){
    0x00, 0x13,
    0xA2, 0x00,
    0x41, 0x75,
    0xA8, 0x9D
  });
  uint16_t number = 0;
  while(true) {
    number++;
    memset(payload, 0, 32);
    sprintf(payload, "%d\n", number);
    uint8_t payload_length = (uint8_t) strlen(payload);
    Mac802154_setPayload(mac802154, (uint8_t *) payload, payload_length);
    Mac802154_sendBlocking(mac802154);
  }
}
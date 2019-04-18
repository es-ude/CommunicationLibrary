#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "integration_tests/src/config.h"
#include "CommunicationModule/Mac802154MRFImpl.h"
#include <stdio.h>

int main(void) {

  setUpPeripheral();
  Mac802154Config config = {
          .interface = peripheral_interface,
          .channel = 0xC,
          .pan_id = 0x3332,
          .short_source_address = 0x1122,
          .extended_source_address = 0x1122334455667788,
          .device = &mrf_spi_client,
  };
  _delay_ms(1000);
  uint8_t raw_memory[Mac802154MRF_getADTSize ()];
  Mac802154MRF_create(raw_memory, delay_microseconds, peripheral_interface, &mrf_spi_client);
  Mac802154 mac = (Mac802154*) raw_memory;
  Mac802154_configure(mac, &config);
  char payload[32];
  Mac802154_setExtendedDestinationAddress(mac, 0x0013A2004175A89D);
  uint16_t number = 0;
  while(true) {
    number++;
    memset(payload, 32, 0);
    sprintf(payload, "%d\n", number);
    uint8_t payload_length = (uint8_t) strlen(payload);
    Mac802154_setPayload(mac, payload, payload_length);
    Mac802154_sendBlocking(mac);
  }
}
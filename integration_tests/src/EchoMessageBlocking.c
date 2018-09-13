#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "integration_tests/src/config.h"
#include "lib/include/Mac802154MRFImpl.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"
#include <stdio.h>

int main(void) {
  setup();
  setUpPeripheral();
  Mac802154Config config = {
          .interface = peripheral_interface,
          .channel = 12,
          .pan_id = 0x3332,
          .short_source_address = 0xAABB,
          .extended_source_address = 0x1111111111111111,
          .device = &mrf_spi_client,
  };
  _delay_ms(1000);
  debug("Start\n");
  uint8_t raw_memory[Mac802154MRF_requiredSize()];
  Mac802154MRF_create(raw_memory, delay_microseconds);
  Mac802154 *mac = (Mac802154*) raw_memory;
  Mac802154_init(mac, &config);
  Mac802154_setExtendedDestinationAddress(mac, 0x0013A2004175A89D);
  _delay_ms(500);
  while(true) {
    debug("wait for message\n");
    while(!Mac802154_newMessageAvailable(mac)) {}
    debug("Message\n");
    uint8_t size = Mac802154_getReceivedMessageSize(mac);
    uint8_t payload[size];
    Mac802154_fetchMessageBlocking(mac, payload, size);
    Mac802154_setPayload(mac, payload, size);
    Mac802154_sendBlocking(mac);
    payload[size-1] = 0;
    debug(payload);
    debug("\n");
  }
}
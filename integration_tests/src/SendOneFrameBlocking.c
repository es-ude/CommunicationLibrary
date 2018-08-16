#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "integration_tests/src/config.h"
#include "lib/include/Mac802154MRFImpl.h"
#include "integration_tests/LUFA-Setup/Helpers.h"


int main(void) {

  setup();
  Mac802154Config config = {
          .interface = peripheral_interface,
          .channel = 12,
          .pan_id = 0x1234,
          .short_source_address = 0x1122,
          .extended_source_address = 0x1122334455667788,
          .device = &mrf_spi_client,
  };
  _delay_ms(1000);
//  debug("START\n");
  char text[32];
  sprintf(text, "Mrf Implementation size: %d\n", Mac802154MRF_requiredSize());
//  debug(text);
  uint8_t raw_memory[Mac802154MRF_requiredSize()];
  Mac802154MRF_create(raw_memory, delay_microseconds);
//  debug("Mrf data structure initialized\n");
  Mac802154 *mac = (Mac802154*) raw_memory;
  Mac802154_init(mac, &config);
//  debug("Mrf initialized\n");
  uint8_t payload[] = "hello, world!";
  uint8_t text_size = strlen(payload);
  Mac802154_setPayload(mac, payload, text_size);
  Mac802154_setExtendedDestinationAddress(mac, 0x0013A2004175A89D);
  while(true) {
    _delay_ms(1000);
    Mac802154_sendBlocking(mac);
    // function below is only needed when you want to see the debug messages
//    periodicUsbTask();
  }
}
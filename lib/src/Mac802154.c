#include "lib/src/Mac802154/Mac802154.h"

extern void debug(uint8_t *string);

void Mac802154_init(Mac802154 *self, const Mac802154Config *config) {
  self->init(self, config);
}

void Mac802154_sendBlocking(Mac802154 *self) {
  self->sendBlocking(self);
}

void Mac802154_destroy(Mac802154 *self) {
  self->destroy(self);
}

void Mac802154_setShortDestinationAddress(Mac802154 *self, uint16_t address) {
  self->setShortDestinationAddress(self, address);
}

void Mac802154_setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length) {
  self->setPayload(self, payload, payload_length);
}

void Mac802154_setExtendedDestinationAddress(Mac802154 *self, uint64_t address) {
  self->setExtendedDestinationAddress(self, address);
}

uint8_t Mac802154_getReceivedMessageSize(Mac802154 *self) {
  return self->getReceivedMessageSize(self);
}

bool Mac802154_newMessageAvailable(Mac802154 *self) {
  return self->newMessageAvailable(self);
}
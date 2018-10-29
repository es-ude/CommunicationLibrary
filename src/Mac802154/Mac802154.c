#include "src/Mac802154/Mac802154.h"

extern void debug(uint8_t *string);

void Mac802154_configure(Mac802154 *self, const Mac802154Config *config) {
  self->reconfigure(self, config);
}

void Mac802154_sendBlocking(Mac802154 *self) {
  self->sendBlocking(self);
}

void Mac802154_setShortDestinationAddress(Mac802154 *self, uint16_t address) {
  self->setShortDestinationAddress(self, address);
}

void Mac802154_setPayload(Mac802154 *self, const char *payload, size_t payload_length) {
  self->setPayload(self, payload, payload_length);
}

void Mac802154_setExtendedDestinationAddress(Mac802154 *self, uint64_t address) {
  self->setExtendedDestinationAddress(self, address);
}

uint8_t Mac802154_getReceivedPacketSize(Mac802154 *self) {
  return self->getReceivedPacketSize(self);
}

bool Mac802154_newPacketAvailable(Mac802154 *self) {
  return self->newPacketAvailable(self);
}

void Mac802154_fetchPacketBlocking(Mac802154 *self, uint8_t *buffer, uint8_t size) {
  self->fetchPacketBlocking(self, buffer, size);
}

const char * Mac802154_getPacketPayload(Mac802154 *self, const uint8_t *packet) {
  return self->getPacketPayload(packet);
}

bool Mac802154_packetAddressIsShort(Mac802154 *self, const uint8_t *packet) {
  return self->packetAddressIsShort(packet);
}

bool Mac802154_packetAddressIsLong(Mac802154 *self, const uint8_t *packet) {
  return self->packetAddressIsExtended(packet);
}

uint8_t Mac802154_getPacketSourceAddressSize(Mac802154 *self, const uint8_t *packet) {
  return self->getPacketSourceAddressSize(packet);
}

uint64_t Mac802154_getPacketExtendedSourceAddress(const Mac802154 *self, const uint8_t *packet) {
  return self->getPacketExtendedSourceAddress(packet);
}

uint16_t
Mac802154_getPacketShortSourceAddress(const Mac802154 *self, const uint8_t *packet)
{
  return self->getPacketShortSourceAddress(packet);
}


uint8_t Mac802154_getPacketPayloadSize(Mac802154 *self, const uint8_t *packet) {
  return self->getPacketPayloadSize(packet);
}

void
Mac802154_enablePromiscuousMode(Mac802154 *self)
{
  self->enablePromiscuousMode(self);
}

void
Mac802154_disablePromiscuousMode(Mac802154 *self)
{
  self->disablePromiscuousMode(self);
}

void
Mac802154_useExtendedSourceAddress(Mac802154 *self)
{
  self->useExtendedSourceAddress(self);
}

void
Mac802154_useShortSourceAddress(Mac802154 *self)
{
  self->useShortSourceAddress(self);
}
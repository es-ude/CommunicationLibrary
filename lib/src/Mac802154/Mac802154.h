#ifndef COMMUNICATIONMODULE_MAC802154_H
#define COMMUNICATIONMODULE_MAC802154_H

#include "lib/include/Mac802154.h"

struct Mac802154 {
  void (*setShortDestinationAddress) (Mac802154 *self, uint16_t address);
  void (*setShortDestinationAddressFromArray) (Mac802154 *self, const uint8_t *address);
  void (*setExtendedDestinationAddress) (Mac802154 *self, uint64_t address);
  void (*setExtendedDestinationAddressFromArray) (Mac802154 *self, const uint8_t *address);
  void (*setPayload) (Mac802154 *self, const uint8_t* buffer, size_t size);

  void (*sendBlocking) (Mac802154 *self);
  void (*sendNonBlocking) (Mac802154 *self);
  void (*init) (Mac802154 *self, const Mac802154Config *config);
  void (*destroy) (Mac802154 *self);

  uint8_t (*getReceivedPacketSize) (Mac802154 *self);
  bool (*newPacketAvailable) (Mac802154 *self);
  void (*fetchPacketBlocking) (Mac802154 *self, uint8_t *buffer, uint8_t size);
  const uint8_t *(*getPacketPayload) (const uint8_t *packet);
  bool (*packetAddressIsShort) (const uint8_t *packet);
  bool (*packetAddressIsLong) (const uint8_t *packet);
  uint8_t (*getPacketSourceAddressSize) (const uint8_t *packet);
  const uint8_t *(*getPacketSourceAddress) (const uint8_t *packet);

  uint16_t (*getShortDestinationAddress) (Mac802154 *self);
  const uint8_t *(*getExtendedDestinationAddress) (Mac802154 *self);
  const uint8_t *(*getPayload) (Mac802154 *self);
  uint16_t (*getPayloadSize) (Mac802154 *self);
};

#endif //COMMUNICATIONMODULE_MAC802154_H

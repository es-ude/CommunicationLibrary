#ifndef COMMUNICATIONMODULE_MAC802154_H
#define COMMUNICATIONMODULE_MAC802154_H

#include "include/Mac802154.h"

struct Mac802154 {
  void (*setShortDestinationAddress)(Mac802154 self, const uint8_t *address);
  void (*setExtendedDestinationAddress)(Mac802154 self, const uint8_t *address);
  void (*setPayload)(Mac802154 self, const uint8_t *buffer, size_t size);
  void (*useExtendedSourceAddress) (Mac802154 self);
  void (*useShortSourceAddress) (Mac802154 self);

  void (*sendBlocking) (Mac802154 self);
  void (*sendNonBlocking) (Mac802154 self);
  void (*reconfigure) (Mac802154 self, const Mac802154Config *config);

  uint8_t (*getReceivedPacketSize) (Mac802154 self);
  bool (*newPacketAvailable) (Mac802154 self);
  void (*fetchPacketBlocking) (Mac802154 self, uint8_t *buffer, uint8_t size);
  const uint8_t *(*getPacketPayload) (const uint8_t *packet);
  uint8_t (*getPacketPayloadSize) (const uint8_t *packet);
  bool (*packetAddressIsShort) (const uint8_t *packet);
  bool (*packetAddressIsExtended) (const uint8_t *packet);
  uint8_t (*getPacketSourceAddressSize) (const uint8_t *packet);
  const uint8_t *(*getPacketExtendedSourceAddress) (const uint8_t *packet);
  const uint8_t *(*getPacketShortSourceAddress) (const uint8_t *packet);

  void (*enablePromiscuousMode) (Mac802154 self);
  void (*disablePromiscuousMode) (Mac802154 self);

};

#endif //COMMUNICATIONMODULE_MAC802154_H

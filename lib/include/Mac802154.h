#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/include/Message.h"

typedef struct Mac802154 Mac802154;
typedef struct Mac802154Config Mac802154Config;

struct Mac802154 {
  void (*setShortDestinationAddress) (Mac802154 *self, uint16_t address);
  void (*setExtendedDestinationAddress) (Mac802154 *self, uint8_t* address);
  void (*setPayload) (Mac802154 *self, uint8_t* buffer, size_t size);

  void (*associate) (Mac802154 *self);
  void (*receive) (Mac802154 *self, Message *received_msg);
  void (*send) (Mac802154 *self);
  void (*init) (Mac802154 *self, const Mac802154Config *config);
  void (*destroy) (Mac802154 *self);
  bool (*isAssociated) (Mac802154 *self);
  uint16_t (*getAddress16Bit) (Mac802154 *self);
  const uint8_t *(*getAddress64Bit) (Mac802154 *self);
  const uint8_t *(*getPayload) (Mac802154 *self);
  uint16_t (*getPayloadSize) (Mac802154 *self);
};

struct Mac802154Config {
  uint16_t short_source_address;
  uint8_t extended_source_address[8];
  uint16_t pan_id;
};

static inline void NetworkHardware_init(Mac802154 *hardware,
                                        const Mac802154Config *config) {
  hardware->init(hardware, config);
}


#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H

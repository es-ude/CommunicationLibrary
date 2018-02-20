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

  void (*receive) (Mac802154 *self, Message *received_msg);
  void (*send) (Mac802154 *self);
  void (*init) (Mac802154 *self, const Mac802154Config *config);
  void (*destroy) (Mac802154 *self);

  uint16_t (*getShortDestinationAddress) (Mac802154 *self);
  const uint8_t *(*getExtendedDestinationAddress) (Mac802154 *self);
  const uint8_t *(*getPayload) (Mac802154 *self);
  uint16_t (*getPayloadSize) (Mac802154 *self);
};

struct Mac802154Config {
  uint16_t short_source_address;
  uint8_t extended_source_address[8];
  uint16_t pan_id;
};

static inline void Mac802154_init(Mac802154 *hardware,
                                  const Mac802154Config *config) {
  hardware->init(hardware, config);
}

static inline void Mac802154_send(Mac802154 *hardware) {
  hardware->send(hardware);
}

typedef struct FrameControlField802154 {
  unsigned frame_type : 3;
  unsigned security_enabled : 1;
  unsigned frame_pending : 1;
  unsigned acknowledgment_request : 1;
  unsigned information_element_present : 1;
  unsigned pan_id_compression : 1;
  unsigned reserved : 1;
  unsigned sequence_number_suppression : 1;
  unsigned destination_addressing_mode : 2;
  unsigned frame_version : 2;
  unsigned source_addressing_mode : 2;
} FrameControlField802154;

typedef struct FrameHeader802154 {
  union {
    struct {
      uint8_t first;
      uint8_t second;
    } as_byte;
    FrameControlField802154 as_struct;
  } control;
  uint8_t sequence_number;
  uint8_t destination_pan_id[2];
  union {
    uint8_t short_address[2];
    uint8_t long_address[8];
  } destination;
} FrameHeader802154;

#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H

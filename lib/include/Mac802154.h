#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/include/Peripheral.h"

typedef struct Mac802154 Mac802154;
typedef struct Mac802154Config Mac802154Config;

struct Mac802154 {
  void (*setShortDestinationAddress) (Mac802154 *self, uint16_t address);
  void (*setExtendedDestinationAddress) (Mac802154 *self, uint64_t address);
  void (*setPayload) (Mac802154 *self, const uint8_t* buffer, size_t size);

  void (*sendBlocking) (Mac802154 *self);
  void (*sendNonBlocking) (Mac802154 *self);
  void (*init) (Mac802154 *self, const Mac802154Config *config);
  void (*destroy) (Mac802154 *self);

  uint16_t (*getShortDestinationAddress) (Mac802154 *self);
  const uint8_t *(*getExtendedDestinationAddress) (Mac802154 *self);
  const uint8_t *(*getPayload) (Mac802154 *self);
  uint16_t (*getPayloadSize) (Mac802154 *self);
};

enum {
  FRAME_TYPE_BEACON = 0,
  FRAME_TYPE_DATA = 1,
  FRAME_TYPE_ACKNOWLEDGEMENT = 2,
  FRAME_TYPE_MAC_COMMAND = 3,
  ADDRESSING_MODE_NEITHER_PAN_NOR_ADDRESS_PRESENT = 0,
  ADDRESSING_MODE_SHORT_ADDRESS = 0b10,
  ADDRESSING_MODE_EXTENDED_ADDRESS = 0b11,
  FRAME_VERSION_2015 = 0b10,
  FRAME_VERSION_2003 = 0b00,
  FRAME_VERSION_2006 = 0b01,
};

/**
 * both addresses and the pan id are to be set in ascending byte order (big endian)
 */
struct Mac802154Config {
  uint16_t short_source_address;
  uint64_t extended_source_address;
  uint16_t pan_id;
  uint8_t channel;
  PeripheralInterface *interface;
  Peripheral *device;
};

void Mac802154_init(Mac802154 *hardware,
                                  const Mac802154Config *config);

void Mac802154_sendBlocking(Mac802154 *hardware);

void Mac802154_destroy(Mac802154 *self);

void Mac802154_setShortDestinationAddress(Mac802154 *self, uint16_t address);
void Mac802154_setExtendedDestinationAddress(Mac802154 *self, uint64_t address);
void Mac802154_setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length);

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

#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H

#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/include/Peripheral.h"

typedef struct Mac802154 Mac802154;
typedef struct Mac802154Config Mac802154Config;

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

struct Mac802154Config {
  uint16_t short_source_address;
  uint64_t extended_source_address;
  uint16_t pan_id;
  uint8_t channel;
  PeripheralInterface interface;
  Peripheral *device;
};

void Mac802154_init(Mac802154 *hardware,
                                  const Mac802154Config *config);

void Mac802154_sendBlocking(Mac802154 *hardware);

void Mac802154_destroy(Mac802154 *self);

void Mac802154_setShortDestinationAddress(Mac802154 *self, uint16_t address);
void Mac802154_setShortDestinationAddressFromArray(Mac802154 *self, const uint8_t *address);

// sets address in big endian representation suitable for network transmission
void Mac802154_setExtendedDestinationAddress(Mac802154 *self, uint64_t address);

// use a one-to-one copy of the specified address as destination address
void Mac802154_setExtendedDestinationAddressFromArray(Mac802154 *self, const uint8_t *address);
void Mac802154_setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length);
uint8_t Mac802154_getReceivedPacketSize(Mac802154 *self);
bool Mac802154_newPacketAvailable(Mac802154 *self);
void Mac802154_fetchPacketBlocking(Mac802154 *self, uint8_t *buffer, uint8_t size);
const uint8_t *Mac802154_getPacketPayload(Mac802154 *self, const uint8_t *packet);
bool Mac802154_packetAddressIsShort(Mac802154 *self, const uint8_t *packet);
bool Mac802154_packetAddressIsLong(Mac802154 *self, const uint8_t *packet);
uint8_t Mac802154_getPacketSourceAddressSize(Mac802154 *self, const uint8_t *packet);
const uint8_t *Mac802154_getPacketSourceAddress(Mac802154 *self, const uint8_t *packet);

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

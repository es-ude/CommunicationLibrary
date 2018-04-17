#include <string.h>
#include "lib/src/BitManipulation.h"
#include "lib/include/Mac802154.h"
#include "lib/src/Mac802154/FrameHeader802154.h"
#include "lib/include/Exception.h"


/**
 * The memory layout of a mac header control field for 802.15.4 looks like this
 *
 * typedef struct FrameControlField802154 {
 * unsigned frame_type : 3;
 * unsigned security_enabled : 1;
 * unsigned frame_pending : 1;
  unsigned acknowledgment_request : 1;
  unsigned information_element_present : 1;
  unsigned pan_id_compression : 1;
  unsigned reserved : 1;
  unsigned sequence_number_suppression : 1;
  unsigned destination_addressing_mode : 2;
  unsigned frame_version : 2;
  unsigned source_addressing_mode : 2;
} FrameControlField802154;

 * Due to different alignment of these fields on different platforms and
 * because this header needs to be transferred bytewise to the network module,
 * we actually use a byte array to store the data.
 * Below the offsets and bitmasks for each field are listed. Note that the control field
 * of the header consists of two bytes.
 */

// first byte
static const uint8_t frame_type_bitmask = 0b111;
static const uint8_t security_enabled_offset = 3;
static const uint8_t frame_pending_offset = 4;
static const uint8_t acknowledgement_request_offset = 5;
static const uint8_t pan_id_compression_offset = 6;

// second byte
static const uint8_t sequence_number_suppression_offset = 8;
static const uint8_t information_element_present_offset = 9;
static const uint8_t destination_addressing_mode_bitmask = 0b11;
static const uint8_t destination_addressing_mode_offset = 11;
static const uint8_t frame_version_offset = 12;
static const uint8_t frame_version_bitmask = 0b11;
static const uint8_t source_addressing_mode_bitmask = 0b11;
static const uint8_t source_addressing_mode_offset = 14;

/*
 * after the second byte follow
 *  - sequence number either zero or one byte (as stated by sequence_number_suppression)
 *  - destination pan id (we'll always omit the source pan id, and set up the frame in a way that source and
 *    destination pan_id have to be the same.
 *  - destination address either in 16 (short) or 64 bit (extended) format
 *  - source address (generally this can be short or extended format as well.
 *    However if both addresses are used in the extended form, the pan id compression has to be disabled, but the source pan id is still omitted.
 */

const FrameHeader802154 *Mac802154_defaultHeader(void) {
  static FrameHeader802154 header = {
          .control = {
                  .frame_type = 0b001,
                  .security_enabled = 0b0,
                  .frame_pending = 0b0,
                  .acknowledgment_request = 0b0,
                  .pan_id_compression = 0b1,
                  .reserved = 0b0,
                  .sequence_number_suppression = 0b0,
                  .information_element_present = 0b0,
                  .destination_addressing_mode = 0b10,
                  .frame_version = 0b10,
                  .source_addressing_mode = 0b10,
          },
          .destination_address = 0x00,
          .destination_pan_id = 0xffff,
          .sequence_number = 0,
  };
  return &header;
}

typedef struct FrameHeader802154_t {
  uint8_t data[21];
} FrameHeader802154_t;

static bool sourceAddressIsPresent(const FrameHeader802154_t *self);
static bool destinationAddressIsPresent(const FrameHeader802154_t *self);
static bool sequenceNumberIsPresent(const FrameHeader802154_t *self);
static void setFrameType(FrameHeader802154_t *self, uint8_t frame_type);
static void setFrameVersion(FrameHeader802154_t *self, uint8_t frame_type);
static void enablePanIdCompression(FrameHeader802154_t *self);
static void disablePanIdCompression(FrameHeader802154_t *self);
static bool panIdCompressionIsEnabled(const FrameHeader802154_t *self);
static void enableSecurity(FrameHeader802154_t *self);
static void disableSecurity(FrameHeader802154_t *self);
static void enableFramePending(FrameHeader802154_t *self);
static void disableFramePending(FrameHeader802154_t *self);
static void enableAcknowledgementRequest(FrameHeader802154_t *self);
static void disableAcknowledgementRequest(FrameHeader802154_t *self);
static void setSourceAddressingMode(FrameHeader802154_t *self, uint8_t mode);
static void setDestinationAddressingMode(FrameHeader802154_t *self, uint8_t mode);
static uint8_t getSourceAddressingMode(const FrameHeader802154_t *self);
static uint8_t getDestinationAddressingMode(const FrameHeader802154_t *self);
static bool panIdIsPresent(const FrameHeader802154_t *self);
static uint8_t getAddressSize(uint8_t addressing_mode);

void FrameHeader802154_init(FrameHeader802154_t *self) {
  memset(self->data, 0, MAXIMUM_HEADER_SIZE);
  enableSequenceNumberSuppression(self);
}

const uint8_t *FrameHeader802154_getSequenceNumberPtr(FrameHeader802154_t *self) {
  return self->data + 2;
}

void FrameHeader802154_setSequenceNumber(FrameHeader802154_t *self, uint8_t number) {
  disableSequenceNumberSuppression(self);
  self->data[2] = number;
}

const uint8_t *FrameHeader802154_getPanIdPtr(FrameHeader802154_t *self) {
  return self->data + 3;
}

void FrameHeader802154_setPanId(FrameHeader802154_t *self, uint16_t pan_id) {
  if (!sourceAddressIsPresent(self) && !destinationAddressIsPresent(self)) {
    enablePanIdCompression(self);
  }
  if (destinationAddressIsPresent(self) && !sourceAddressIsPresent(self)) {
    disablePanIdCompression(self);
  }
  if (!destinationAddressIsPresent(self) && sourceAddressIsPresent(self)) {
    Throw(FRAMEHEADER802154_INVALID_EXCEPTION);
  }
  self->data[3] = (uint8_t) (pan_id >> 8);
  self->data[4] = (uint8_t) pan_id;
}

uint8_t FrameHeader802154_getHeaderSize(FrameHeader802154_t *self) {
  uint8_t base_size = 2;
  if (sequenceNumberIsPresent(self)) {
    base_size++;
  }
  if (panIdIsPresent(self)) {
    base_size += sizeof(uint16_t);
  }
  base_size += getAddressSize(getSourceAddressingMode(self));
  base_size += getAddressSize(getDestinationAddressingMode(self));
  return base_size;
}

void FrameHeader802154_setExtendedSourceAddress(FrameHeader802154_t *self, uint64_t address) {
  if (!panIdIsPresent(self) && getDestinationAddressingMode(self) == ADDRESSING_MODE_SHORT_ADDRESS
      || panIdIsPresent(self) && !destinationAddressIsPresent(self)) {
    Throw(FRAMEHEADER802154_INVALID_EXCEPTION);
  }
  setSourceAddressingMode(self, ADDRESSING_MODE_EXTENDED_ADDRESS);
}

void FrameHeader802154_setShortSourceAddress(FrameHeader802154_t *self, uint16_t address) {
  if (!panIdIsPresent(self) && !destinationAddressIsPresent(self)){
    enablePanIdCompression(self);
  }
  else if ((panIdIsPresent(self) && !destinationAddressIsPresent(self))
          || (!panIdIsPresent(self) && destinationAddressIsPresent(self))) {
    Throw(FRAMEHEADER802154_INVALID_EXCEPTION);
  }
  setSourceAddressingMode(self, ADDRESSING_MODE_SHORT_ADDRESS);
}

void FrameHeader802154_setExtendedDestinationAddress(FrameHeader802154_t *self, uint64_t address) {
  if (panIdIsPresent(self) && !sourceAddressIsPresent(self)) {
    disablePanIdCompression(self);
  }
    else if(!panIdIsPresent(self) && !sourceAddressIsPresent(self) ||
          panIdIsPresent(self) && sourceAddressIsPresent(self)) {
    enablePanIdCompression(self);
  }
  else if (!panIdIsPresent(self) && getSourceAddressingMode(self) == ADDRESSING_MODE_SHORT_ADDRESS) {
    Throw(FRAMEHEADER802154_INVALID_EXCEPTION);
  }
  setDestinationAddressingMode(self, ADDRESSING_MODE_EXTENDED_ADDRESS);
}

void FrameHeader802154_setShortDestinationAddress(FrameHeader802154_t *self, uint16_t address) {
  if (!panIdIsPresent(self) && !sourceAddressIsPresent(self)) {
    enablePanIdCompression(self);
  }
  else if (panIdIsPresent(self) && !sourceAddressIsPresent(self)) {
    disablePanIdCompression(self);
  }
  else if (!panIdIsPresent(self) && sourceAddressIsPresent(self)) {
    Throw(FRAMEHEADER802154_INVALID_EXCEPTION);
  }
  setDestinationAddressingMode(self, ADDRESSING_MODE_SHORT_ADDRESS);
}

uint8_t getAddressSize(uint8_t addressing_mode) {
  switch (addressing_mode) {
    case ADDRESSING_MODE_EXTENDED_ADDRESS:
      return 8;
    case ADDRESSING_MODE_SHORT_ADDRESS:
      return 2;
    case ADDRESSING_MODE_NEITHER_PAN_NOR_ADDRESS_PRESENT:
    default:
      return 0;
  }
}

static bool panIdIsPresent(const FrameHeader802154_t *self) {
  if (!sourceAddressIsPresent(self) && !destinationAddressIsPresent(self)) {
    return panIdCompressionIsEnabled(self);
  }
  if (!destinationAddressIsPresent(self) && sourceAddressIsPresent(self)) {
    return false;
  }
  if (destinationAddressIsPresent(self) && !sourceAddressIsPresent(self)) {
    return !panIdCompressionIsEnabled(self);
  }
  return false;
}

static bool sourceAddressIsPresent(const FrameHeader802154_t *self) {
  return getSourceAddressingMode(self) != ADDRESSING_MODE_NEITHER_PAN_NOR_ADDRESS_PRESENT;
}

static bool destinationAddressIsPresent(const FrameHeader802154_t *self) {
  return getDestinationAddressingMode(self) != ADDRESSING_MODE_NEITHER_PAN_NOR_ADDRESS_PRESENT;
}


uint8_t getSourceAddressingMode(const FrameHeader802154_t *self) {
  return BitManipulation_getByte(self->data, source_addressing_mode_bitmask, source_addressing_mode_offset);
}

void enablePanIdCompression(FrameHeader802154_t *self) {
  BitManipulation_setBit(self->data, pan_id_compression_offset);
}

void disablePanIdCompression(FrameHeader802154_t *self) {
  BitManipulation_clearBit(self->data, pan_id_compression_offset);
}

bool panIdCompressionIsEnabled(const FrameHeader802154_t *self) {
  return BitManipulation_bitIsSet(self->data, pan_id_compression_offset);
}

void enableSequenceNumberSuppression(FrameHeader802154_t *self) {
  BitManipulation_setBit(self->data, sequence_number_suppression_offset);
}

void disableSequenceNumberSuppression(FrameHeader802154_t *self) {
  BitManipulation_clearBit(self->data, sequence_number_suppression_offset);
}

bool sequenceNumberIsPresent(const FrameHeader802154_t *self) {
  return !BitManipulation_bitIsSet(self->data, sequence_number_suppression_offset);
}

void setFrameType(FrameHeader802154_t *self, uint8_t frame_type) {
  BitManipulation_setByte(self->data, frame_type_bitmask, 0, frame_type);
}

void setDestinationAddressingMode(FrameHeader802154_t *self, uint8_t mode) {
  BitManipulation_setByte(self->data, destination_addressing_mode_bitmask, destination_addressing_mode_offset, mode);
}

uint8_t getDestinationAddressingMode(const FrameHeader802154_t *self) {
  BitManipulation_getByte(self->data, destination_addressing_mode_bitmask, destination_addressing_mode_offset);
}

void setFrameVersion(FrameHeader802154_t *self, uint8_t version) {
  BitManipulation_setByte(self->data, frame_version_bitmask, frame_version_offset, version);
}

void setSourceAddressingMode(FrameHeader802154_t *self, uint8_t mode) {
  BitManipulation_setByte(self->data, source_addressing_mode_bitmask, source_addressing_mode_offset, mode);
}


void enableSecurity(FrameHeader802154_t *self){
  BitManipulation_setBit(self->data, security_enabled_offset);
}

void disableSecurity(FrameHeader802154_t *self) {
  BitManipulation_clearBit(self->data, security_enabled_offset);
}

void enableFramePending(FrameHeader802154_t *self) {
  BitManipulation_setBit(self->data, frame_pending_offset);
}

void disableFramePending(FrameHeader802154_t *self) {
  BitManipulation_clearBit(self->data, frame_pending_offset);
}

void enableAcknowledgementRequest(FrameHeader802154_t *self){
  BitManipulation_setBit(self->data, acknowledgement_request_offset);
}

void enableInformationElementPresent(FrameHeader802154_t *self) {
  BitManipulation_setBit(self->data, information_element_present_offset);
}

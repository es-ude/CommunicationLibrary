#include "src/Util/BitManipulation.h"
#include "CommunicationModule/Mac802154.h"
#include "src/Mac802154/MRF/FrameHeader802154.h"


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
static const uint8_t destination_addressing_mode_offset = 10;
static const uint8_t frame_version_offset = 12;
static const uint8_t frame_version_bitmask = 0b11;
static const uint8_t source_addressing_mode_bitmask = 0b11;
static const uint8_t source_addressing_mode_offset = 14;

// other constants
static const uint8_t control_field_size = 2;
static const uint8_t sequence_number_size = 1;
static const uint8_t pan_id_size = 2;
/*
 * after the second byte follow
 *  - sequence number either zero or one byte (as stated by sequence_number_suppression)
 *  - destination pan id (we'll always omit the source pan id, and set up the frame in a way that source and
 *    destination pan_id have to be the same.
 *  - destination address either in 16 (short) or 64 bit (extended) format
 *  - source address (generally this can be short or extended format as well.
 *    However if both addresses are used in the extended form, the pan id compression has to be disabled, but the source pan id is still omitted.
 */



static bool sourceAddressIsPresent(const FrameHeader802154 *self);
static bool destinationAddressIsPresent(const FrameHeader802154 *self);
static bool sequenceNumberIsPresent(const FrameHeader802154 *self);
static bool panIdIsPresent(const FrameHeader802154 *self);
static bool panIdCompressionIsEnabled(const FrameHeader802154 *self);
static bool acknowledgmentIsEnabled(const FrameHeader802154 *self);

static void setFrameType(FrameHeader802154 *self, uint8_t frame_type);
static void setFrameVersion(FrameHeader802154 *self, uint8_t frame_type);
static void enablePanIdCompression(FrameHeader802154 *self);
static void disablePanIdCompression(FrameHeader802154 *self);
static void enableSecurity(FrameHeader802154 *self);
static void disableSecurity(FrameHeader802154 *self);
static void enableFramePending(FrameHeader802154 *self);
static void disableFramePending(FrameHeader802154 *self);
static void disableAcknowledgementRequest(FrameHeader802154 *self);
static void setSourceAddressingMode(FrameHeader802154 *self, uint8_t mode);
static void setDestinationAddressingMode(FrameHeader802154 *self, uint8_t mode);

static uint8_t getSourceAddressingMode(const FrameHeader802154 *self);
static uint8_t getDestinationAddressingMode(const FrameHeader802154 *self);
static uint8_t getAddressSize(uint8_t addressing_mode);
static uint8_t getPanIdOffset(const FrameHeader802154 *self);

static void moveSourceAddress(FrameHeader802154 *self, int8_t distance);
static void movePanId(FrameHeader802154 *self, int8_t distance);
static void moveDestinationAddress(FrameHeader802154 *Self, int8_t distance);

void FrameHeader802154_init(FrameHeader802154 *self) {
  for (uint8_t i = 0; i < MAXIMUM_HEADER_SIZE; i++) {
    self->data[i] = 0;
  }
  enablePanIdCompression(self);
  setDestinationAddressingMode(self, ADDRESSING_MODE_SHORT_ADDRESS);
  setSourceAddressingMode(self, ADDRESSING_MODE_SHORT_ADDRESS);
  setFrameType(self, FRAME_TYPE_DATA);
  setFrameVersion(self, FRAME_VERSION_2015);

}

const uint8_t *FrameHeader802154_getHeaderPtr(const FrameHeader802154 *self) {
  return self->data;
}

const uint8_t *FrameHeader802154_getSequenceNumberPtr(const FrameHeader802154 *self) {
  return self->data + control_field_size;
}

void FrameHeader802154_setSequenceNumber(FrameHeader802154 *self, uint8_t number) {
  movePanId(self, sizeof(number));
  FrameHeader802154_disableSequenceNumberSuppression(self);
  self->data[2] = number;
}

uint8_t FrameHeader802154_getSequenceNumberSize(const FrameHeader802154 *self) {
  if (sequenceNumberIsPresent(self))
  {
    return 1;
  }
  else {
    return 0;
  }
}

const uint8_t *FrameHeader802154_getPanIdPtr(const FrameHeader802154 *self) {
  return self->data + getPanIdOffset(self);
}

void FrameHeader802154_setPanId(FrameHeader802154 *self, const uint8_t *pan_id) {
  self->data[getPanIdOffset(self)] = pan_id[0];
  self->data[getPanIdOffset(self) + 1] = pan_id[1];
}

uint8_t FrameHeader802154_getHeaderSize(FrameHeader802154 *self) {
  uint8_t base_size = 2;
  if (sequenceNumberIsPresent(self)) {
    base_size++;
  }
  base_size += FrameHeader802154_getPanIdSize(self);
  base_size += getAddressSize(getSourceAddressingMode(self));
  base_size += getAddressSize(getDestinationAddressingMode(self));
  return base_size;
}

void FrameHeader802154_setExtendedSourceAddress(FrameHeader802154 *self, const uint8_t *address) {
  uint8_t *source_address = self->data + FrameHeader802154_getSourceAddressOffset(self);
  BitManipulation_copyBytes(address, source_address, 8);
  if (getDestinationAddressingMode(self) == ADDRESSING_MODE_EXTENDED_ADDRESS)
  {
    disablePanIdCompression(self);
  }
  setSourceAddressingMode(self, ADDRESSING_MODE_EXTENDED_ADDRESS);
}

void FrameHeader802154_setShortSourceAddress(FrameHeader802154 *self, const uint8_t *address) {
  uint8_t *source_address = self->data + FrameHeader802154_getSourceAddressOffset(self);
  BitManipulation_copyBytes(address, source_address, 2);
  enablePanIdCompression(self);
  setSourceAddressingMode(self, ADDRESSING_MODE_SHORT_ADDRESS);
}



void FrameHeader802154_setExtendedDestinationAddress(FrameHeader802154 *self, const uint8_t *address) {
  uint8_t *destination_address_ptr = self->data + FrameHeader802154_getDestinationAddressOffset(self);
  if (getDestinationAddressingMode(self) == ADDRESSING_MODE_SHORT_ADDRESS)
  {
    moveSourceAddress(self, getAddressSize(ADDRESSING_MODE_EXTENDED_ADDRESS) - getAddressSize(ADDRESSING_MODE_SHORT_ADDRESS));
  }
  if (getSourceAddressingMode(self) == ADDRESSING_MODE_EXTENDED_ADDRESS)
  {
    disablePanIdCompression(self);
  }
  BitManipulation_copyBytes(address, destination_address_ptr, sizeof(uint64_t));
  setDestinationAddressingMode(self, ADDRESSING_MODE_EXTENDED_ADDRESS);
}

void FrameHeader802154_setShortDestinationAddress(FrameHeader802154 *self, const uint8_t *address) {
  if (getDestinationAddressingMode(self) == ADDRESSING_MODE_EXTENDED_ADDRESS)
  {
    moveSourceAddress(self, getAddressSize(ADDRESSING_MODE_SHORT_ADDRESS) - getAddressSize(ADDRESSING_MODE_EXTENDED_ADDRESS));
  }
  uint8_t *destination_address_ptr = self->data + FrameHeader802154_getDestinationAddressOffset(self);
  enablePanIdCompression(self);
  BitManipulation_copyBytes(address, destination_address_ptr, 2);
  setDestinationAddressingMode(self, ADDRESSING_MODE_SHORT_ADDRESS);
}

uint8_t FrameHeader802154_getSourceAddressSize(const FrameHeader802154 *self){
  return getAddressSize(getSourceAddressingMode(self));
}

uint8_t FrameHeader802154_getDestinationAddressSize(const FrameHeader802154 *self) {
  return getAddressSize(getDestinationAddressingMode(self));
}

uint8_t FrameHeader802154_getPanIdSize(const FrameHeader802154 *self) {
  if (panIdIsPresent(self)) return 2;
  return 0;
}

const uint8_t *FrameHeader802154_getDestinationAddressPtr(const FrameHeader802154 *self) {
  return self->data + FrameHeader802154_getDestinationAddressOffset(self);
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

static bool panIdIsPresent(const FrameHeader802154 *self) {
  return (getSourceAddressingMode(self) == ADDRESSING_MODE_EXTENDED_ADDRESS
          && getDestinationAddressingMode(self) == ADDRESSING_MODE_EXTENDED_ADDRESS )
          || panIdCompressionIsEnabled(self);
}

static bool sourceAddressIsPresent(const FrameHeader802154 *self) {
  return getSourceAddressingMode(self) != ADDRESSING_MODE_NEITHER_PAN_NOR_ADDRESS_PRESENT;
}

static bool destinationAddressIsPresent(const FrameHeader802154 *self) {
  return getDestinationAddressingMode(self) != ADDRESSING_MODE_NEITHER_PAN_NOR_ADDRESS_PRESENT;
}


uint8_t getSourceAddressingMode(const FrameHeader802154 *self) {
  return BitManipulation_getByteOnArray(self->data, source_addressing_mode_bitmask, source_addressing_mode_offset);
}

void enablePanIdCompression(FrameHeader802154 *self) {
  BitManipulation_setBitOnArray(self->data, pan_id_compression_offset);
}

void disablePanIdCompression(FrameHeader802154 *self) {
  BitManipulation_clearBitOnArray(self->data, pan_id_compression_offset);
}

bool panIdCompressionIsEnabled(const FrameHeader802154 *self) {
  return BitManipulation_bitIsSetOnArray(self->data, pan_id_compression_offset);
}

void FrameHeader802154_enableSequenceNumberSuppression(FrameHeader802154 *self) {
  BitManipulation_setBitOnArray(self->data, sequence_number_suppression_offset);
}

void FrameHeader802154_disableSequenceNumberSuppression(FrameHeader802154 *self) {
  BitManipulation_clearBitOnArray(self->data, sequence_number_suppression_offset);
}

bool sequenceNumberIsPresent(const FrameHeader802154 *self) {
  return !BitManipulation_bitIsSetOnArray(self->data, sequence_number_suppression_offset);
}

void setFrameType(FrameHeader802154 *self, uint8_t frame_type) {
  BitManipulation_setByteOnArray(self->data, frame_type_bitmask, 0, frame_type);
}

void setDestinationAddressingMode(FrameHeader802154 *self, uint8_t mode) {
  BitManipulation_setByteOnArray(self->data, destination_addressing_mode_bitmask, destination_addressing_mode_offset,
                                 mode);
}

uint8_t getDestinationAddressingMode(const FrameHeader802154 *self) {
  return BitManipulation_getByteOnArray(self->data, destination_addressing_mode_bitmask,
                                        destination_addressing_mode_offset);
}

void setFrameVersion(FrameHeader802154 *self, uint8_t version) {
  BitManipulation_setByteOnArray(self->data, frame_version_bitmask, frame_version_offset, version);
}

void setSourceAddressingMode(FrameHeader802154 *self, uint8_t mode) {
  BitManipulation_setByteOnArray(self->data, source_addressing_mode_bitmask, source_addressing_mode_offset, mode);
}


void enableSecurity(FrameHeader802154 *self){
  BitManipulation_setBitOnArray(self->data, security_enabled_offset);
}

void disableSecurity(FrameHeader802154 *self) {
  BitManipulation_clearBitOnArray(self->data, security_enabled_offset);
}

void enableFramePending(FrameHeader802154 *self) {
  BitManipulation_setBitOnArray(self->data, frame_pending_offset);
}

void disableFramePending(FrameHeader802154 *self) {
  BitManipulation_clearBitOnArray(self->data, frame_pending_offset);
}

void FrameHeader802154_enableAcknowledgementRequest(FrameHeader802154 *self){
  BitManipulation_setBitOnArray(self->data, acknowledgement_request_offset);
}

void enableInformationElementPresent(FrameHeader802154 *self) {
  BitManipulation_setBitOnArray(self->data, information_element_present_offset);
}

uint8_t FrameHeader802154_getDestinationAddressOffset(const FrameHeader802154 *self) {
  uint8_t offset = getPanIdOffset(self);
  offset += pan_id_size;
  return offset;
}

uint8_t FrameHeader802154_getSourceAddressOffset(const FrameHeader802154 *self) {
  uint8_t offset = FrameHeader802154_getDestinationAddressOffset(self);
  offset += FrameHeader802154_getDestinationAddressSize(self);
  return offset;
}

uint8_t getPanIdOffset(const FrameHeader802154 *self) {
  uint8_t offset = control_field_size;
  offset += FrameHeader802154_getSequenceNumberSize(self);
  return offset;
}

void moveLeft(uint8_t *source, uint8_t number_of_elements, int8_t distance) {
  for (int8_t index = 0; index < number_of_elements; index++)
  {
    *(source + distance) = *source;
    source++;
  }
}

void moveRight(uint8_t *source, uint8_t number_of_elements, uint8_t distance) {
  source += number_of_elements-1;
  for (int8_t index = number_of_elements-1; index >= 0; index--)
  {
    *(source + distance) = *(source);
    source--;
  }
}

void moveSourceAddress(FrameHeader802154 *self, int8_t distance) {
  uint8_t *source_address_ptr = self->data + FrameHeader802154_getSourceAddressOffset(self);
  uint8_t size = getAddressSize(getSourceAddressingMode(self));
  if(distance > 0)
  {
    moveRight(source_address_ptr, size, distance);
  }
  else {
    moveLeft(source_address_ptr, size, distance);
  }
}

void movePanId(FrameHeader802154 *self, int8_t distance) {
  uint8_t *pan_id_ptr = self->data + getPanIdOffset(self);
  moveDestinationAddress(self, distance);
  moveRight(pan_id_ptr, sizeof(uint16_t), distance);
}

void moveDestinationAddress(FrameHeader802154 *self, int8_t distance) {
  uint8_t *address_ptr = self->data + FrameHeader802154_getDestinationAddressOffset(self);
  moveSourceAddress(self, distance);
  moveRight(address_ptr, 8, distance);
}

const uint8_t *FrameHeader802154_getSourceAddressPtr(const FrameHeader802154 *self) {
    return self->data + FrameHeader802154_getSourceAddressOffset(self);
}

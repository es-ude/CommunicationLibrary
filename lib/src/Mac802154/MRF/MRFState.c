#include "MRFState.h"

extern void debug(uint8_t *string);

void MrfState_init(MrfState *mrf) {
  FrameHeader802154_init(&mrf->header.frame_header);
  mrf->header.frame_header_length = FrameHeader802154_getHeaderSize(&mrf->header.frame_header);
  mrf->header.frame_length = mrf->header.frame_header_length;
}

void MrfState_setShortDestinationAddress(MrfState *mrf, uint16_t address) {
  FrameHeader802154_setShortDestinationAddress(&mrf->header.frame_header, address);
  mrf->header.frame_header_length = FrameHeader802154_getHeaderSize(&mrf->header.frame_header);
  mrf->state = 1;
}

void MrfState_setExtendedDestinationAddress(MrfState *mrf, uint64_t address) {
  FrameHeader802154_setExtendedDestinationAddress(&mrf->header.frame_header, address);
  mrf->header.frame_header_length = FrameHeader802154_getHeaderSize(&mrf->header.frame_header);
}

void MrfState_setShortSourceAddress(MrfState *mrf, uint16_t address) {
  mrf->header.frame_header_length = FrameHeader802154_getHeaderSize(&mrf->header.frame_header);
  FrameHeader802154_setShortSourceAddress(&mrf->header.frame_header, address);
}

void MrfState_setPayload(MrfState *mrf, const uint8_t *payload, uint8_t payload_length){
  mrf->header.frame_length = payload_length + mrf->header.frame_header_length;
  mrf->payload = payload;
}

uint8_t MrfState_getPayloadLength(MrfState *mrf) {
  return mrf->header.frame_length - mrf->header.frame_header_length;
}

void MrfState_setPanId(MrfState *mrf, uint16_t pan_id) {
  FrameHeader802154_setPanId(&mrf->header.frame_header, pan_id);
}

uint8_t MrfState_getFullHeaderLength(MrfState *mrf) {
  return (uint8_t)(FrameHeader802154_getHeaderSize(&mrf->header.frame_header) + 2);
}

const uint8_t *MrfState_getFullHeaderData(MrfState *mrf) {
  return (uint8_t *) &mrf->header;
}

const uint8_t *MrfState_getPayload(MrfState *mrf) {
  return mrf->payload;
}

bool MrfState_nextField(MrfState *mrf) {
  if (mrf->state != 0)
  {
    mrf->state = 0;
    return true;
  }
}

MrfField MrfState_getFullHeaderField(MrfState *mrf) {
  MrfField field = {
          .address = 0,
          .data = MrfState_getFullHeaderData(mrf),
          .size = MrfState_getFullHeaderLength(mrf),
  };
  return field;
}

MrfField MrfState_getPayloadField(MrfState *self) {
  MrfField field = {
          .data = MrfState_getPayload(self),
          .size = MrfState_getPayloadLength(self),
          .address = MrfState_getFullHeaderLength(self),
  };
  return field;
}
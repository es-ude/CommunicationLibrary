#include "MRFState.h"

void MrfState_setShortDestinationAddress(MrfState *mrf) {
  mrf->state = 1;
}

void MrfState_setPayload(MrfState *mrf, const uint8_t *payload, uint8_t payload_length){
  mrf->payload_length = payload_length;
}

uint8_t MrfState_getFullDataLength(MrfState *mrf) {
  return FrameHeader802154_getHeaderSize(&mrf->header.frame_header) + 2;
}

const uint8_t *MrfState_getFullHeaderData(MrfState *mrf) {
  mrf->header.frame_header_length =
          FrameHeader802154_getHeaderSize(&mrf->header.frame_header);
  mrf->header.frame_length = mrf->payload_length + mrf->header.frame_header_length;
  return (uint8_t *) &mrf->header;
}

bool MrfState_nextField(MrfState *mrf) {
  if (mrf->state != 0)
  {
    mrf->state = 0;
    return true;
  }
}
#ifndef COMMUNICATIONMODULE_MRFSTATE_H
#define COMMUNICATIONMODULE_MRFSTATE_H

#include "lib/src/Mac802154/MRF/MrfIo.h"
#include "lib/src/Mac802154/FrameHeader802154.h"

/**
 * This module manages the data, that needs to be
 * written to the mrf chip in order to transmit
 * a message. For each field to be updated it
 * returns the destination memory address, the
 * size and the pointer to the actual data.
 * All changes made to fields through this module
 * will result in the corresponding data to be
 * included in the set of fields returned by
 * getNextField(). The module yields all data
 * that needs to be updated on the mrf due to
 * field changes.
 */

typedef struct MrfHeader {
  uint8_t frame_header_length;
  uint8_t frame_length;
  FrameHeader802154 frame_header;
} MrfHeader;

typedef struct MrfField {
  uint8_t size;
  uint16_t address;
  const uint8_t *data;
} MrfField;

typedef struct MrfState {
  uint8_t state;
  MrfHeader header;
  const uint8_t *payload;
  uint8_t payload_length;
} MrfState;

void MrfState_setLongDestinationAddress(MrfState *mrf);
void MrfState_setShortDestinationAddress(MrfState *mrf);
void MrfState_setPayload(MrfState *mrf, const uint8_t *payload, uint8_t payload_length);
void MrfState_setSequenceNumber(MrfState *mrf);
void MrfState_disableSequenceNumber(MrfState *mrf);
void MrfState_enableSequenceNumber(MrfState *mrf);
const uint8_t *MrfState_getFullHeaderData(MrfState *mrf);
bool MrfState_nextField(MrfState *mrf);
MrfField MrfState_getCurrentField(MrfState *mrf);
uint8_t MrfState_getFullDataLength(MrfState *mrf);
uint8_t MrfState_getCurrentFieldsOffset(MrfState *mrf);
const uint8_t *MrfState_getCurrentFieldsData(MrfState *mrf);
#endif
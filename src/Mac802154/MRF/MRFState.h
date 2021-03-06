#ifndef COMMUNICATIONMODULE_MRFSTATE_H
#define COMMUNICATIONMODULE_MRFSTATE_H

#include "src/Mac802154/MRF/FrameHeader802154.h"
#include "src/Mac802154/MRF/MrfField.h"
#include "CommunicationModule/Mac802154MRFImpl.h"

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
 *
 * It is important to understand the term header is associated
 * with different things in this context.
 * First there is the mac header for 802.15.4 (FrameHeader802154) that is defined
 * exactly as in the 802.15.4 specification. Additionally
 * there is the MrfHeader below, that includes the FrameHeader802154 as well
 * as two preceding bytes telling the Mrf chip how long the total frame and
 * the frame header are.
 */


enum {
  MRF_STATE_HEADER_LENGTH_CHANGED       = 1,
  MRF_STATE_FRAME_LENGTH_CHANGED        = 1 << 1,
  MRF_STATE_FRAME_CONTROL_FIELD_CHANGED = 1 << 2,
  MRF_STATE_PAN_ID_CHANGED              = 1 << 3,
  MRF_STATE_DESTINATION_ADDRESS_CHANGED = 1 << 4,
  MRF_STATE_PAYLOAD_CHANGED             = 1 << 6,
};

void MrfState_init(MrfState *mrf_state);
void MrfState_setExtendedDestinationAddress(MrfState *mrf, const uint8_t *address);
void MrfState_setShortDestinationAddress(MrfState *mrf, const uint8_t *address);
void MrfState_setShortSourceAddress(MrfState *mrf, const uint8_t *address);
void MrfState_setExtendedSourceAddress(MrfState *mrf, const uint8_t *address);
void MrfState_setPanId(MrfState *mrf, const uint8_t *pan_id);
void MrfState_setPayload(MrfState *mrf, const uint8_t *payload, uint8_t payload_length);
const uint8_t *MrfState_getPayload(MrfState *mrf);
uint8_t MrfState_getPayloadLength(MrfState *mrf);
MrfField MrfState_getPayloadField(MrfState *mrf_state);
void MrfState_setSequenceNumber(MrfState *mrf);
void MrfState_disableSequenceNumber(MrfState *mrf);
void MrfState_enableSequenceNumber(MrfState *mrf);
void MrfState_enableAcknowledgement(MrfState *mrf);
const uint8_t *MrfState_getFullHeaderData(MrfState *mrf);
bool MrfState_moveIteratorToNextField(MrfState *mrf);

MrfField MrfState_getCurrentField(MrfState *mrf);
uint8_t MrfState_getFullHeaderLength(MrfState *mrf);

MrfField MrfState_getFullHeaderField(MrfState *mrf_state);
#endif
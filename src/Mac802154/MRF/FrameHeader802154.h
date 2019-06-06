
#ifndef COMMUNICATIONMODULE_MAC802154FRAMEHEADER_H
#define COMMUNICATIONMODULE_MAC802154FRAMEHEADER_H

#include <stdint.h>
#include <stdbool.h>
#include "CommunicationModule/FrameHeader802154Struct.h"

void FrameHeader802154_init(FrameHeader802154 *self);

void FrameHeader802154_enableSequenceNumberSuppression(FrameHeader802154 *self);
void FrameHeader802154_disableSequenceNumberSuppression(FrameHeader802154 *self);
void FrameHeader802154_enableAcknowledgementRequest(FrameHeader802154 *self);
void FrameHeader802154_disableAcknowledgementRequest(FrameHeader802154 *self);
void FrameHeader802154_setShortDestinationAddress(FrameHeader802154 *self, const uint8_t *address);
void FrameHeader802154_setExtendedDestinationAddress(FrameHeader802154 *self, const uint8_t *address);
void FrameHeader802154_setShortSourceAddress(FrameHeader802154 *self, const uint8_t *address);
void FrameHeader802154_setExtendedSourceAddress(FrameHeader802154 *self, const uint8_t *address);

/**
 * The pan id is always interpreted as the destination pan id, this leads to
 * some invalid configurations being possible, trying to set up an invalid configuration
 * results in an Exception. An invalid configuration is e.g. setting a source address
 * and pan id without setting a destination address. Also refer to Table 7-2 (Pan id compression
 * field value) in the 802.15.4-2015 specification.
 * */
void FrameHeader802154_setPanId(FrameHeader802154 *self, const uint8_t *pan_id);
void FrameHeader802154_setSequenceNumber(FrameHeader802154 *self, uint8_t number);


// calculates the header size based on what address formats are used and if sequence numbers are enabled
// all sizes measured in bytes
uint8_t FrameHeader802154_getHeaderSize(FrameHeader802154 *self);
uint8_t FrameHeader802154_getSourceAddressSize(const FrameHeader802154 *self);
uint8_t FrameHeader802154_getDestinationAddressSize(const FrameHeader802154 *self);
uint8_t FrameHeader802154_getPanIdSize(const FrameHeader802154 *self);
uint8_t FrameHeader802154_getSequenceNumberSize(const FrameHeader802154 *self);

uint8_t FrameHeader802154_getDestinationAddressOffset(const FrameHeader802154 *self);
uint8_t FrameHeader802154_getSourceAddressOffset(const FrameHeader802154 *self);


const uint8_t *FrameHeader802154_getSequenceNumberPtr(const FrameHeader802154 *self);
const uint8_t *FrameHeader802154_getPanIdPtr(const FrameHeader802154 *self);
const uint8_t *FrameHeader802154_getDestinationAddressPtr(const FrameHeader802154 *self);
const uint8_t *FrameHeader802154_getSourceAddressPtr(const FrameHeader802154 *self);
const uint8_t *FrameHeader802154_getHeaderPtr(const FrameHeader802154 *self);


#endif //COMMUNICATIONMODULE_MAC802154FRAMEHEADER_H

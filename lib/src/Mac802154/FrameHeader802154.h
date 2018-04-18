
#ifndef COMMUNICATIONMODULE_MAC802154FRAMEHEADER_H
#define COMMUNICATIONMODULE_MAC802154FRAMEHEADER_H

#include <stdint.h>
#define MAXIMUM_HEADER_SIZE 21

typedef struct FrameHeader802154_t FrameHeader802154_t;

void FrameHeader802154_init(FrameHeader802154_t *self);

void enableSequenceNumberSuppression(FrameHeader802154_t *self);
void disableSequenceNumberSuppression(FrameHeader802154_t *self);
void FrameHeader802154_setShortDestinationAddress(FrameHeader802154_t *self, uint16_t address);
void FrameHeader802154_setExtendedDestinationAddress(FrameHeader802154_t *self, uint64_t address);
void FrameHeader802154_setShortSourceAddress(FrameHeader802154_t *self, uint16_t address);
void FrameHeader802154_setExtendedSourceAddress(FrameHeader802154_t *self, uint64_t address);

/**
 * The pan id is always interpreted as the destination pan id, this leads to
 * some invalid configurations being possible, trying to set up an invalid configuration
 * results in an Exception. An invalid configuration is e.g. setting a source address
 * and pan id without setting a destination address. Also refer to Table 7-2 (Pan id compression
 * field value) in the 802.15.4-2015 specification.
 * */
void FrameHeader802154_setPanId(FrameHeader802154_t *self, uint16_t pan_id);
void FrameHeader802154_setSequenceNumber(FrameHeader802154_t *self, uint8_t number);


// calculates the header size based on what address formats are used and if sequence numbers are enabled
uint8_t FrameHeader802154_getHeaderSize(FrameHeader802154_t *self);
uint8_t FrameHeader802154_getSourceAddressSize(const FrameHeader802154_t *self);
uint8_t FrameHeader802154_getDestinationAddressSize(const FrameHeader802154_t *self);
const uint8_t *FrameHeader802154_getSequenceNumberPtr(FrameHeader802154_t *self);
const uint8_t *FrameHeader802154_getPanIdPtr(FrameHeader802154_t *self);

#endif //COMMUNICATIONMODULE_MAC802154FRAMEHEADER_H

#ifndef COMMUNICATIONMODULE_MAC802154MRFIMPLINTERN_H
#define COMMUNICATIONMODULE_MAC802154MRFIMPLINTERN_H

#include <stdio.h>
#include "src/Mac802154/Mac802154.h"
#include "include/Mac802154MRFImpl.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "src/Mac802154/MRF/MRFHelperFunctions.h"
#include "src/Mac802154/MRF/MRFState.h"
#include "src/Mac802154/MRF/MrfIo.h"

/**
 * # Data Frame Header structure #
 * To reduce the number of bytes that need to be transferred to actually send a data frame
 * we make some protocol parameters fixed.
 *
 *  - include the short source address instead of the extended one
 *  - for all frames destination and source pan id are equal (only intra-pan messages)
 *  - only send data frames (this will change in future)
 *  - always use pan id compression
 *  - do not use any security mechanisms
 *  - do not use acknowledgments (this will change soon)
 *
 *  Referring to the 802.15.4 standard this leads to the following header value:
 *
 * |Name               | Value | Description                                                                      |
 * |------------------ |-------|----------------------------------------------------------------------------------|
 * | Frame Type        | 0b001 | Data Frame (other possible values are e.g. Acknowledgment or MAC Command)        |
 * | Security Enabled  | 0b0   | Disabled                                                                         |
 * | Frame Pending     | 0b0   | only relevant for specific modes and frames, we say no frame pending here        |
 * | AR                | 0b0   | tell the recepient we don't want an Acknowledgement                              |
 * | PAN ID Compression| 0b1   | only use one PAN ID field because source and destination PAN ID will be the same |
 * | Reserved field    | 0b0   | -                                                                                |
 * | Sequence Number
 * | suppression       | 0b0   | We have to include a sequence number (not supressing makes changes easier later on)|
 * | IE Present        | 0b0   | We don't include an information element
 * | Destination
 * | Addressing mode   | 0b10/0b11 | short destination address / long destination address
 * | Frame Version     | 0b10  | we follow the last standard from 2015
 * | Source
 * | Addressing mode   | 0b10  | always include the short source address
 *
 *
 *
 * ## Memory Layout ##
 * The memory mapped area for the TX Normal FIFO (the fifo buffer
 * that the mrf module uses for normal TX frames) has the address space
 * from 0x00 to 0x7F (i.e. 128 Bytes). Using a word length of one byte,
 * and 16bit addresses
 * this address space is mapped to an 802.15.4 packet as follows:
 *
 * |Header Length (m) | Frame (n) | Header                 | Payload                           |
 * |------------------|-----------|------------------------|-----------------------------------|
 * |0x000             | 0x001     | 0x002 - (0x002 + m -1) | (0x002 + m) - (0x002 + m + n - 1) |
 *
 * (see http://www.microchip.com/wwwproducts/en/en027752 (03.02.2018))
 *
 * ## Header Format ##
 * Additionally we have to know how the header has to be build:
 * The header is comprised of the following fields
 *
 * | Number of Octets:| Field Name:               |
 * |------------------|---------------------------|
 * | 1/2              | Frame Control             |
 * | 0/1              | Sequence Number           |
 * | 0/2              | Destination PAN ID        |
 * | 0/2/8            | Destination Address       |
 * | 0/2              | Source PAN ID             |
 * | 0/2/8            | Source Address            |
 * | 0/2/8            | Auxiliary Security Header |
 * | variable         | Header IEs                |
 *
 */

typedef struct Mrf Mrf;
struct Mrf {
  struct Mac802154 mac;
  MrfIo io;
  void (*deallocate)(void *);
  void (*delay_microseconds)(double);
  MrfState state;
};

static void init(Mac802154 *self, const Mac802154Config *config);
static void destroy(Mac802154 *self);
static void setShortDestinationAddress(Mac802154 *self, uint16_t address);
static void setShortDestinationAddressFromArray(Mac802154 *self, const uint8_t *address);
static void setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length);
static void sendBlocking(Mac802154 *self);
static void setExtendedDestinationAddress(Mac802154 *self, uint64_t address);
static void setExtendedDestinationAddressFromArray(Mac802154 *self, const uint8_t *address);
static void setShortSourceAddress(Mrf *impl, const uint16_t* address);
static void setExtendedSourceAddress(Mrf *impl, const uint64_t *address);
static void setPanId(Mrf *impl, const uint16_t *pan_id);
static uint8_t getReceivedMessageSize(Mac802154 *self);
static bool newMessageAvailable(Mac802154 *self);
static void fetchMessageBlocking(Mac802154 *self, uint8_t *buffer, uint8_t size);
static const uint8_t *getPacketPayload(const uint8_t *packet);
static uint8_t getPacketPayloadSize(const uint8_t *packet);
static bool packetAddressIsShort(const uint8_t *packet);
static bool packetAddressIsLong(const uint8_t *packet);
static uint8_t getPacketSourceAddressSize(const uint8_t *packet);
static const uint8_t *getPacketSourceAddress(const uint8_t *packet);

static void reset(Mrf *impl);
static void setInitializationValuesFromDatasheet(MrfIo *impl);
static void setPrivateVariables(Mrf *impl, const Mac802154Config *config);
static void setUpInterface(Mac802154 *interface);
static void enableRXInterrupt(Mrf *impl);
static void setChannel(Mrf *impl, uint8_t channel);
static void setUpTransmitterPower(Mrf *impl);
static void resetInternalRFStateMachine(Mrf *impl);
static void triggerSend(Mrf *impl);
extern void debug(uint8_t *string);
static void setPromiscuousMode(Mrf *impl);

static const uint8_t frame_length_field_size = 1;
static const uint8_t rssi_field_size = 1;
static const uint8_t frame_check_sequence_size = 2;
static const uint8_t link_quality_field_size = 1;



#endif //COMMUNICATIONMODULE_MAC802154MRFIMPLINTERN_H_H

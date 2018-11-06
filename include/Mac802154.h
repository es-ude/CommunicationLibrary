#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/Peripheral.h"

/*!
 *  The Mac802154 ADT is used to send/receive/analyze
 *  802.15.4 packages. For performance and interoperability
 *  reasons the way message sending works is a little bit
 *  unintuitive. To prepare a message for sending you
 *  use several setter functions to set things like
 *  the destination address or the payload seperately.
 *  Once you are happy with your message setup, you can
 *  send the frame using the Mac802154_sendBlocking() function.
 *
 */

typedef struct Mac802154* Mac802154;
typedef struct Mac802154Config Mac802154Config;

struct Mac802154Config {
  uint16_t short_source_address;
  uint64_t extended_source_address;
  uint16_t pan_id;
  uint8_t channel;
};

/**
 * This sets up internal fields and initializes hardware
 * if necessary. PeripheralInterface and Peripheral handed over
 * in the config are expected to be set up already. The Mac802154Config
 * struct can be safely removed after the function returned.
 */
void Mac802154_configure(Mac802154 hardware, const Mac802154Config *config);

void Mac802154_sendBlocking(Mac802154 hardware);

void Mac802154_setShortDestinationAddress(Mac802154 self, uint16_t address);

/**
 * Call this to configure the module to include the extended
 * source address when constructing a frame. Please note that
 * you can currently only use either the extended or the short
 * source address, but not both.
 */
void Mac802154_useExtendedSourceAddress(Mac802154 self);

/**
 * Call this to configure the module to include the short
 * source address when constructing a frame. Please note that
 * you can currently only use either the extended or the short
 * source address, but not both.
 */
void Mac802154_useShortSourceAddress(Mac802154 self);

/**
 * In promiscuous mode all 802.15.4 frames with a
 * correct crc will be received, no matter how their
 * address fields are set.
 * WARNING: Remember to disable promiscuous mode before sending
 * any packages. Otherwise your device might stop receiving any packages.
 * (This has been observed for the MRF24J40.)
 */
void Mac802154_enablePromiscuousMode(Mac802154 self);

void Mac802154_disablePromiscuousMode(Mac802154 self);

/**
 * sets address in big endian representation suitable for network transmission
*/
void Mac802154_setExtendedDestinationAddress(Mac802154 self, uint64_t address);

/**
 * the payload needs to be alive in memory while transmission is running
*/
void Mac802154_setPayload(Mac802154 self, const char *payload, size_t payload_length);

/**
 *
 * @return size of all data available, this might also include additional information like rssi
 */
uint8_t Mac802154_getReceivedPacketSize(Mac802154 self);

/**
 * check whether the hardware has received new data since the last call to this function
 */
bool Mac802154_newPacketAvailable(Mac802154 self);

/**
 * Place all received data available from the hardware into the buffer.
 * @param self
 * @param buffer
 * @param size This should be the value you got prior from Mac802154_getReceivedPacketSize(), generally
 *             you're free to use a different one but the result will almost certainly lead to problems with the
 *             inspection functions, that expect a complete frame.
 */
void Mac802154_fetchPacketBlocking(Mac802154 self, uint8_t *buffer, uint8_t size);

/**
 * @return A pointer to the start of the payload field
 */
const char * Mac802154_getPacketPayload(Mac802154 self, const uint8_t *packet);
uint8_t Mac802154_getPacketPayloadSize(Mac802154 self, const uint8_t *packet);

bool Mac802154_packetAddressIsShort(Mac802154 self, const uint8_t *packet);

bool Mac802154_packetAddressIsLong(Mac802154 self, const uint8_t *packet);

uint8_t Mac802154_getPacketSourceAddressSize(Mac802154 self, const uint8_t *packet);

uint64_t Mac802154_getPacketExtendedSourceAddress(const Mac802154 self, const uint8_t *packet);
uint16_t Mac802154_getPacketShortSourceAddress(const Mac802154 self, const uint8_t *packet);

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

#endif //COMMUNICATIONMODULE_NETWORKHARDWARE_H

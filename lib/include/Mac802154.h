#ifndef COMMUNICATIONMODULE_NETWORKHARDWARE_H
#define COMMUNICATIONMODULE_NETWORKHARDWARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/include/Peripheral.h"

/**
 * \file
 * This Module allows to send, receive and inspect 802154 packets.
 * The way sending of new packets works is by setting internal fields.
 * So instead of building a new frame as a seperate data structure,
 * you only provide a pointer to the payload and use the other setters
 * to configure the packet that can then be sent by calling the send(Non)Blocking
 * function on the Mac802154 data type. While the payload needs to be present during
 * the process of sending, other parameters like the destination address are copied
 * to an internal data structure.
 *
 * The structure of received frames is implementation dependent, but the
 * module provides functions to inspect the frames. This includes extracting
 * the payload and source addresses. Corresponding functions include the
 * word packet in their name.
 * A packet in the context of this interface is all data that is created
 * by the hardware in the process of receiving a 802154 frame. Therefore
 * this might also include values like frame length or rssi.
 *
 *
 * OPTION 1:
 * The following example illustrates how packet reception with interrupts
 * could be implemented.
 * ```
 *
 * Mac802154 *mac;
 *
 * uint8_t *buffer_for_last_packet;
 *
 * void saveMessage(void)
 * {
 *      uint8_t last_packet_size = Mac802154_getLastPacketSize(mac);
 *      buffer_for_last_packet = malloc(last_packet_size);
 *      Mac802154_fetchPacketNonBlocking(buffer_for_last_packet, last_packet_size);
 * }
 *
 * void macInterruptSetup(void)
 * {
 *      Mac802154_setTriggerFetchPacketCallback(mac, saveMessage);
 *      Mac802154_setFetchPacketCompletionCallback(mac, someOtherFunction);
 * }
 *
 * ISR(interrupt_triggered_by_your_hardware)
 * {
 *      Mac802154_handleInterrupt(mac);
 * }
 * ```
 *
 * OPTION2:
 * ```
 * Mac802154 *mac;
 * uint8_t *next_packet_buffer;
 * uint8_t *current_packet_buffer;
 * bool new_packet = false;
 *
 * void macInterruptSetup(void)
 * {
 *      next_packet_buffer = malloc(MAX_PACKET_SIZE);
 *      Mac802154_setPacketBuffer(mac, next_packet_buffer);
 *      Mac802154_setFetchPacketCompletionCallback(mac, packetReceptionCompletionCallback);
 * }
 *
 * void packetReceptionCompletionCallback(void)
 * {
 *      if (!new_packet)
 *      {
 *          new_packet = true;
 *          current_packet_buffer = next_packet_buffer;
 *          next_packet_buffer = malloc(MAX_PACKET_SIZE);
 *          Max802154_setPacketBuffer(mac, next_packet_buffer);
 *      }
 * }
 *
 * ISR(interrupt_triggered_by_your_hardware)
 * {
 *      Mac802154_handleInterrupt(mac);
 * }
 * ```
 *
 * OPTION3:
 * ```
 * Mac802154 *mac;
 * uint8_t *packet_buffer;
 *
 * void *provideMemory(size_t size)
 * {
 *      packet_buffer = malloc(size);
 *      return packet_buffer;
 * }
 *
 * void interruptSetup(void)
 * {
 *      Mac802154_setPacketMemoryAllocator(mac, provideMemory);
 *      Mac802154_setFetchPacketCompletionCallback(mac, someOtherFunction);
 * }
 *
 * ISR(interrupt_triggered_by_your_hardware)
 * {
 *      Mac802154_handleInterrupt(mac);
 * }

 * ```
 */

typedef struct Mac802154 Mac802154;
typedef struct Mac802154Config Mac802154Config;

struct Mac802154Config {
  uint16_t short_source_address;
  uint64_t extended_source_address;
  uint16_t pan_id;
  uint8_t channel;
  PeripheralInterface interface;
  Peripheral *device;
};

/**
 * This sets up internal fields and initializes hardware
 * if necessary. PeripheralInterface and Peripheral handed over
 * in the config are expected to be set up already. The Mac802154Config
 * struct can be safely removed after the function returned.
 */
void Mac802154_init(Mac802154 *hardware, const Mac802154Config *config);

void Mac802154_sendBlocking(Mac802154 *hardware);

void Mac802154_destroy(Mac802154 *self);

void Mac802154_setShortDestinationAddress(Mac802154 *self, uint16_t address);


void Mac802154_setShortDestinationAddressFromArray(Mac802154 *self, const uint8_t *address);

/**
 * sets address in big endian representation suitable for network transmission
*/
void Mac802154_setExtendedDestinationAddress(Mac802154 *self, uint64_t address);

/**
 * use a one-to-one copy of the specified address as destination address
* use this function to set a previously received source address as the new destination address
*/
void Mac802154_setExtendedDestinationAddressFromArray(Mac802154 *self, const uint8_t *address);

/**
 * the payload needs to be alive in memory while transmission is running
*/
 void Mac802154_setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length);

/**
 *
 * @return size of all data available, this might also include additional information like rssi
 */
uint8_t Mac802154_getReceivedPacketSize(Mac802154 *self);

/**
 * check whether the hardware has received new data since the last call to this function
 */
bool Mac802154_newPacketAvailable(Mac802154 *self);

/**
 * Place all received data available from the hardware into the buffer.
 * @param self
 * @param buffer
 * @param size This should be the value you got prior from Mac802154_getReceivedPacketSize(), generally
 *             you're free to use a different one but the result will almost certainly lead to problems with the
 *             inspection functions, that expect a complete frame.
 */
void Mac802154_fetchPacketBlocking(Mac802154 *self, uint8_t *buffer, uint8_t size);

/**
 *
 * @param self
 * @param packet
 * @return A pointer to the start of the payload field
 */
const uint8_t *Mac802154_getPacketPayload(Mac802154 *self, const uint8_t *packet);
uint8_t Mac802154_getPacketPayloadSize(Mac802154 *self, const uint8_t *packet);


bool Mac802154_packetAddressIsShort(Mac802154 *self, const uint8_t *packet);

bool Mac802154_packetAddressIsLong(Mac802154 *self, const uint8_t *packet);

/**
 * @return The size of the source address field in byte. This should be either 0, 2 or 8.
 */
uint8_t Mac802154_getPacketSourceAddressSize(Mac802154 *self, const uint8_t *packet);

/**
 * Use this together with Mac802154_getPacketSourceAddressSize() to retrieve the source address
 * of a packet.
 * @param self
 * @param packet
 * @return Pointer to the start of the source address field
 */
const uint8_t *Mac802154_getPacketSourceAddress(Mac802154 *self, const uint8_t *packet);

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

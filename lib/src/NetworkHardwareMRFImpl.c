#include "lib/include/NetworkHardware.h"
#include "lib/include/NetworkHardwareMRFImpl.h"

/**
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
 * Not all fields have to be used and we will restrict the use of 802.15.4 features so
 * creating frames and keeping the state of the TX Normal FIFO consistent becomes easier
 * and produces less overhead.
 *
 * In the following the choosen values of the fixed fields and general functionality of
 * some other fields are explained.
 *
 * ### Frame Control ###
 * The Frame Control field itself consists of several other fields.
 * The following table shows the chosen fixed values as well as a short description
 * of each values meaning
 *
 * |Name               | Value | Description                                                                      |
 * |------------------ |-------|----------------------------------------------------------------------------------|
 * | Frame Type        | 0b001 | Data Frame (other possible values are e.g. Acknowledgment or MAC Command)        |
 * | Security Enabled  | 0b0   | Disabled                                                                         |
 * | Frame Pending     | 0b0   | only relevant for specific modes and frames, we say no frame pending here        |
 * | AR                | 0b0   | tell the recepient we don't want an Acknowledgement                              |
 * | PAN ID Compression| 0b1   | only use one PAN ID field because source and destination PAN ID will be the same |
 *
 */

typedef struct NetworkHardwareMRFImpl NetworkHardwareMRFImpl;

struct NetworkHardwareMRFImpl {
  NetworkHardware interface;
  Peripheral *output;
};

static void init(NetworkHardware *self);

NetworkHardware *NetworkHardware_createMRF(Peripheral *output, Allocator allocate) {
  NetworkHardwareMRFImpl *impl = allocate(sizeof(NetworkHardwareMRFImpl));
  impl->output = output;
  NetworkHardware *interface = (NetworkHardware*) impl;
  interface->init = init;
  return interface;
}

void init(NetworkHardware *self) {

}

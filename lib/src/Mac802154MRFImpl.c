#include "lib/include/Mac802154.h"
#include "lib/include/Mac802154MRFImpl.h"
#include "lib/src/MRFInternalConstants.h"
#include "lib/src/MRFHelperFunctions.h"
#include "CException.h"
#include "lib/include/Exception.h"

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
 */
FrameControlField802154 default_frame_control_field = {
        .frame_type = 0b001,
        .security_enabled = 0b0,
        .frame_pending = 0b0,
        .acknowledgment_request = 0b0,
        .pan_id_compression = 0b1,
        .reserved = 0b0,
        .sequence_number_suppression = 0b0,
        .information_element_present = 0b0,
        .destination_addressing_mode = 0b10,
        .frame_version = 0b10,
        .source_addressing_mode = 0b10,
};

SPIMessage frame_control_field_message = {
        .length = sizeof(FrameControlField802154),
        .outgoing_data = (uint8_t*) &default_frame_control_field,
        .incoming_data = NULL,
};

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
 */

typedef struct MRFImpl MRFImpl;

struct MRFImpl {
  Mac802154 interface;
  SPISlave *output_device;
  DelayFunction delayMicroseconds;
  uint16_t pan_id;
  union {
    struct {
      uint8_t header;
      uint8_t frame;
    } as_fields;
    uint8_t as_byte_array[2];
  } current_header_and_frame_size;
  union {
    FrameHeader802154 as_fields;
    uint8_t as_byte_array[sizeof(FrameHeader802154)];
  } header;
  SPIMessage header_and_frame_size_message;
  SPIMessage header_message;
};

static const FrameHeader802154 default_header = {
        .control.as_struct = {
                .frame_type = 0b001,
                .security_enabled = 0b0,
                .frame_pending = 0b0,
                .acknowledgment_request = 0b0,
                .pan_id_compression = 0b1,
                .reserved = 0b0,
                .sequence_number_suppression = 0b0,
                .information_element_present = 0b0,
                .destination_addressing_mode = 0b10,
                .frame_version = 0b10,
                .source_addressing_mode = 0b10,
        },
        .destination.short_address = {0x00, 0x00},
        .destination_pan_id = {0xff, 0xff},
        .sequence_number = 0,
};



static void init(Mac802154 *self, const Mac802154Config *config);
static void destroy(Mac802154 *self);

static void setShortAddressRegister(MRF *impl, uint8_t register_address, uint8_t value);
static void setLongAddressRegister(MRF *impl, uint16_t register_address, uint8_t value);
static void reset(MRF *impl);
static void setInitializationValuesFromDatasheet(MRF *impl);
static void setPrivateVariables(MRF *impl, const Mac802154Config *config);
static void selectDevice(MRF *impl);
static void deselectDevice(MRF *impl);
static void enableRXInterrupt(MRF *impl);
static void setChannel(MRF *impl, uint8_t channel);

Mac802154 *Mac802154_createMRF(MemoryManagement *dynamic_memory, DelayFunction delay) {
  MRF *impl = dynamic_memory->allocate(sizeof(*impl));
  impl->deallocate = dynamic_memory->deallocate;
  impl->mac.init = init;
  impl->mac.destroy = destroy;
  impl->delay = delay;
  return (Mac802154*)impl;
}

void init(Mac802154 *self, const Mac802154Config *config) {
  MRF *impl = (MRF *) self;
  setPrivateVariables(impl, config);
  reset(impl);
  setInitializationValuesFromDatasheet(impl);
  enableRXInterrupt(impl);
  impl->delay(200);
}

void enableRXInterrupt(MRF *impl) {
  // clearing a bit in the register enables the corresponding interrupt
  MRF_setControlRegister(impl, mrf_register_interrupt_control, (uint8_t) ~(1 << 3));
}

void setPrivateVariables(MRF *impl, const Mac802154Config *config) {
  impl->interface = config->interface;
  impl->device = config->device;
}

void reset(MRF *impl) {
  MRF_setControlRegister(impl, mrf_register_software_reset, 0x07);
}

void setInitializationValuesFromDatasheet(MRF *impl){
  MRF_setControlRegister(impl, mrf_register_power_amplifier_control2, 0x98);
  MRF_setControlRegister(impl, mrf_register_tx_stabilization, 0x95);
  MRF_setControlRegister(impl, mrf_register_rf_control0, 0x03);
  MRF_setControlRegister(impl, mrf_register_rf_control1, 0x01);
  MRF_setControlRegister(impl, mrf_register_rf_control2, 0x80);
  MRF_setControlRegister(impl, mrf_register_rf_control6, 0x90);
  MRF_setControlRegister(impl, mrf_register_rf_control7, 0x80);
  MRF_setControlRegister(impl, mrf_register_sleep_clock_control1, 0x21);
  MRF_setControlRegister(impl, mrf_register_base_band2, 0x80);
  MRF_setControlRegister(impl, mrf_register_energy_detection_threshold_for_clear_channel_assessment, 0x60);
  MRF_setControlRegister(impl, mrf_register_base_band6, 0x40);
}

void setChannel(MRF *impl, uint8_t channel_number) {

}

void setShortAddressRegister(MRF *impl, uint8_t register_address, uint8_t value) {
}

void setLongAddressRegister(MRF *impl, uint16_t register_address, uint8_t value) {
}

void selectDevice(MRF *impl) {
  PeripheralInterface_selectPeripheral(impl->interface, impl->device);
}

void deselectDevice(MRF *impl) {
  PeripheralInterface_deselectPeripheral(impl->interface, impl->device);
}

void destroy(Mac802154 *self){
  MRF *impl = (MRF*) self;
  impl->deallocate(impl);
}
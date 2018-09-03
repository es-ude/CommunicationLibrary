#include "lib/src/Mac802154/Mac802154.h"
#include "lib/include/Mac802154MRFImpl.h"
#include "lib/src/Mac802154/MRF/MRFInternalConstants.h"
#include "lib/src/Mac802154/MRF/MRFHelperFunctions.h"
#include "lib/src/Mac802154/MRF/MRFState.h"
#include "lib/src/Mac802154/MRF/MrfIo.h"
#include "lib/src/Mac802154/MRF/Mrf.h"

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

static void init(Mac802154 *self, const Mac802154Config *config);
static void destroy(Mac802154 *self);
static void setShortDestinationAddress(Mac802154 *self, uint16_t address);
static void setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length);
static void sendBlocking(Mac802154 *self);
static void setExtendedDestinationAddress(Mac802154 *self, uint64_t address);
static void setShortSourceAddress(Mrf *impl, const uint16_t* address);
static void setExtendedSourceAddress(Mrf *impl, const uint64_t *address);
static void setPanId(Mrf *impl, const uint16_t *pan_id);
static uint8_t getReceivedMessageSize(Mac802154 *self);

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

size_t Mac802154MRF_requiredSize(void) {
  return sizeof(struct Mrf);

}

void Mac802154MRF_create(uint8_t *memory, DelayFunction delay_microseconds) {
  Mrf *impl = (Mrf *) memory;
  impl->delay_microseconds = delay_microseconds;
  impl->deallocate = NULL;
  setUpInterface(&impl->mac);
}

void setUpInterface(Mac802154 *interface) {
  interface->init = init;
  interface->destroy = destroy;
  interface->setShortDestinationAddress = setShortDestinationAddress;
  interface->setPayload = setPayload;
  interface->setExtendedDestinationAddress = setExtendedDestinationAddress;
  interface->sendBlocking = sendBlocking;
  interface->getReceivedMessageSize = getReceivedMessageSize;
}

void init(Mac802154 *self, const Mac802154Config *config) {
  Mrf *impl = (Mrf *) self;
  setPrivateVariables(impl, config);
  reset(impl);
  setInitializationValuesFromDatasheet(&impl->io);
  enableRXInterrupt(impl);
  setChannel(impl, config->channel);
  setUpTransmitterPower(impl);
  setShortSourceAddress(impl, &config->short_source_address);
  setExtendedSourceAddress(impl, &config->extended_source_address);
  setPanId(impl, &config->pan_id);

  MrfState_init(&impl->state);

  MrfState_setPanId(&impl->state, config->pan_id);
  MrfState_setShortSourceAddress(&impl->state, config->short_source_address);
  uint64_t coordinators_address = 0;
  MrfState_setExtendedDestinationAddress(&impl->state, coordinators_address);
}

void setShortSourceAddress(Mrf *impl, const uint16_t *address) {
  MrfIo_writeBlockingToShortAddress(&impl->io, (const uint8_t *) address,
                                  sizeof(uint16_t), mrf_register_short_address_low_byte);
}

void setExtendedSourceAddress(Mrf *impl, const uint64_t *address) {
  MrfIo_writeBlockingToShortAddress(&impl->io, (const uint8_t *) address,
                                  sizeof(uint64_t), mrf_register_extended_address0);
}

void setPanId(Mrf *impl, const uint16_t *pan_id) {
  MrfIo_writeBlockingToShortAddress(&impl->io, (const uint8_t *) pan_id, sizeof(uint16_t), mrf_register_pan_id_low_byte);
}

void enableRXInterrupt(Mrf *impl) {
  // clearing a bit in the register enables the corresponding interrupt
  MrfIo_setControlRegister(&impl->io, mrf_register_interrupt_control, (uint8_t) ~(1 << 3));
}

void setPrivateVariables(Mrf *impl, const Mac802154Config *config) {
  impl->io.interface = config->interface;
  impl->io.device = config->device;
}

void reset(Mrf *impl) {
  MrfIo_setControlRegister(&impl->io, mrf_register_software_reset, mrf_value_full_software_reset);
}

void setInitializationValuesFromDatasheet(MrfIo *io) {
  MrfIo_setControlRegister(
          io,
          mrf_register_power_amplifier_control2,
          (uint8_t)((1 << mrf_fifo_enable) | mrf_value_recommended_transmitter_on_time_before_beginning_a_packet)
  );
  MrfIo_setControlRegister(io, mrf_register_tx_stabilization, mrf_value_recommended_interframe_spacing);
  MrfIo_setControlRegister(io, mrf_register_rf_control0, mrf_value_recommended_rf_optimize_control0);
  MrfIo_setControlRegister(io, mrf_register_rf_control1, mrf_value_recommended_rf_optimize_control1);
  MrfIo_setControlRegister(io, mrf_register_rf_control2, mrf_value_phase_locked_loop_enabled);
  MrfIo_setControlRegister(
          io,
          mrf_register_rf_control6,
          mrf_value_enable_tx_filter | mrf_value_20MHz_clock_recovery_less_than_1ms
  );
  MrfIo_setControlRegister(io, mrf_register_rf_control7, mrf_value_use_internal_100kHz_oscillator);
  MrfIo_setControlRegister(io, mrf_register_rf_control8, mrf_value_recommended_rf_control8);
  MrfIo_setControlRegister(
          io,
          mrf_register_sleep_clock_control1,
          mrf_value_disable_deprecated_clkout_sleep_clock_feature | mrf_value_minimum_sleep_clock_divisor_for_internal_oscillator
  );
  MrfIo_setControlRegister(io, mrf_register_base_band2, mrf_value_clear_channel_assessment_energy_detection_only);
  MrfIo_setControlRegister(io, mrf_register_energy_detection_threshold_for_clear_channel_assessment, mrf_value_recommended_energy_detection_threshold);
  MrfIo_setControlRegister(io, mrf_register_base_band6, mrf_value_append_rssi_value_to_rxfifo);


}

void setChannel(Mrf *impl, uint8_t channel_number) {
  MrfIo_setControlRegister(&impl->io, mrf_register_rf_control0, MRF_getRegisterValueForChannelNumber(channel_number));
  resetInternalRFStateMachine(impl);
}

void setUpTransmitterPower(Mrf *impl) {
  MrfIo_setControlRegister(&impl->io, mrf_register_rf_control3, mrf_value_transmitter_power_minus30dB);
}

void resetInternalRFStateMachine(Mrf *impl) {
  MrfIo_setControlRegister(&impl->io, mrf_register_rf_mode_control, mrf_value_rf_state_machine_reset_state);
  MrfIo_setControlRegister(&impl->io, mrf_register_rf_mode_control, mrf_value_rf_state_machine_operating_state);
  impl->delay_microseconds(mrf_value_delay_interval_after_state_machine_reset);
}

void destroy(Mac802154 *self){
  Mrf *impl = (Mrf*) self;
  impl->deallocate(impl);
}

void setShortDestinationAddress(Mac802154 *self, uint16_t address) {
  Mrf *impl = (Mrf *) self;
  MrfState_setShortDestinationAddress(&impl->state, address);
}

void setPayload(Mac802154 *self, const uint8_t *payload, size_t payload_length) {

  Mrf *impl = (Mrf *) self;
  MrfState_setPayload(&impl->state, payload, (uint8_t) payload_length);
}

void sendBlocking(Mac802154 *self) {
  Mrf *impl = (Mrf *) self;

  MrfField current_field = MrfState_getFullHeaderField(&impl->state);
  MrfIo_writeBlockingToLongAddress(&impl->io, current_field.data, current_field.size, current_field.address);
  current_field = MrfState_getPayloadField(&impl->state);
  MrfIo_writeBlockingToLongAddress(&impl->io, current_field.data, current_field.size, current_field.address);
  triggerSend(impl);
  while (!(MrfIo_readControlRegister(&impl->io, mrf_register_interrupt_status) & 1))
    ;
}

void setExtendedDestinationAddress(Mac802154 *self, uint64_t address) {
  Mrf *impl = (Mrf *) self;
  MrfState_setExtendedDestinationAddress(&impl->state, address);
}

void triggerSend(Mrf *impl) {
  MrfIo_setControlRegister(&impl->io, mrf_register_tx_normal_fifo_control, 1);
}

uint8_t getReceivedMessageSize(Mac802154 *self) {
  Mrf *impl = (Mrf *) self;
}
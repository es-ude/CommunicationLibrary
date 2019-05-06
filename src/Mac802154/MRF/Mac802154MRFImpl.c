#include "src/Mac802154/MRF/Mac802154MRFImplIntern.h"
#include "Util/BitManipulation.h"
#include "Util/Debug.h"
#include <stdio.h>

size_t
Mac802154MRF_getADTSize (void)
{
  return sizeof(struct Mrf);

}

static void
setResetLineToDefinedState(MRFConfig *config)
{
  if (config->reset_line.data_direction_register != NULL && config->reset_line.data_register != NULL)
  {
    *config->reset_line.data_direction_register |= (1 << config->reset_line.pin_number);
    *config->reset_line.data_register |= (1 << config->reset_line.pin_number);
  }
}

void
Mac802154MRF_create(Mac802154           * memory,
    MRFConfig *config)
{
  Mrf *impl = (Mrf *) memory;
  impl->delay_microseconds = config->delay_microseconds;
  setUpInterface(&impl->mac);
  impl->io.interface = config->interface;
  impl->io.device    = config->device;
  setResetLineToDefinedState(config);
}

void
setUpInterface(Mac802154 *interface)
{
  interface->reconfigure = reconfigure;
  interface->setShortDestinationAddress = setShortDestinationAddress;
  interface->setPayload = setPayload;
  interface->setExtendedDestinationAddress = setExtendedDestinationAddress;
  interface->sendBlocking                   = sendBlocking;
  interface->getReceivedPacketSize          = getReceivedMessageSize;
  interface->newPacketAvailable             = newMessageAvailable;
  interface->fetchPacketBlocking            = fetchMessageBlocking;
  interface->getPacketPayload               = getPacketPayload;
  interface->getPacketPayloadSize           = getPacketPayloadSize;
  interface->packetAddressIsShort           = packetAddressIsShort;
  interface->packetAddressIsExtended        = packetAddressIsLong;
  interface->getPacketSourceAddressSize     = getPacketSourceAddressSize;
  interface->getPacketExtendedSourceAddress = getPacketExtendedSourceAddress;
  interface->getPacketShortSourceAddress    = getPacketShortSourceAddress;
  interface->enablePromiscuousMode          = enablePromiscuousMode;
  interface->disablePromiscuousMode         = disablePromiscuousMode;
  interface->useExtendedSourceAddress       = useExtendedSourceAddress;
  interface->useShortSourceAddress          = useShortSourceAddress;
}

void
reconfigure(Mac802154 *self,
            const Mac802154Config *config)
{
  Mrf *impl = (Mrf *) self;
  impl->config = *config;
  debug(String, "resetting mrf...\n");
  reset(impl);
  debug(String, "initializing mrf...\n");
  setInitializationValuesFromDatasheet(&impl->io);
  enableRXInterrupt(impl);
  setChannel(impl, config->channel);
  setUpTransmitterPower(impl);
  setShortSourceAddress(impl, config->short_source_address);
  setExtendedSourceAddress(impl, config->extended_source_address);
  setPanId(impl, config->pan_id);

  MrfState_init(&impl->state);

  MrfState_setPanId(&impl->state, config->pan_id);
  MrfState_setShortSourceAddress(&impl->state, config->short_source_address);
  uint8_t coordinators_address[8] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
  };
  MrfState_setExtendedDestinationAddress(&impl->state, coordinators_address);
}

void
setShortSourceAddress(Mrf           *impl,
                      const uint8_t *address)
{
  MrfIo_writeBlockingToShortAddress(&impl->io, address,
                                    2, mrf_register_short_address_low_byte);
}

void
setExtendedSourceAddress(Mrf           *impl,
                         const uint8_t *address)
{
  MrfIo_writeBlockingToShortAddress(&impl->io,
                                    address,
                                    sizeof(uint64_t),
                                    mrf_register_extended_address0);
}

void
setPanId(Mrf           *impl,
         const uint8_t *pan_id)
{
  MrfIo_writeBlockingToShortAddress(&impl->io,
                                    pan_id,
                                    2,
                                    mrf_register_pan_id_low_byte);
}

void
enableRXInterrupt(Mrf *impl)
{
  // clearing a bit in the register enables the corresponding interrupt
  MrfIo_setControlRegister(&impl->io,
                           mrf_register_interrupt_control,
                           (uint8_t) ~(1 << 3));
}

void
reset(Mrf *impl)
{
  MrfIo_setControlRegister(&impl->io,
                           mrf_register_software_reset,
                           mrf_value_full_software_reset);
}

void
setInitializationValuesFromDatasheet(MrfIo *io)
{
  MrfIo_setControlRegister(
    io,
    mrf_register_power_amplifier_control2,
    (uint8_t) ((1 << mrf_fifo_enable) |
               mrf_value_recommended_transmitter_on_time_before_beginning_a_packet)
    );
  MrfIo_setControlRegister(io,
                           mrf_register_tx_stabilization,
                           mrf_value_recommended_interframe_spacing);
  MrfIo_setControlRegister(io,
                           mrf_register_rf_control0,
                           mrf_value_recommended_rf_optimize_control0);
  MrfIo_setControlRegister(io,
                           mrf_register_rf_control1,
                           mrf_value_recommended_rf_optimize_control1);
  MrfIo_setControlRegister(io,
                           mrf_register_rf_control2,
                           mrf_value_phase_locked_loop_enabled);
  MrfIo_setControlRegister(
    io,
    mrf_register_rf_control6,
    mrf_value_enable_tx_filter | mrf_value_20MHz_clock_recovery_less_than_1ms
    );
  MrfIo_setControlRegister(io,
                           mrf_register_rf_control7,
                           mrf_value_use_internal_100kHz_oscillator);
  MrfIo_setControlRegister(io,
                           mrf_register_rf_control8,
                           mrf_value_recommended_rf_control8);
  MrfIo_setControlRegister(
    io,
    mrf_register_sleep_clock_control1,
    mrf_value_disable_deprecated_clkout_sleep_clock_feature | mrf_value_minimum_sleep_clock_divisor_for_internal_oscillator
    );
  MrfIo_setControlRegister(io,
                           mrf_register_base_band2,
                           mrf_value_clear_channel_assessment_energy_detection_only);
  MrfIo_setControlRegister(io,
                           mrf_register_energy_detection_threshold_for_clear_channel_assessment,
                           mrf_value_recommended_energy_detection_threshold);
  MrfIo_setControlRegister(io,
                           mrf_register_base_band6,
                           mrf_value_append_rssi_value_to_rxfifo);


}

void
setChannel(Mrf    *impl,
           uint8_t channel_number)
{
  MrfIo_setControlRegister(&impl->io,
                           mrf_register_rf_control0,
                           MRF_getRegisterValueForChannelNumber(channel_number));
  resetInternalRFStateMachine(impl);
}

void
setUpTransmitterPower(Mrf *impl)
{
  MrfIo_setControlRegister(&impl->io,
                           mrf_register_rf_control3,
                           mrf_value_transmitter_power_0dB);
}

void
resetInternalRFStateMachine(Mrf *impl)
{
  MrfIo_setControlRegister(&impl->io,
                           mrf_register_rf_mode_control,
                           mrf_value_rf_state_machine_reset_state);
  MrfIo_setControlRegister(&impl->io,
                           mrf_register_rf_mode_control,
                           mrf_value_rf_state_machine_operating_state);
  impl->delay_microseconds(mrf_value_delay_interval_after_state_machine_reset);
}

void
setShortDestinationAddress(Mac802154     *self,
                           const uint8_t *address)
{
  Mrf *impl = (Mrf *) self;
  MrfState_setShortDestinationAddress(&impl->state, address);
}

void
setPayload(Mac802154     *self,
           const uint8_t *payload,
           size_t         payload_length)
{
  Mrf *impl = (Mrf *) self;
  MrfState_setPayload(&impl->state, payload, (uint8_t) payload_length);
}

void
sendBlocking(Mac802154 *self)
{
  Mrf     *impl = (Mrf *) self;

  MrfField current_field = MrfState_getFullHeaderField(&impl->state);
  MrfIo_writeBlockingToLongAddress(&impl->io,
                                   current_field.data,
                                   current_field.length,
                                   current_field.address);
  current_field = MrfState_getPayloadField(&impl->state);
  MrfIo_writeBlockingToLongAddress(&impl->io,
                                   current_field.data,
                                   current_field.length,
                                   current_field.address);
  triggerSend(impl);
}

void
setExtendedDestinationAddress(Mac802154     *self,
                              const uint8_t *address)
{
  Mrf *impl = (Mrf *) self;
  MrfState_setExtendedDestinationAddress(&impl->state, address);
}

void
triggerSend(Mrf *impl)
{
  MrfIo_setControlRegister(&impl->io, mrf_register_tx_normal_fifo_control, 1);
  //here we basically assume a transmission success on completion
  //actually reading the transmission status and throwing an exception
  //if transmission failed would be better.
  while (!(MrfIo_readControlRegister(&impl->io,
                                     mrf_register_interrupt_status) & 1)) {}
}

uint8_t
getReceivedMessageSize(Mac802154 *self)
{
  Mrf    *impl = (Mrf *) self;
  uint8_t size = 0;
  MrfIo_readBlockingFromLongAddress(&impl->io, mrf_rx_fifo_start, &size, 1);
  return size + frame_length_field_size;
}

static void
flushRXBuffer(Mrf *self)
{
  MrfIo_setControlRegister(&self->io, 0x0D, 1);
}

static void
disableReception(Mrf *self)
{
  MrfIo_setControlRegister(&self->io,
                           mrf_register_base_band1,
                           mrf_value_rx_decode_inversion);
}

static void
enableReception(Mrf *self)
{
  MrfIo_setControlRegister(&self->io, mrf_register_base_band1, 0);
}

bool
newMessageAvailable(Mac802154 *self)
{
  Mrf    *impl = (Mrf *) self;
  uint8_t status_register_value = MrfIo_readControlRegister(&impl->io,
                                                            mrf_register_interrupt_status);
  bool    new_message = ((status_register_value >> 3) & 1);
  return new_message;
}

void
fetchMessageBlocking(Mac802154 *self,
                     uint8_t  *buffer,
                     uint8_t   size)
{
  Mrf *impl = (Mrf *) self;
  MrfIo_readBlockingFromLongAddress(&impl->io, mrf_rx_fifo_start, buffer, size);
}

/**
 * accept all packages with correct crc
 *
 */
void
enablePromiscuousMode(Mac802154 *self)
{
  Mrf *impl = (Mrf *) self;
  MrfIo_setControlRegister(&impl->io, mrf_register_receive_mac_control, 1);
}

void
disablePromiscuousMode(Mac802154 *self)
{
  Mrf *impl = (Mrf *) self;
  MrfIo_setControlRegister(&impl->io, mrf_register_receive_mac_control, 0);
}

/**
 * accept packages with good or bad crc
 */
void
setErrorMode(Mrf *impl)
{
  MrfIo_setControlRegister(&impl->io, 0x00, 2);
}

const uint8_t *
getPacketPayload(const uint8_t *packet)
{
  packet += frame_length_field_size;
  return packet + FrameHeader802154_getHeaderSize((FrameHeader802154 *) packet);
}

uint8_t
getPacketPayloadSize(const uint8_t *packet)
{
  uint8_t packet_size = packet[0];
  uint8_t header_size =
    FrameHeader802154_getHeaderSize((FrameHeader802154 *) (packet + 1));
  uint8_t payload_size = packet_size
                         - header_size
                         - frame_check_sequence_size;
  return payload_size;
}

bool
packetAddressIsShort(const uint8_t *packet)
{
  return FrameHeader802154_getSourceAddressSize(
    (FrameHeader802154*) (packet + 1)) == 2;
}

bool
packetAddressIsLong(const uint8_t *packet)
{
  return FrameHeader802154_getSourceAddressSize(
    (FrameHeader802154*) (packet + 1)) == 8;
}

uint8_t
getPacketSourceAddressSize(const uint8_t *packet)
{
  return FrameHeader802154_getSourceAddressSize((FrameHeader802154 *) (packet +
                                                                       1));
}

const uint8_t*
getPacketExtendedSourceAddress(const uint8_t *packet)
{
  return FrameHeader802154_getSourceAddressPtr(
    (FrameHeader802154 *) (packet + 1));
}

const uint8_t *
getPacketShortSourceAddress(const uint8_t *packet)
{
  return FrameHeader802154_getSourceAddressPtr(
    (FrameHeader802154 *) (packet + 1));
}

void
useExtendedSourceAddress(Mac802154 *self)
{
  Mrf *impl = (Mrf *) self;
  MrfState_setExtendedSourceAddress(&impl->state,
                                    impl->config.extended_source_address);
}

void
useShortSourceAddress(Mac802154 *self)
{
  Mrf *impl = (Mrf *) self;
  MrfState_setShortSourceAddress(&impl->state,
                                 impl->config.short_source_address);
}

#include "unity.h"

#include "CommunicationModule/CommunicationModule.h"
#include <stdlib.h>
#include <string.h>

#include "EmbeddedUtilities/BitManipulation.h"
#include "src/Mac802154/MRF/MRFInternalConstants.h"
#include "src/Mac802154/MRF/Mac802154MRFImplIntern.h"
#include "src/Mac802154/MRF/MockMRFHelperFunctions.h"
#include "src/Mac802154/MRF/MockMRFState.h"
#include "src/Mac802154/MRF/MockMrfIo.h"
#include "src/Mac802154/MRF/MockFrameHeader802154.h"
#include "test/MRF/MockMac802154MRF_TestHelper.h"

uint8_t reset_line_ddr;
uint8_t reset_line_data;
static const GPIOPin reset_line = {
  .data_direction_register = &reset_line_ddr,
  .data_register = &reset_line_data,
  .pin_number = 4,
};
Peripheral          *device;
PeripheralInterface *interface;
Mac802154 *mrf;
Mac802154Config      mac_config;
MRFConfig hardware_config;

void
setUp(void)
{
  hardware_config.device = device;
  hardware_config.interface = interface;
  hardware_config.delay_microseconds = fakeDelay;
  hardware_config.transmitter_power = 0;
  hardware_config.reset_line = reset_line;
  mrf = malloc(Mac802154MRF_getADTSize());
  mac_config.pan_id[0] = 0;
  mac_config.pan_id[1] = 0;
  BitManipulation_fillArray(mac_config.extended_source_address, 0, 8);
  mac_config.short_source_address[0] = 0;
  mac_config.short_source_address[1] = 0;
  mac_config.channel = 11;
  Mac802154MRF_create(mrf, &hardware_config);
}

void
tearDown(void)
{
  free(mrf);
}

static void
setUpInitializationValues(MrfIo *impl,
                          const Mac802154Config *config);

void
test_channelSelectionRegisterValueIsCalculatedCorrectly(void)
{
  TEST_ASSERT_EQUAL_UINT8(0x23, MRF_getRegisterValueForChannelNumber(13));
  TEST_ASSERT_EQUAL_UINT8(0x43, MRF_getRegisterValueForChannelNumber(15));
  TEST_ASSERT_EQUAL_UINT8(0xF3, MRF_getRegisterValueForChannelNumber(26));
}

void
test_resetLineIsHigh(void)
{
  TEST_ASSERT_BIT_HIGH(hardware_config.reset_line.pin_number, *hardware_config.reset_line.data_direction_register);
  TEST_ASSERT_BIT_HIGH(hardware_config.reset_line.pin_number, *hardware_config.reset_line.data_register);
}

void
test_initPerformsSetupLikeShownInDatasheet(void)
{
  struct Mrf *impl = (struct Mrf *) mrf;
  setUpInitializationValues(&impl->io, &mac_config);
  MrfState_init_Expect(&impl->state);
  MrfState_setPanId_Expect(&impl->state, mac_config.pan_id);
  MrfState_setShortSourceAddress_Expect(&impl->state,
                                        mac_config.short_source_address);
  uint8_t coordinators_address[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  MrfState_setExtendedDestinationAddress_Expect(&impl->state,
                                                coordinators_address);

  Mac802154_configure(mrf, &mac_config);
  TEST_ASSERT_EQUAL_PTR(impl->io.device,    device);
  TEST_ASSERT_EQUAL_PTR(impl->io.interface, interface);
}

void
test_initWithDifferentConfig(void)
{
  uint8_t extended_address_byte = 0x11;
  for (uint8_t i = 0; i < 8; i++)
    {
      mac_config.extended_source_address[i] = extended_address_byte;
      extended_address_byte += 0x11;
    }
  mac_config.short_source_address[0] = 0xFF;
  mac_config.short_source_address[1] = 0xAB;
  mac_config.channel   = 22;
  mac_config.pan_id[0] = 0xAB;
  mac_config.pan_id[1] = 0xCD;
  struct Mrf *impl = (struct Mrf *) mrf;

  setUpInitializationValues(&impl->io, &mac_config);
  MrfState_init_Expect(&impl->state);
  MrfState_setPanId_Expect(&impl->state, mac_config.pan_id);
  MrfState_setShortSourceAddress_Expect(&impl->state,
                                        mac_config.short_source_address);
  uint8_t coordinators_address[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  MrfState_setExtendedDestinationAddress_Expect(&impl->state,
                                                coordinators_address);

  Mac802154_configure(mrf, &mac_config);
}

void
setUpInitializationValues(MrfIo *impl,
                          const Mac802154Config *config)
{
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_software_reset, mrf_value_full_software_reset);
  MrfIo_setControlRegister_Expect(
    impl,
    mrf_register_power_amplifier_control2,
    mrf_fifo_enable |
    mrf_value_recommended_transmitter_on_time_before_beginning_a_packet);
  MrfIo_setControlRegister_Expect(impl,
                                  mrf_register_tx_stabilization,
                                  mrf_value_recommended_interframe_spacing);
  MrfIo_setControlRegister_Expect(impl,
                                  mrf_register_rf_control0,
                                  mrf_value_recommended_rf_optimize_control0);
  MrfIo_setControlRegister_Expect(impl,
                                  mrf_register_rf_control1,
                                  mrf_value_recommended_rf_optimize_control1);
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_rf_control2, mrf_value_phase_locked_loop_enabled);
  MrfIo_setControlRegister_Expect(
    impl,
    mrf_register_rf_control6,
    mrf_value_enable_tx_filter | mrf_value_20MHz_clock_recovery_less_than_1ms);
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_rf_control7, mrf_value_use_internal_100kHz_oscillator);
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_rf_control8, mrf_value_recommended_rf_control8);
  MrfIo_setControlRegister_Expect(
    impl,
    mrf_register_sleep_clock_control1,
    mrf_value_disable_deprecated_clkout_sleep_clock_feature |
    mrf_value_minimum_sleep_clock_divisor_for_internal_oscillator);
  MrfIo_setControlRegister_Expect(
    impl,
    mrf_register_base_band2,
    mrf_value_clear_channel_assessment_energy_detection_only);
  MrfIo_setControlRegister_Expect(
    impl,
    mrf_register_energy_detection_threshold_for_clear_channel_assessment,
    mrf_value_recommended_energy_detection_threshold);
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_base_band6, mrf_value_append_rssi_value_to_rxfifo);
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_interrupt_control, mrf_value_rx_interrupt_enabled);

  // select channel 11, afterwards the rf state machine should be reset
  MrfIo_setControlRegister_Expect(
    impl,
    mrf_register_rf_control0,
    MRF_getRegisterValueForChannelNumber(config->channel));
  MrfIo_setControlRegister_Expect(impl,
                                  mrf_register_rf_mode_control,
                                  mrf_value_rf_state_machine_reset_state);
  MrfIo_setControlRegister_Expect(impl,
                                  mrf_register_rf_mode_control,
                                  mrf_value_rf_state_machine_operating_state);

  fakeDelay_Expect(mrf_value_delay_interval_after_state_machine_reset);

  // set transmitter power to 0dB
  MrfIo_setControlRegister_Expect(
    impl, mrf_register_rf_control3, mrf_value_transmitter_power_0dB);

  // here the addresses are required to be stored in ascending byte order (big
  // endian)
  MrfIo_writeBlockingToShortAddress_Expect(
    impl,
    (uint8_t *) &config->short_source_address,
    2,
    mrf_register_short_address_low_byte);
  MrfIo_writeBlockingToShortAddress_Expect(
    impl,
    (uint8_t *) &config->extended_source_address,
    8,
    mrf_register_extended_address0);
  MrfIo_writeBlockingToShortAddress_Expect(
    impl, (uint8_t *) &config->pan_id, 2, mrf_register_pan_id_low_byte);
}

void
test_sendBlocking(void)
{
  struct Mrf *impl           = (struct Mrf *) mrf;
  uint8_t     payload[]      = "hello, world!";
  uint8_t     payload_length = (uint8_t) strlen((char *) payload);

  MrfState_setPayload_Expect(NULL, payload, payload_length);
  MrfState_setPayload_IgnoreArg_mrf();

  Mac802154_setPayload(mrf, (uint8_t *) payload, payload_length);

  uint8_t  fake_header_data[]         = "123456789";
  uint8_t  fake_header_length         = 9;
  uint8_t  fake_header_memory_address = 0;
  MrfField full_header = {
    .data    = fake_header_data,
    .length  = fake_header_length,
    .address = fake_header_memory_address,
  };
  MrfState_getFullHeaderField_ExpectAnyArgsAndReturn(full_header);

  MrfIo_writeBlockingToLongAddress_Expect(
    NULL, full_header.data, full_header.length, full_header.address);
  MrfIo_writeBlockingToLongAddress_IgnoreArg_mrf();

  MrfField payload_field = {
    .data    = (uint8_t *) payload,
    .length  = payload_length,
    .address = fake_header_length,
  };
  MrfState_getPayloadField_ExpectAnyArgsAndReturn(payload_field);

  MrfIo_writeBlockingToLongAddress_Expect(
    NULL, payload_field.data, payload_field.length, payload_field.address);
  MrfIo_writeBlockingToLongAddress_IgnoreArg_mrf();

  MrfIo_setControlRegister_Expect(
    &impl->io, mrf_register_tx_normal_fifo_control, 1);

  MrfIo_readControlRegister_ExpectAndReturn(
    NULL, mrf_register_interrupt_status, 0);
  MrfIo_readControlRegister_IgnoreArg_mrf();
  MrfIo_readControlRegister_ExpectAndReturn(
    NULL, mrf_register_interrupt_status, 1);
  MrfIo_readControlRegister_IgnoreArg_mrf();

  Mac802154_sendBlocking(mrf);
}

void
test_getMessageSizeMessage(void)
{
  uint8_t expected_frame_size     = 0xAB;
  uint8_t frame_length_field_size = 1;
  uint8_t expected_packet_size    = expected_frame_size +
                                    frame_length_field_size;
  MrfIo_readBlockingFromLongAddress_Expect(
    NULL, mrf_rx_fifo_start, &expected_frame_size, 1);
  MrfIo_readBlockingFromLongAddress_IgnoreArg_buffer();
  MrfIo_readBlockingFromLongAddress_IgnoreArg_mrf();
  MrfIo_readBlockingFromLongAddress_ReturnArrayThruPtr_buffer(
    &expected_frame_size, 1);

  uint8_t packet_size = Mac802154_getReceivedPacketSize(mrf);
  TEST_ASSERT_EQUAL(expected_packet_size, packet_size);
}

static bool
gotNewMessage(uint8_t status_register_value)
{
  return (status_register_value >> 3) & 1;
}

static void
expectDisableReception(MrfIo *io)
{
        MrfIo_setControlRegister_Expect(io, mrf_register_base_band1, mrf_value_rx_decode_inversion);
}

static void
expectEnableReception(MrfIo *io)
{
  MrfIo_setControlRegister_Expect(io, mrf_register_base_band1, 0);
}

static void
expectRXFlush(MrfIo *io)
{
        MrfIo_setControlRegister_Expect(io, mrf_register_rx_flush, 1);
}

void
test_newMessageAvailable(void)
{
  uint8_t interrupt_register_value = 0;
  char    error_message[32];

  MrfIo  *io = &((struct Mrf *) mrf)->io;
  // more than about 3*108 calls to for the mock functions don't seem possible
  for (uint16_t counter = 0; counter < 108; counter++)
    {
      interrupt_register_value = (uint8_t) counter;
      MrfIo_readControlRegister_ExpectAndReturn(
	io, mrf_register_interrupt_status, interrupt_register_value);

      bool result = Mac802154_newPacketAvailable(mrf);

      sprintf(error_message, "failed for value %d", interrupt_register_value);
      if (gotNewMessage(interrupt_register_value))
	{
	  TEST_ASSERT_TRUE_MESSAGE(result, error_message);
	}
      else
	{
	  TEST_ASSERT_FALSE_MESSAGE(result, error_message);
	}
    }
}

void
test_fetchMessageBlocking(void)
{
  uint8_t expected_message[] = "hello world!";
  MrfIo_readBlockingFromLongAddress_Expect(
    NULL,
    mrf_rx_fifo_start,
    expected_message,
    (uint8_t) strlen((char *) expected_message));
  MrfIo_readBlockingFromLongAddress_IgnoreArg_mrf();
  MrfIo_readBlockingFromLongAddress_IgnoreArg_buffer();
  MrfIo_readBlockingFromLongAddress_ReturnArrayThruPtr_buffer(
    expected_message, strlen((char *) expected_message));

    uint8_t message_buffer[16];
  memset(message_buffer, 0, 16);
  Mac802154_fetchPacketBlocking(
    mrf, message_buffer, (uint8_t) strlen((char *) expected_message));
  TEST_ASSERT_EQUAL_STRING(expected_message, message_buffer);
}

void
test_getReceivedFramePayload(void)
{
  const uint8_t     *payload;
  uint8_t            header_size = 14;
  uint8_t            frame_length_field_size         = 1;
  uint8_t            bigger_than_maximum_header_size = 2 * MAXIMUM_HEADER_SIZE;
  uint8_t            random_packet_data[bigger_than_maximum_header_size];
  FrameHeader802154 *frame =
    (FrameHeader802154 *) (random_packet_data + frame_length_field_size);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(frame, header_size);
  payload = Mac802154_getPacketPayload(mrf, (uint8_t *) random_packet_data);
  TEST_ASSERT_EQUAL_PTR(
    random_packet_data + header_size + frame_length_field_size, payload);
}

void
test_packetAddressIsShort(void)
{
  uint8_t *packet = (uint8_t *) 134;
  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 2);
  TEST_ASSERT_TRUE(Mac802154_packetAddressIsShort(mrf, packet));

  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 2);
  TEST_ASSERT_FALSE(Mac802154_packetAddressIsLong(mrf, packet));
}

void
test_packetAddressIsExtended(void)
{
  uint8_t *packet = (uint8_t *) 85;
  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 8);
  TEST_ASSERT_TRUE(Mac802154_packetAddressIsLong(mrf, packet));

  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 8);
  TEST_ASSERT_FALSE(Mac802154_packetAddressIsShort(mrf, packet));
}

void
test_packetAddressIsNeitherExtendedNorShort(void)
{
  uint8_t *packet = (uint8_t *) 34;
  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 61);
  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 15);
  TEST_ASSERT_FALSE(Mac802154_packetAddressIsLong(mrf, packet));
  TEST_ASSERT_FALSE(Mac802154_packetAddressIsShort(mrf, packet));
}

void
test_getPacketSourceAddressSize(void)
{
  uint8_t *packet = (uint8_t *) 14;
  FrameHeader802154_getSourceAddressSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), 16);
  TEST_ASSERT_EQUAL_UINT8(16,
                          Mac802154_getPacketSourceAddressSize(mrf, packet));
}

void
test_getPacketExtendedSourceAddress(void)
{
  uint8_t            address[]        = { 0xAA, 0xBB, 0xCC };
  uint8_t           *packet           = (uint8_t *) &address - 1;
  FrameHeader802154 *frame_header_ptr = (FrameHeader802154 *) &address;
  FrameHeader802154_getSourceAddressPtr_ExpectAndReturn(frame_header_ptr,
                                                        packet + 1);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(
    address, Mac802154_getPacketExtendedSourceAddress(mrf, packet), 8);
}

void
test_getPacketShortSourceAddress(void)
{
  uint8_t            address[]        = { 0xCC, 0xDD };
  uint8_t           *packet           = (uint8_t *) &address - 1;
  FrameHeader802154 *frame_header_ptr = (FrameHeader802154 *) &address;
  FrameHeader802154_getSourceAddressPtr_ExpectAndReturn(frame_header_ptr,
                                                        (uint8_t *) &address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(
    address, Mac802154_getPacketShortSourceAddress(mrf, packet), 2);
}

void
test_getPacketPayloadSize(void)
{
  uint8_t packet[32];
  uint8_t frame_size = 38;
  packet[0] = frame_size;
  uint8_t header_size = 17;
  uint8_t frame_check_sequence_size = 2;
  uint8_t expected_size = frame_size - header_size - frame_check_sequence_size;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(
    (FrameHeader802154 *) (packet + 1), header_size);
  TEST_ASSERT_EQUAL_UINT8(expected_size,
                          Mac802154_getPacketPayloadSize(mrf, packet));
}

void
test_enablePromiscuousMode(void)
{
  Mrf *impl = (Mrf *) mrf;
  MrfIo_setControlRegister_Expect(
    &impl->io, mrf_register_receive_mac_control, 1);
  Mac802154_enablePromiscuousMode(mrf);
}

void
test_disablePromiscuousMode(void)
{
  Mrf *impl = (Mrf *) mrf;
  MrfIo_setControlRegister_Expect(
    &impl->io, mrf_register_receive_mac_control, 0);
  Mac802154_disablePromiscuousMode(mrf);
}

void
test_useExtendedSourceAddress(void)
{
  Mrf *impl = (Mrf *) mrf;
  MrfIo_setControlRegister_Ignore();
  MrfIo_writeBlockingToShortAddress_Ignore();
  MrfIo_writeBlockingToLongAddress_Ignore();
  fakeDelay_Ignore();
  MrfState_init_Ignore();
  MrfState_setPanId_Ignore();
  MrfState_setShortSourceAddress_Ignore();
  MrfState_setExtendedDestinationAddress_Ignore();
  Mac802154_configure(mrf, &mac_config);
  MrfState_setExtendedSourceAddress_Expect(&impl->state,
                                           mac_config.extended_source_address);
  Mac802154_useExtendedSourceAddress(mrf);
}

void
test_useShortSourceAddress(void)
{
  Mrf *impl = (Mrf *) mrf;
  MrfIo_setControlRegister_Ignore();
  MrfIo_writeBlockingToShortAddress_Ignore();
  MrfIo_writeBlockingToLongAddress_Ignore();
  fakeDelay_Ignore();
  MrfState_init_Ignore();
  MrfState_setExtendedDestinationAddress_Ignore();
  MrfState_setPanId_Ignore();
  MrfState_setShortSourceAddress_Expect(&impl->state,
                                        mac_config.short_source_address);
  Mac802154_configure(mrf, &mac_config);
  MrfState_setShortSourceAddress_Expect(&impl->state,
                                        mac_config.short_source_address);
  Mac802154_useShortSourceAddress(mrf);
}

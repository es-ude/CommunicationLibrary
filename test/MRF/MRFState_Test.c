#include <string.h>
#include "unity.h"
#include "src/Mac802154/MockFrameHeader802154.h"
#include "src/Mac802154/MRF/MRFState.h"

static const uint8_t frame802_header_length = 10;
static MrfState mrf_state;

void debug(const uint8_t *message) {}

void setUp(void) {
  FrameHeader802154_init_Expect(&mrf_state.header.frame_header);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header,
                                                  frame802_header_length);
  MrfState_init(&mrf_state);
}

void test_initSetsPayloadLengthAndFrameHeaderLength(void) {
  FrameHeader802154_init_Ignore();
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(13);
  MrfState_init(&mrf_state);

  TEST_ASSERT_EQUAL_UINT8(13, mrf_state.header.frame_header_length);
  TEST_ASSERT_EQUAL_UINT8(mrf_state.header.frame_header_length,
                          mrf_state.header.frame_length);
}

void test_getFullSize(void) {
  TEST_ASSERT_EQUAL(frame802_header_length + 2, MrfState_getFullHeaderLength(&mrf_state));
}

void test_getFullSize2(void) {
  TEST_ASSERT_EQUAL_UINT8(frame802_header_length + 2, MrfState_getFullHeaderLength(&mrf_state));
}

void test_getFullHeaderData(void) {
  uint8_t payload[] = "hello, world!";
  uint8_t payload_length = (uint8_t) strlen((char *)payload);
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(frame802_header_length);
  MrfState_setPayload(&mrf_state, payload, payload_length);
  const uint8_t *data = MrfState_getFullHeaderData(&mrf_state);
  uint8_t expected[] = {frame802_header_length, frame802_header_length + payload_length};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, 2);
}

void test_getFullHeaderDataTwice(void) {
  uint8_t *payload = NULL;
  uint8_t payload_length = 4;
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(frame802_header_length);
  MrfState_setPayload(&mrf_state, payload, payload_length);
  MrfState_getFullHeaderData(&mrf_state);
  const uint8_t *data = MrfState_getFullHeaderData(&mrf_state);
  uint8_t expected[] = {frame802_header_length, frame802_header_length + payload_length};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, 2);
}

void test_accessHeaderDataAfterSettingPayloadWithoutCallingGetAgain(void) {
  uint8_t *payload = NULL;
  uint8_t payload_length = 6;
  MrfState_setPayload(&mrf_state, payload, payload_length);
  const uint8_t *data = MrfState_getFullHeaderData(&mrf_state);
  payload_length++;
  MrfState_setPayload(&mrf_state, payload, payload_length);
  uint8_t expected[] = {frame802_header_length, frame802_header_length + payload_length};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, 2);
}

void test_setShortDestinationAddress(void) {
  uint8_t address[2] = {0xCD, 0xED};
  FrameHeader802154_setShortDestinationAddress_Expect(&mrf_state.header.frame_header, address);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length);
  MrfState_setShortDestinationAddress(&mrf_state, address);
}

void test_setExtendedDestinationAddress(void) {
  uint8_t address[8] = {
      0x32, 0x43, 0x53, 0xA5,
      0x11, 0x34, 0x67, 0xF4,
  };
  FrameHeader802154_setExtendedDestinationAddress_Expect(&mrf_state.header.frame_header, address);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length);
  MrfState_setExtendedDestinationAddress(&mrf_state, address);
}

void test_setShortSourceAddress(void) {
  uint8_t address[2] = {
      0xAB, 0x43,
  };
  uint8_t frame_header_length = 55;
  FrameHeader802154_setShortSourceAddress_Expect(&mrf_state.header.frame_header, address);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame_header_length);
  MrfState_setShortSourceAddress(&mrf_state, address);
  TEST_ASSERT_EQUAL(frame_header_length + 2, MrfState_getFullHeaderLength(&mrf_state));
}

void
test_setExtendedSourceAddress(void)
{
  uint8_t address[8] = {
    0xAA, 0xBB,
    0xCA, 0xCD,
    0xDD, 0xDE,
    0xDD, 0xFF,
  };
  FrameHeader802154_setExtendedSourceAddress_Expect(&mrf_state.header.frame_header, address);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length + 6);
  MrfState_setExtendedSourceAddress(&mrf_state, address);
  uint8_t header_length = MrfState_getFullHeaderLength(&mrf_state);
  TEST_ASSERT_EQUAL(frame802_header_length+8, header_length);
}

void test_setDestinationPanId(void) {
  uint8_t pan_id[2] = {0x54, 0x11};
  FrameHeader802154_setPanId_Expect(&mrf_state.header.frame_header, pan_id);
  MrfState_setPanId(&mrf_state, pan_id);
}

void test_getPayload(void) {
  uint8_t payload[] = "helloworld";
  uint8_t payload_length = (uint8_t) strlen((const char*) payload);
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(10);
  MrfState_setPayload(&mrf_state, payload, payload_length);
  TEST_ASSERT_EQUAL_PTR(payload, MrfState_getPayload(&mrf_state));
}

void
test_getPayloadLength(void)
{
  uint8_t payload[] = "forthoseabouttorock";
  uint8_t payload_length = (uint8_t) strlen((char *)payload);
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(10);
  MrfState_setPayload(&mrf_state, payload, payload_length);
  TEST_ASSERT_EQUAL_UINT8(payload_length, MrfState_getPayloadLength(&mrf_state));
}

void
test_settingShortAndThenExtendedDestinationAddressResultsInCorrectSizeChange(void)
{
  uint8_t size_difference = 15;
  FrameHeader802154_setExtendedDestinationAddress_Expect(&mrf_state.header.frame_header, 0);
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length + size_difference);
  MrfState_setExtendedDestinationAddress(&mrf_state, 0);
  TEST_ASSERT_EQUAL_UINT8(frame802_header_length + size_difference + 2, MrfState_getFullHeaderLength(&mrf_state));
}


#define TEST_ASSERT_EQUAL_MRF_FIELD(expected_param___, actual_param___) \
  {\
    MrfField expected___ = expected_param___;\
    MrfField actual___ = actual_param___;\
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected___.data, actual___.data, "MrfField member data differs");\
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected___.length, actual___.length, "MrfField member length differs");\
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected___.address, actual___.address, "MrfField member address differs");\
  }


void
test_gettingFirstFieldAfterChangingDestinationAddressYieldsCompleteFrameHeader(void)
{

  MrfField expected = {
      .address = 0,
      .length = 77,
      .data = MrfState_getFullHeaderData(&mrf_state),
  };
  uint8_t short_address[2] =
      {
          0xAF, 0x5D,
      };
  FrameHeader802154_setShortDestinationAddress_Ignore();
  FrameHeader802154_getHeaderSize_ExpectAnyArgsAndReturn((uint8_t)(expected.length-2));
  MrfState_setShortDestinationAddress(&mrf_state, short_address);
  TEST_ASSERT_EQUAL_MRF_FIELD(expected, MrfState_getCurrentField(&mrf_state));
}

void
test_onlyOneFieldToGetAfterChangingDestinationAddress(void)
{
  FrameHeader802154_setShortDestinationAddress_Ignore();
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(15);
  uint8_t short_address[2] =
      {
          0xAB, 0x12,
      };
  MrfState_setShortDestinationAddress(&mrf_state, short_address);
  MrfState_moveIteratorToNextField(&mrf_state);
  TEST_ASSERT_FALSE(MrfState_moveIteratorToNextField(&mrf_state));
}


void
checkFirstFieldAfterChangingHeaderFieldYieldsCompleteFrameHeader(void (*field_setting_function) (void))
{
  MrfField expected;
  expected.address = 0;
  expected.data = MrfState_getFullHeaderData(&mrf_state);
  expected.length = 43;
  FrameHeader802154_setShortDestinationAddress_Ignore();
  FrameHeader802154_setExtendedDestinationAddress_Ignore();
  FrameHeader802154_setPanId_Ignore();
  FrameHeader802154_setSequenceNumber_Ignore();
  FrameHeader802154_getHeaderSize_ExpectAnyArgsAndReturn((uint8_t)(expected.length-2));
  field_setting_function();
  TEST_ASSERT_EQUAL_MRF_FIELD(expected, MrfState_getCurrentField(&mrf_state));
}

void
setDestinationAddress(void)
{
  uint8_t short_address[2] = {
      0x11, 0x22,
  };
  MrfState_setShortDestinationAddress(&mrf_state, short_address);
}

void
test_one(void)
{
  checkFirstFieldAfterChangingHeaderFieldYieldsCompleteFrameHeader(setDestinationAddress);
}

void
test_twoNewFieldsAfterChangingPayload(void)
{
  uint8_t payload[] = "mimimi";
  MrfState_setPayload(&mrf_state, payload, (uint8_t)strlen((char*)payload));
  MrfState_moveIteratorToNextField(&mrf_state);
  TEST_ASSERT_TRUE(MrfState_moveIteratorToNextField(&mrf_state))
}

void
test_enableAcknowledgement(void)
{
  FrameHeader802154_enableAcknowledgementRequest_Expect(&mrf_state.header.frame_header);
  MrfState_enableAcknowledgement(&mrf_state);
}

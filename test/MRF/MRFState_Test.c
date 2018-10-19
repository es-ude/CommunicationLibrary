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
  uint8_t payload_length = (uint8_t) strlen((const char *)payload);
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
  uint8_t  *payload = NULL;
  uint8_t payload_length = 6;
  MrfState_setPayload(&mrf_state, payload, payload_length);
  const uint8_t *data = MrfState_getFullHeaderData(&mrf_state);
  payload_length++;
  MrfState_setPayload(&mrf_state, payload, payload_length);
  uint8_t expected[] = {frame802_header_length, frame802_header_length + payload_length};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, 2);
}

void test_setShortDestinationAddress(void) {
  uint16_t address = 13;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length);
  FrameHeader802154_setShortDestinationAddress_Expect(&mrf_state.header.frame_header, address);
  MrfState_setShortDestinationAddress(&mrf_state, address);
}

void test_setExtendedDestinationAddress(void) {
  uint64_t address = 23;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length);
  FrameHeader802154_setExtendedDestinationAddress_Expect(&mrf_state.header.frame_header, address);
  MrfState_setExtendedDestinationAddress(&mrf_state, address);
}

void test_setShortSourceAddress(void) {
  uint16_t address = 5;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length);
  FrameHeader802154_setShortSourceAddress_Expect(&mrf_state.header.frame_header, address);
  MrfState_setShortSourceAddress(&mrf_state, address);
}

void test_setDestinationPanId(void) {
  uint16_t pan_id = 156;
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
  uint8_t payload_length = (uint8_t) strlen((const char*) payload);
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(10);
  MrfState_setPayload(&mrf_state, payload, payload_length);
  TEST_ASSERT_EQUAL_UINT8(payload_length, MrfState_getPayloadLength(&mrf_state));
}

void
test_settingShortAndThenExtendedDestinationAddressResultsInCorrectSizeChange(void)
{
  uint8_t payload[] = "some random string";
  uint8_t payload_length = (uint8_t) strlen((const char *) payload);
  uint8_t size_difference = 15;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, frame802_header_length + size_difference);
  FrameHeader802154_setExtendedDestinationAddress_Expect(&mrf_state.header.frame_header, 0);
  MrfState_setExtendedDestinationAddress(&mrf_state, 0);
  TEST_ASSERT_EQUAL_UINT8(frame802_header_length + size_difference + 2, MrfState_getFullHeaderLength(&mrf_state));
}

void
test_gettingFirstFieldAfterChangingDestinationAddressYieldsCompleteFrameHeader(void)
{

  MrfField expected = {
      .address = 0,
      .length = 77,
      .data = MrfState_getFullHeaderData(&mrf_state),
  };
  FrameHeader802154_setShortDestinationAddress_Ignore();
  FrameHeader802154_getHeaderSize_ExpectAnyArgsAndReturn(expected.length-2);
  MrfState_setShortDestinationAddress(&mrf_state, 5);
  TEST_ASSERT_EQUAL_UINT8(expected.address, MrfState_getCurrentField(&mrf_state).address);
  TEST_ASSERT_EQUAL_UINT8(expected.length, MrfState_getCurrentField(&mrf_state).length);
  TEST_ASSERT_EQUAL_PTR(expected.data, MrfState_getCurrentField(&mrf_state).data);
}

void
test_onlyOneFieldToGetAfterChangingDestinationAddress(void)
{
  FrameHeader802154_setShortDestinationAddress_Ignore();
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(15);
  uint8_t expected;
  MrfState_setShortDestinationAddress(&mrf_state, 5);
  MrfState_moveIteratorToNextField(&mrf_state);
  TEST_ASSERT_FALSE(MrfState_moveIteratorToNextField(&mrf_state));
}

void
test_twoNewFieldsAfterChangingPayload(void)
{
  uint8_t payload[] = "mimimi";
  MrfState_setPayload(&mrf_state, payload, (uint8_t)strlen(payload));
  MrfState_moveIteratorToNextField(&mrf_state);
  TEST_ASSERT_TRUE(MrfState_moveIteratorToNextField(&mrf_state))
}

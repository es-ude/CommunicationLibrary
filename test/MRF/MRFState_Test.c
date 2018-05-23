#include <string.h>
#include "unity.h"
#include "lib/src/Mac802154/MockFrameHeader802154.h"
#include "lib/src/Mac802154/MRF/MRFState.h"


void test_getFullSize(void) {
  MrfState mrf_state;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, 13);
  TEST_ASSERT_EQUAL(15, MrfState_getFullDataLength(&mrf_state));
}

void test_getFullSize2(void) {
  MrfState mrf_state;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(&mrf_state.header.frame_header, 34);
  TEST_ASSERT_EQUAL_UINT8(34+2, MrfState_getFullDataLength(&mrf_state));
}

void test_noNextField(void) {
  MrfState mrf_state = {.state = 0};
  TEST_ASSERT_FALSE(MrfState_nextField(&mrf_state));
}

void test_nextFieldAfterChangingShortAddress(void) {
  MrfState mrf_state = {.state = 0};
  MrfState_setShortDestinationAddress(&mrf_state);
  TEST_ASSERT_TRUE(MrfState_nextField(&mrf_state));
  TEST_ASSERT_FALSE(MrfState_nextField(&mrf_state));
}

void test_getFullHeaderData(void) {
  MrfState mrf_state;
  uint8_t payload[] = "hello, world!";
  uint8_t payload_length = (uint8_t) strlen((const char *)payload);
  MrfState_setPayload(&mrf_state, payload, payload_length);
  uint8_t frame802_header_length = 12;
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(frame802_header_length);
  const uint8_t *data = MrfState_getFullHeaderData(&mrf_state);
  uint8_t expected[] = {frame802_header_length, frame802_header_length + payload_length};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, 2);
}

void test_getFullHeaderDataTwice(void) {
  MrfState mrf_state;
  uint8_t *payload = NULL;
  uint8_t payload_length = 4;
  MrfState_setPayload(&mrf_state, payload, payload_length);
  uint8_t frame802_header_length = 15;
  FrameHeader802154_getHeaderSize_IgnoreAndReturn(frame802_header_length);
  MrfState_getFullHeaderData(&mrf_state);
  const uint8_t *data = MrfState_getFullHeaderData(&mrf_state);
  uint8_t expected[] = {frame802_header_length, frame802_header_length + payload_length};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, 2);
}
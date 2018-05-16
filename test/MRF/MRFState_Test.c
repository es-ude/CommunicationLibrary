#include "unity.h"
#include "lib/src/Mac802154/MockFrameHeader802154.h"
#include "lib/src/Mac802154/MRF/MRFState.h"


void test_getFullSize(void) {
  MrfState mrf_state;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(mrf_state.header, 13);
  TEST_ASSERT_EQUAL(15, MrfState_getCurrentFieldsSize(&mrf_state));
}

void test_getFullSize2(void) {
  MrfState mrf_state;
  FrameHeader802154_getHeaderSize_ExpectAndReturn(mrf_state.header, 34);
  TEST_ASSERT_EQUAL_UINT8(34+2, MrfState_getCurrentFieldsSize(&mrf_state));
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

void test_nextFieldAfterChangingFromShortToLongAddress(void) {
  MrfState mrf_state = {.state = 0};
  
}
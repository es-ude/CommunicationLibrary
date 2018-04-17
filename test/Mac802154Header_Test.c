#include "unity.h"
#include "lib/src/Mac802154/FrameHeader802154.h"
#include "CException.h"
#include "lib/include/Exception.h"

static uint8_t header_data[MAXIMUM_HEADER_SIZE];
static FrameHeader802154_t *header = (FrameHeader802154_t *)header_data;
static const uint8_t pan_id_compression_bit = 6;
static const uint8_t minimum_packet_size = 2;

void setUp(void) {
  FrameHeader802154_init(header);
}

void test_setSequenceNumber(void) {
  uint8_t number;
  FrameHeader802154_setSequenceNumber(header, number);
  TEST_ASSERT_EQUAL_UINT8(number, *FrameHeader802154_getSequenceNumberPtr(header));
}

void test_getPanId(void) {
  TEST_ASSERT_EQUAL_UINT(FrameHeader802154_getSequenceNumberPtr(header) + 1,
    FrameHeader802154_getPanIdPtr(header)
  );
}

void test_setPanId(void) {
  uint16_t pan_id = 0xABCD;
  FrameHeader802154_setPanId(header, pan_id);
  uint8_t expected[2] = {0xAB, 0xCD};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getPanIdPtr(header), 2);
}

void test_setPanIdSecond(void) {
  uint16_t pan_id = 0xF3D2;
  FrameHeader802154_setPanId(header, pan_id);
  uint8_t expected[2] = {(uint8_t) (pan_id >> 8), (uint8_t) pan_id};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getPanIdPtr(header), 2);
}

void test_getSizeOfEmptyHeader(void) {
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getHeaderSize(header));
}

void test_getSizeOfHeaderWithSequenceNumber(void) {
  FrameHeader802154_setSequenceNumber(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint8_t), FrameHeader802154_getHeaderSize(header));
}

void test_getSizeOfHeaderWithShortSourceAddress(void) {
  FrameHeader802154_setShortSourceAddress(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint16_t), FrameHeader802154_getHeaderSize(header));
}

void test_getSizeOfHeaderWithLongSourceAddress(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint64_t), FrameHeader802154_getHeaderSize(header));
}

void test_getSizeOfHeaderWithShortDestinationAddress(void) {
  FrameHeader802154_setShortDestinationAddress(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint16_t), FrameHeader802154_getHeaderSize(header));
}

void test_getSizeOfHeaderWithShortDestinationExtendedSourceAddressAndSequenceNumber(void) {
  FrameHeader802154_setShortDestinationAddress(header, 0);
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  FrameHeader802154_setSequenceNumber(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint16_t) + sizeof(uint64_t) + sizeof(uint8_t), FrameHeader802154_getHeaderSize(header));
}

void test_getSizeOfHeaderWhenPanIdIsSet(void) {
  FrameHeader802154_setPanId(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint16_t), FrameHeader802154_getHeaderSize(header));
}

void test_panIdCompressionIsDisabledWhenOnlyPanIdAndDestinationAddressAreSet(void) {
  FrameHeader802154_setShortDestinationAddress(header, 0);
  FrameHeader802154_setPanId(header, 0);
  TEST_ASSERT_BIT_LOW(pan_id_compression_bit, header_data[0]);
}

void test_getSizeOfHeaderForPanIdAndDestinationAddress(void) {
  FrameHeader802154_setPanId(header, 0);
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint16_t) + sizeof(uint64_t), FrameHeader802154_getHeaderSize(header));
}

void test_getSizeAfterSettingExtendedSourceAddress(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  TEST_ASSERT_EQUAL(minimum_packet_size + sizeof(uint64_t), FrameHeader802154_getHeaderSize(header));
}


void test_setupSequencePanIdShortSourceAddressRaisesException(void) {
  FrameHeader802154_setPanId(header, 0);
  CEXCEPTION_T exception;
  Try{
        FrameHeader802154_setShortSourceAddress(header, 0);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setupSequencePanIdExtendedSourceAddressRaisesException(void) {
  FrameHeader802154_setPanId(header, 0);
  CEXCEPTION_T exception;
  Try{
        FrameHeader802154_setExtendedSourceAddress(header, 0);
        TEST_FAIL();
      } Catch(exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setupSequenceShortSourceAddressExtendedDestinationAddress(void) {
  FrameHeader802154_setShortSourceAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setExtendedDestinationAddress(header, 0);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}


void test_setupSequenceShortDestinationExtendedSourceRaisesException(void) {
  FrameHeader802154_setShortDestinationAddress(header, 0);
  CEXCEPTION_T exception;
  Try{
        FrameHeader802154_setExtendedSourceAddress(header, 0);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setupExtendedSourceShortDestinationAddress(void){
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setShortDestinationAddress(header, 0);
        TEST_FAIL();
      } Catch(exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setupExtendedDestinationShortSourceAddress(void) {
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setShortSourceAddress(header, 0);
        TEST_FAIL();
      } Catch(exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setupShortDestinationShortSourceAddress(void) {
  FrameHeader802154_setShortDestinationAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setShortSourceAddress(header, 0);
        TEST_FAIL();
      } Catch(exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setupSequenceExtendedSourceAddressExtendedDestinationAddress(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setExtendedDestinationAddress(header, 0);
      } Catch(exception) {
    TEST_FAIL();
  }
}

void test_setupSequenceExtendedDestinationExtendedSource(void) {
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  CEXCEPTION_T exception;
  Try{
        FrameHeader802154_setExtendedSourceAddress(header, 0);
      } Catch (exception) {
    TEST_FAIL();
  }
}

void test_setShortSourcePanId(void) {
  FrameHeader802154_setShortSourceAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setPanId(header, 0);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setExtendedSourcePanId(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  CEXCEPTION_T exception;
  Try {
        FrameHeader802154_setPanId(header, 0);
        TEST_FAIL();
      } Catch (exception) {
    TEST_ASSERT_EQUAL(FRAMEHEADER802154_INVALID_EXCEPTION, exception);
  }
}

void test_setExtendedAddressesAndPanId(void) {
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  FrameHeader802154_setPanId(header, 0);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, header_data[0]);
}
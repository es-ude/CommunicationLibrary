#include <string.h>
#include "unity.h"
#include "lib/src/Mac802154/FrameHeader802154.h"
#include "CException.h"
#include "lib/include/Exception.h"
#include "lib/src/BitManipulation.h"

/**
 * Currently we assume the network byte order for pan ids and addresses to be big endian.
 * In case this assumption is not true, it might make sense to use a struct with multiple fields
 * for the header and convert that struct to an array with the correct byte order as needed, i.e.
 * on the layer above (that knows what network device is used).
 *
 * [[Regarding the retrieval of field values: it might be best to only getNextFieldPtr and getNextFieldSize
 * instead of get<InsertFieldNameHere>Ptr(), this way we would not expose the inner structure
 * and semantics of the header.]]
 * This iterator functionality belongs into another module because
 * the necessary fields might differ on a network chip base, opposed
 * to this 802154 header that should only change for performance reasons.
 *
 **/

static uint8_t header_data[MAXIMUM_HEADER_SIZE];
static FrameHeader802154 *header = (FrameHeader802154 *)header_data;
static const uint8_t pan_id_compression_bit = 6;
static const uint8_t minimum_packet_size = 2;
static const uint8_t fresh_header_size = 2 + 2 + 2 + 2;

void debug(const uint8_t *msg){}

void setUp(void) {
  FrameHeader802154_init(header);
}

void test_controlFieldIsCorrectlyInitialized(void) {
  /*
   * This is supposed to be a standard default header
   * control field setting:
   * Frame Type => Data Frame
   * Security => disabled
   * Frame Pending => false
   * Ack Request => true
   * PAN ID Compression => true
   * Sequence Number suppression => false
   * Information Element Present => false
   * Destination Addressing Mode => short (16bit)
   * Frame Version => 802.15.4-2015
   * Source Addressing Mode => short (16bit)
   *
   * When comparing the value below to the values
   * from the IEE Std. bear in mind that the bit strings
   * are represented in reverse order there, i.e. starting
   * with the lsb.
   */
  uint8_t default_control_field[] = {
          0b01100001,
          0b10101000
  };



  TEST_ASSERT_EQUAL_HEX8_ARRAY(default_control_field, header_data, 2);
}

void test_setSequenceNumber(void) {
  uint8_t number;
  FrameHeader802154_setSequenceNumber(header, number);
  TEST_ASSERT_EQUAL_UINT8(number, *FrameHeader802154_getSequenceNumberPtr(header));
}

void test_getSequenceNumberSizeWhenPresent(void) {
  uint8_t number;
  FrameHeader802154_setSequenceNumber(header, number);
  TEST_ASSERT_EQUAL_UINT8(1, FrameHeader802154_getSequenceNumberSize(header));
}

void test_getSequenceNumberSizeWhenNotPresent(void) {
  FrameHeader802154_enableSequenceNumberSuppression(header);
  TEST_ASSERT_EQUAL_UINT8(0, FrameHeader802154_getSequenceNumberSize(header));
}

void test_getPanIdWithoutSequenceNumber(void) {
  FrameHeader802154_enableSequenceNumberSuppression(header);
  TEST_ASSERT_EQUAL_PTR(FrameHeader802154_getSequenceNumberPtr(header),
    FrameHeader802154_getPanIdPtr(header)
  );
}

void test_setPanId(void) {
  uint16_t pan_id = 0xABCD;
  FrameHeader802154_setPanId(header, pan_id);
  uint8_t expected[2] = {0xCD, 0xAB};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getPanIdPtr(header), 2);
}

void test_setPanIdSecond(void) {
  uint16_t pan_id = 0xF3D2;
  FrameHeader802154_setPanId(header, pan_id);
  uint8_t expected[2] = {0xD2, 0xF3};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getPanIdPtr(header), 2);
}

void test_getSizeOfFreshHeader(void) {
  uint8_t short_source_address_size = 2;
  uint8_t short_destination_address_size = 2;
  uint8_t pan_id_size = 2;
  uint8_t control_field_size = 2;
  uint8_t sequence_number_size = 1;
  uint8_t total_size = short_destination_address_size +
          short_source_address_size + pan_id_size +
          control_field_size + sequence_number_size;
  TEST_ASSERT_EQUAL(total_size, FrameHeader802154_getHeaderSize(header));
}

void test_getSizeWithSequenceNumber(void) {
  uint8_t short_source_address_size = 2;
  uint8_t short_destination_address_size = 2;
  uint8_t pan_id_size = 2;
  uint8_t control_field_size = 2;
  uint8_t sequence_number_size = 1;
  uint8_t total_size = short_destination_address_size +
                       short_source_address_size + pan_id_size +
                       control_field_size + sequence_number_size;
  FrameHeader802154_setSequenceNumber(header, 0);
  TEST_ASSERT_EQUAL(total_size, FrameHeader802154_getHeaderSize(header));
}

void test_setShortAddressesAndPanId(void) {
  FrameHeader802154_setPanId(header, 0);
  FrameHeader802154_setShortDestinationAddress(header, 0);
  FrameHeader802154_setShortSourceAddress(header, 0);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, header_data[0]);
}

void test_getSizeOfShortSourceAddress(void) {
  FrameHeader802154_setShortSourceAddress(header, 0);
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getSourceAddressSize(header));
}

void test_getSizeOfExtendedSourceAddress(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  TEST_ASSERT_EQUAL(8, FrameHeader802154_getSourceAddressSize(header));
}

void test_getSizeOfDestinationAddress(void) {
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getDestinationAddressSize(header));
}

void test_getSizeOfShortDestinationAddress(void) {
  FrameHeader802154_setShortDestinationAddress(header, 0);
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getDestinationAddressSize(header));
}

void test_getShortDestinationAddress(void) {
  uint16_t address = 0xABCD;
  uint8_t address_in_network_byte_order[2] = {0xCD, 0xAB};
  FrameHeader802154_setShortDestinationAddress(header, address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(address_in_network_byte_order,
                               FrameHeader802154_getDestinationAddressPtr(header),
                               FrameHeader802154_getDestinationAddressSize(header));
}

void test_getPanIdAndShortDestinationAddress(void) {
  uint16_t pan_id = 0xABCD;
  uint16_t address = 0xEF12;
  uint8_t expected[] = {0xCD, 0xAB, 0x12, 0xEF};
  FrameHeader802154_setPanId(header, pan_id);
  FrameHeader802154_setShortDestinationAddress(header, address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header),
                               FrameHeader802154_getDestinationAddressSize(header) + FrameHeader802154_getPanIdSize(header));
}

void test_getPanIdAndShortDestinationAddress2(void) {
  uint16_t  pan_id = 0xCDEF;
  uint16_t address = 0x1234;
  uint8_t expected[] = {0xEF, 0xCD, 0x34, 0x12};
  FrameHeader802154_setShortDestinationAddress(header, address);
  FrameHeader802154_setPanId(header, pan_id);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header), 4);
}

void test_getPanIdAndExtendedDestinationAddress(void) {
  uint16_t pan_id = 0x1234;
  uint64_t address = 0xABCDEF123456789A;
  uint8_t expected[] = {0x34, 0x12, 0x9A, 0x78, 0x56, 0x34, 0x12, 0xEF, 0xCD, 0xAB};
  FrameHeader802154_setExtendedDestinationAddress(header, address);
  FrameHeader802154_setPanId(header, pan_id);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header), 10);
}

void test_setDestinationFirstAndSourceSecond(void) {
  // without a pan id source and destination address have to be extended addresses
  uint64_t source_address = 0x1122334455667788;
  uint64_t destination_address = 0xAABBCCDD33557799;
  uint8_t expected[] = {
          0x99, 0x77, 0x55, 0x33, 0xDD, 0xCC, 0xBB, 0xAA,
          0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
  };
  FrameHeader802154_setExtendedSourceAddress(header, source_address);
  FrameHeader802154_setExtendedDestinationAddress(header, destination_address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getDestinationAddressPtr(header), 16
  );
}

void test_setPanIdAfterExtendedDestinationAndSourceAddress(void) {
  uint64_t source_address = 0x1122334455667788;
  uint64_t destination_address = 0xAABBCCDD33557799;
  uint16_t pan_id = 0xFFEE;
  uint8_t expected[] = {
          0xEE, 0xFF,
          0x99, 0x77, 0x55, 0x33, 0xDD, 0xCC, 0xBB, 0xAA,
          0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
  };
  FrameHeader802154_setExtendedSourceAddress(header, source_address);
  FrameHeader802154_setExtendedDestinationAddress(header, destination_address);
  FrameHeader802154_setPanId(header, pan_id);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header),
                               18
  );
}

void test_changeDestinationAddressFromLongToShort(void) {
  uint64_t destination_address = 0x55667788AABBCCDD;
  uint16_t source_address = 0x1122;
  uint16_t new_destination_address = 0x3344;
  uint8_t expected[] = {
          0x44, 0x33,
          0x22, 0x11,
  };
  FrameHeader802154_setExtendedDestinationAddress(header, destination_address);
  FrameHeader802154_setShortSourceAddress(header, source_address);
  FrameHeader802154_setShortDestinationAddress(header, new_destination_address);

  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getDestinationAddressPtr(header), 4);
}

void test_setExtendedDestinationTwice(void) {
  uint64_t source_address = 0x1122;
  uint64_t new_destination_address = 0x5566778899AABBCC;
  uint8_t expected[16];
  BitManipulation_fillByteArrayWith64BitLittleEndian(expected, new_destination_address);
  BitManipulation_fillByteArrayWith64BitLittleEndian(expected + 8, source_address);
  FrameHeader802154_setExtendedSourceAddress(header, source_address);
  FrameHeader802154_setExtendedDestinationAddress(header, new_destination_address);
  FrameHeader802154_setExtendedDestinationAddress(header, new_destination_address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getDestinationAddressPtr(header), 16);
}

void checkForSetAllFields(uint8_t permutation[]) {
  uint8_t expected[19];
  for (uint8_t index = 0; index < 19; index++)
  {
    expected[index] = 0xff;
  }
  expected[0] = 1;
  char text[32];
  sprintf(text, "failed on permutation {%d, %d, %d, %d}", permutation[0], permutation[1], permutation[2], permutation[3]);
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, FrameHeader802154_getSequenceNumberPtr(header), 19, text);
}


void callFrameHeaderSetter(uint8_t function_id) {
  switch (function_id) {
    case 0:
      FrameHeader802154_setSequenceNumber(header, 1);
      break;
    case 1:
      FrameHeader802154_setPanId(header, ~0);
      break;
    case 2:
      FrameHeader802154_setExtendedDestinationAddress(header, ~0);
      break;
    case 3:
      FrameHeader802154_setShortDestinationAddress(header, ~0);
      break;
    default:
      break;
  }
}

//TODO: Test this function as well
bool PermuteArray(uint8_t array[], uint8_t size) {
  uint8_t left_index = 0;
  while (left_index < size-1 && array[left_index] >= array[left_index+1]) {
    left_index++;
  }
  if (left_index == size-1 && array[left_index] >= array[left_index+1]) return true;
  uint8_t right_index = left_index+1;
  while (right_index < size && array[left_index] >= array[right_index]) {
    right_index++;
  }
  if (right_index < size && array[right_index] < array[left_index]) {
    uint8_t temp = array[right_index];
    array[right_index] = array[left_index];
    array[left_index] = temp;

    for (uint8_t i = left_index + 1; i < (size-left_index-1)/2; i++) {
      temp = array[i];
      array[i] = array[size+i-left_index-1];
      array[size+i-left_index-1] = temp;
    }
  }
  return false;

}

void runFrameHeaderSetterTest(uint8_t function_order[]) {
  setUp();
  for (uint8_t i = 0; i < 4; i++) {
    callFrameHeaderSetter(function_order[i]);
  }
  checkForSetAllFields(function_order);
}

void test_setAllFields1(void) {
  uint8_t function_id_array[4] = {0, 1, 2, 3};
  do {
    callFrameHeaderSetter(function_id_array);
  } while (PermuteArray(function_id_array, 4));
}


void test_getSizeWhenMaximum(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  FrameHeader802154_setSequenceNumber(header, 0);
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  TEST_ASSERT_EQUAL_UINT8(21, FrameHeader802154_getHeaderSize(header));
}

void test_panIdCompressionDisabledWhenBothAddressesAreExtended(void) {
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  TEST_ASSERT_BIT_LOW(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}

void test_panIdCompressionDisabledWhenBothAddressesAreExtended2(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  TEST_ASSERT_BIT_LOW(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}

void test_panIdCompressionGetsEnabledWhenNecessary(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  FrameHeader802154_setShortSourceAddress(header, 0);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}

void test_panIdCompressionGetsEnabledWhenNecessary2(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  FrameHeader802154_setExtendedDestinationAddress(header, 0);
  FrameHeader802154_setShortDestinationAddress(header, 0);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}


void test_sizeOfHeaderForExtendedDestination(void) {
  FrameHeader802154_setExtendedSourceAddress(header, 0);
  TEST_ASSERT_EQUAL_UINT8(15, FrameHeader802154_getHeaderSize(header));
}
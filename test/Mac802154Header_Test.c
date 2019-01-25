#include "unity.h"
#include "src/Mac802154/MRF/FrameHeader802154.h"
#include "src/Util/BitManipulation.h"

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
static const uint8_t extended_zeros_address[] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
};

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
   * Ack Request => false
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
          0b01000001,
          0b10101000
  };



  TEST_ASSERT_EQUAL_HEX8_ARRAY(default_control_field, header_data, 2);
}

void test_setSequenceNumber(void) {
  uint8_t number = 23;
  FrameHeader802154_setSequenceNumber(header, number);
  TEST_ASSERT_EQUAL_UINT8(number, *FrameHeader802154_getSequenceNumberPtr(header));
}

void test_getSequenceNumberSizeWhenPresent(void) {
  uint8_t number = 234;
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
  uint8_t expected[2] = {0xCD, 0xAB};
  FrameHeader802154_setPanId(header, expected);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getPanIdPtr(header), 2);
}

void test_setPanIdSecond(void) {
  uint8_t expected[2] = {0xD2, 0xF3};
  FrameHeader802154_setPanId(header, expected);
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
  uint8_t zeros[2] = {0, 0};
  FrameHeader802154_setPanId(header, zeros);
  FrameHeader802154_setShortDestinationAddress(header, zeros);
  FrameHeader802154_setShortSourceAddress(header, zeros);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, header_data[0]);
}

void test_getSizeOfShortSourceAddress(void) {
  uint8_t source_address[] = {0, 0};
  FrameHeader802154_setShortSourceAddress(header, source_address);
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getSourceAddressSize(header));
}

void test_getSizeOfExtendedSourceAddress(void) {
  FrameHeader802154_setExtendedSourceAddress(header, extended_zeros_address);
  TEST_ASSERT_EQUAL(8, FrameHeader802154_getSourceAddressSize(header));
}

void test_getSizeOfDestinationAddress(void) {
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getDestinationAddressSize(header));
}

void test_getSizeOfShortDestinationAddress(void) {
  uint8_t address[2] = {0x00, 0x00};
  FrameHeader802154_setShortDestinationAddress(header, address);
  TEST_ASSERT_EQUAL(2, FrameHeader802154_getDestinationAddressSize(header));
}

void test_getShortDestinationAddress(void) {
  uint8_t address_in_network_byte_order[2] = {0xCD, 0xAB};
  uint8_t *address = address_in_network_byte_order;
  FrameHeader802154_setShortDestinationAddress(header, address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(address_in_network_byte_order,
                               FrameHeader802154_getDestinationAddressPtr(header),
                               FrameHeader802154_getDestinationAddressSize(header));
}

void test_getPanIdAndShortDestinationAddress(void) {
  uint8_t expected[] = {0xCD, 0xAB, 0x12, 0xEF};
  uint8_t *address = expected+2;
  FrameHeader802154_setPanId(header, expected);
  FrameHeader802154_setShortDestinationAddress(header, address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header),
                               FrameHeader802154_getDestinationAddressSize(header) + FrameHeader802154_getPanIdSize(header));
}

void test_getPanIdAndShortDestinationAddress2(void) {
  uint8_t expected[] = {0xEF, 0xCD, 0x34, 0x12};
  uint8_t *address = expected + 2;
  FrameHeader802154_setShortDestinationAddress(header, address);
  FrameHeader802154_setPanId(header, expected);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header), 4);
}

void test_getPanIdAndExtendedDestinationAddress(void) {
  uint8_t expected[] = {0x34, 0x12, 0x9A, 0x78, 0x56, 0x34, 0x12, 0xEF, 0xCD, 0xAB};
  uint8_t *address = expected+2;
  FrameHeader802154_setExtendedDestinationAddress(header, address);
  FrameHeader802154_setPanId(header, expected);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header), 10);
}

void test_setDestinationFirstAndSourceSecond(void) {
  // without a pan id source and destination address have to be extended addresses
  uint8_t expected[] = {
          0x99, 0x77, 0x55, 0x33, 0xDD, 0xCC, 0xBB, 0xAA,
          0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
  };
  const uint8_t *source_address = expected + 8;
  uint8_t *destination_address = expected;
  FrameHeader802154_setExtendedSourceAddress(header, source_address);
  FrameHeader802154_setExtendedDestinationAddress(header, destination_address);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getDestinationAddressPtr(header), 16
  );
}

void test_setPanIdAfterExtendedDestinationAndSourceAddress(void) {

  uint8_t expected[] = {
          0xEE, 0xFF,
          0x99, 0x77, 0x55, 0x33, 0xDD, 0xCC, 0xBB, 0xAA,
          0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
  };
  const uint8_t *destination_address = expected + 2;
  const uint8_t *source_address = destination_address + 8;
  FrameHeader802154_setExtendedSourceAddress(header, source_address);
  FrameHeader802154_setExtendedDestinationAddress(header, destination_address);
  FrameHeader802154_setPanId(header, expected);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               FrameHeader802154_getPanIdPtr(header),
                               18
  );
}

void test_changeDestinationAddressFromLongToShort(void) {
  uint8_t destination_address[] = {
      0x55, 0x66,
      0x77, 0x88,
      0xAA, 0xBB,
      0xCC, 0xDD,
  };

  uint8_t expected[] = {
          0x44, 0x33,
          0x22, 0x11,
  };
  uint8_t *source_address = expected+2;
  FrameHeader802154_setExtendedDestinationAddress(header, destination_address);
  FrameHeader802154_setShortSourceAddress(header, source_address);
  FrameHeader802154_setShortDestinationAddress(header, expected);

  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getDestinationAddressPtr(header), 4);
}

void test_setExtendedDestinationTwice(void) {
  uint8_t source_address[] = {
      0x11, 0x22,
      0x33, 0x55,
      0x13, 0x34,
      0x11, 0x00,
  };
  uint8_t expected[8] = {0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
  FrameHeader802154_setExtendedSourceAddress(header, source_address);
  FrameHeader802154_setExtendedDestinationAddress(header, expected);
  FrameHeader802154_setExtendedDestinationAddress(header, expected);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, FrameHeader802154_getDestinationAddressPtr(header), 8);
}

void
checkForSetAllFields(uint8_t permutation[])
{
  uint8_t expected[19];
  for (uint8_t index = 0; index < 19; index++)
  {
    expected[index] = 0xff;
  }
  expected[0] = 1;
  char text[64];
  sprintf(text, "failed on permutation {%d, %d, %d, %d}", permutation[0], permutation[1], permutation[2], permutation[3]);
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, FrameHeader802154_getSequenceNumberPtr(header), 19, text);
}

void
swapBytes(uint8_t *first, uint8_t *second)
{
  uint8_t temp = *first;
  *first = *second;
  *second = temp;
}

static uint8_t findLeftIndexForPermutation(const uint8_t array[], uint8_t size) {
  uint8_t left_index = size;
  for (int8_t i = size-2; i != 0; i--){
    if (array[i] < array[i+1]) {
      left_index = i;
      break;
    }
  }
  return left_index;
}

static uint8_t findRightIndexForPermutation(const uint8_t array[], uint8_t size, uint8_t left_index) {
  uint8_t right_index = size;
  for (int8_t i = size-1; i>= 0; i--) {
    if (array[left_index] < array[i]) {
      right_index = i;
      break;
    }
  }
  return right_index;
}

static void reverseArrayFromToEnd(uint8_t array[], uint8_t size, uint8_t from) {
  for (int8_t i = 0; i < (size-from)/2; i++) {
    swapBytes(array+from+i, array+size-1-i);
  }
}

bool permute(uint8_t array[], uint8_t size) {
  uint8_t left_index = findLeftIndexForPermutation(array, size);
  if (left_index == size)
    return false;
  int8_t right_index = findRightIndexForPermutation(array, size, left_index);
  swapBytes(array+left_index, array+right_index);
  reverseArrayFromToEnd(array, size, left_index+1);
  return true;
}

void test_arrayPermutationHelper1(void) {
  uint8_t array[4] = {0,1,2,3};
  uint8_t expected[4] = {0, 1, 3, 2};
  permute(array, 4);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, array, 4);
}

void test_arrayPermutationHelper2(void) {
  uint8_t array[4] = {0,1,3,2};
  uint8_t expected[4] = {0,2,1,3};
  permute(array, 4);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, array, 4);
}

void test_arrayPermutationHelper3(void) {
  uint8_t array[4] = {0,2,1,3};
  uint8_t expected[4] = {0, 2, 3, 1};
  permute(array, 4);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, array, 4);
}

void test_arrayPermutationHelper4(void) {
  uint8_t array[4] = {3,2,1,0};
  uint8_t expected[4] = {3,2,1,0};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, array, 4);
}

void callFrameHeaderSetter(uint8_t function_id) {
  uint8_t expected_pan_id[2] = {0xFF, 0xFF};
  uint8_t expected_extended_address[] = {
      0xFF, 0xFF,
      0xFF, 0xFF,
      0xFF, 0xFF,
      0xFF, 0xFF,
  };
  switch (function_id) {
    case 0:
      FrameHeader802154_setSequenceNumber(header, 1);
      break;
    case 1:
      FrameHeader802154_setPanId(header, expected_pan_id);
      break;
    case 2:
      FrameHeader802154_setExtendedDestinationAddress(header, expected_extended_address);
      break;
    case 3:
      FrameHeader802154_setExtendedSourceAddress(header, expected_extended_address);
      break;
    default:
      break;
  }
}

void runFrameHeaderSetterTest(uint8_t function_order[]) {
  setUp();
  for (uint8_t i = 0; i < 4; i++) {
    callFrameHeaderSetter(function_order[i]);
  }
  checkForSetAllFields(function_order);
}

void test_setFrameInAnyOrder(void) {
  uint8_t function_order[] = {0,1,2,3};
  do {
    runFrameHeaderSetterTest(function_order);
  } while (permute(function_order, 4));
}

void test_getSizeWhenMaximum(void) {
  uint8_t extended_address[8];
  BitManipulation_fillArray(extended_address, 0, 8);
  FrameHeader802154_setExtendedSourceAddress(header, extended_address);
  FrameHeader802154_setSequenceNumber(header, 0);
  uint8_t address[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  FrameHeader802154_setExtendedDestinationAddress(header, address);
  TEST_ASSERT_EQUAL_UINT8(21, FrameHeader802154_getHeaderSize(header));
}

void test_panIdCompressionDisabledWhenBothAddressesAreExtended(void) {
  FrameHeader802154_setExtendedDestinationAddress(header, extended_zeros_address);
  FrameHeader802154_setExtendedSourceAddress(header, extended_zeros_address);
  TEST_ASSERT_BIT_LOW(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}

void test_panIdCompressionDisabledWhenBothAddressesAreExtended2(void) {
  FrameHeader802154_setExtendedSourceAddress(header, extended_zeros_address);
  FrameHeader802154_setExtendedDestinationAddress(header, extended_zeros_address);
  TEST_ASSERT_BIT_LOW(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}

void test_panIdCompressionGetsEnabledWhenNecessary(void) {
  FrameHeader802154_setExtendedSourceAddress(header, extended_zeros_address);
  FrameHeader802154_setExtendedDestinationAddress(header, extended_zeros_address);
  FrameHeader802154_setShortSourceAddress(header, extended_zeros_address);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}

void test_panIdCompressionGetsEnabledWhenNecessary2(void) {
  FrameHeader802154_setExtendedSourceAddress(header, extended_zeros_address);
  FrameHeader802154_setExtendedDestinationAddress(header, extended_zeros_address);
  FrameHeader802154_setShortDestinationAddress(header, extended_zeros_address);
  TEST_ASSERT_BIT_HIGH(pan_id_compression_bit, *FrameHeader802154_getHeaderPtr(header));
}


void test_sizeOfHeaderForExtendedDestination(void) {
  FrameHeader802154_setExtendedSourceAddress(header, extended_zeros_address);
  TEST_ASSERT_EQUAL_UINT8(15, FrameHeader802154_getHeaderSize(header));
}

void test_getSourceAddressPtr(void) {
  uint8_t expected[] = {0xBB, 0xAA};
  FrameHeader802154_setShortSourceAddress(header, expected);
  const uint8_t *source_address = FrameHeader802154_getSourceAddressPtr(header);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, source_address, 2);
}

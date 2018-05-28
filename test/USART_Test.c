#include "unity.h"
#include "lib/include/usart/USART.h"
#include "lib/include/platform/io.h"

void setUp(void) {
    resetDummyIO();
}

void test_initSetsBaudRate(void) {
    uint16_t baud_rate = 9600;
    USART_init(baud_rate);
    uint16_t expected_baud_rate_register_value = (uint16_t)(cpu_frequency/(16 * baud_rate) - 1);
    TEST_ASSERT_EQUAL_UINT8(expected_baud_rate_register_value >> 8, *usart_baud_rate_register_high);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)(expected_baud_rate_register_value), *usart_baud_rate_register_low);
}

void test_initEnablesTransmitter(void) {
    USART_init(9600);
    TEST_ASSERT_BIT_HIGH(usart_transmitter_enable_bit, *usart_control_and_status_register_b);
}

void test_initDoesNotTouchOtherValuesInStatusControlRegisterB(void) {
    uint8_t some_arbitrary_value = 0xAC;
    *usart_control_and_status_register_b = some_arbitrary_value;
    USART_init(9600);
    TEST_ASSERT_BITS(~(1 << usart_transmitter_enable_bit),
                     some_arbitrary_value,
                     *usart_control_and_status_register_b);
}

void test_initUses8BitCharacterSize(void) {
    USART_init(9600);
    TEST_ASSERT_BITS_HIGH( 0b11 << usart_character_size_offset,
                      *usart_control_and_status_register_c);
}

void test_initDoesNotTouchUnncessaryValuesInControlRegisterC(void) {
    *usart_control_and_status_register_c = 0xFB;
    USART_init(9600);
    TEST_ASSERT_BITS( ~(0b11 << usart_character_size_offset),
                      0xFB, *usart_control_and_status_register_c);
}


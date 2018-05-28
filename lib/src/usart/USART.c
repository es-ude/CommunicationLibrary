#include "lib/include/usart/USART.h"
#include "lib/include/platform/io.h"
#include <stdbool.h>

static void setBaudRate(uint32_t baud_rate);
static void enableTransmitter(void);
static void set8BitCharacterSize(void);
static bool notReady(void);

void USART_init(uint32_t baud_rate) {
    setBaudRate(baud_rate);
    enableTransmitter();
    set8BitCharacterSize();
}

void USART_write(uint8_t byte) {
    while( notReady() ) {}
    *usart_data_register = byte;
}

void USART_writeN(uint8_t *buffer, uint16_t length) {
    uint16_t index = 0;
    while(index < length){
        USART_write(buffer[index++]);
    }
}

void setBaudRate(uint32_t baud_rate) {
    uint16_t baud_rate_register_value = (uint16_t)(cpu_frequency/16/baud_rate - 1);
    *usart_baud_rate_register_high = (uint8_t) (baud_rate_register_value >> 8);
    *usart_baud_rate_register_low = (uint8_t) baud_rate_register_value;
}

void enableTransmitter(void) {
    *usart_control_and_status_register_b |= 1 << 3;
}

void set8BitCharacterSize(void) {
    uint8_t usart_8bit_character_size = 0b11 << usart_character_size_offset;
    *usart_control_and_status_register_c |= usart_8bit_character_size;
}

bool notReady(void) {
    return !(*usart_control_and_status_register_a & (1 << usart_data_register_empty));

}

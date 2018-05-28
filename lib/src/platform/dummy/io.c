#include <stdint.h>
#include "lib/include/platform/dummy/io.h"

uint8_t spi_control_register = 0;
uint8_t * const spi_control_register_ptr = &spi_control_register;


uint8_t usart_baud_rate_low = 0;
uint8_t usart_baud_rate_high = 0;
uint8_t usart_control_and_status_a = 0;
uint8_t usart_control_and_status_b = 0;
uint8_t usart_control_and_status_c = 0;
uint8_t usart_data = 0;

void resetDummyIO(void){
    usart_baud_rate_high = 0;
    usart_baud_rate_low = 0;
    usart_control_and_status_a = 0;
    usart_control_and_status_b = 0;
    usart_control_and_status_c = 0;
    usart_data = 0;
}
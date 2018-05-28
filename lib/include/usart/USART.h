#ifndef UNITYOUTPUTIMPLEMENTATIONS_UNITYOUTPUT_H
#define UNITYOUTPUTIMPLEMENTATIONS_UNITYOUTPUT_H

#include <stdint.h>

void USART_init(uint32_t baud_rate);
void USART_write(uint8_t byte);
void USART_writeN(uint8_t *byte, uint16_t length);

#endif //UNITYOUTPUTIMPLEMENTATIONS_UNITYOUTPUT_H

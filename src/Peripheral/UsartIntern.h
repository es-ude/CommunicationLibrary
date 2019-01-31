#ifndef EDGEDEVICE_USARTINTERN_H
#define EDGEDEVICE_USARTINTERN_H

/* This is an internal header meant to be included only by the
 * implementation in Usart.c and possibly the unit tests.
 */

#include "Peripheral/PeripheralInterface.h"
#include "Peripheral/Usart.h"

/**
 * Below are pin definitions
 * common to most avr platforms
 */
static const uint8_t usart_reception_complete_bit  = 7;
static const uint8_t usart_transmit_complete_bit   = 6;
static const uint8_t usart_data_register_empty_bit = 5;
static const uint8_t usart_receiver_enable_bit     = 4;
static const uint8_t usart_transmitter_enable_bit  = 3;
static const uint8_t usart_character_size_2_bit    = 2;
static const uint8_t usart_parity_mode_1_bit       = 5;
static const uint8_t usart_parity_mode_0_bit       = 4;
static const uint8_t usart_stop_bit_select_bit     = 3;
static const uint8_t usart_character_size_1_bit    = 2;
static const uint8_t usart_character_size_0_bit    = 1;

static const uint32_t baud_rate_2400   = 2400;
static const uint32_t baud_rate_9600   = 9600;
static const uint32_t baud_rate_115200 = 115200;

#endif

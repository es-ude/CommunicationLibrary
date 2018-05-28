#ifndef UNITYOUTPUTIMPLEMENTATIONS_ATMEGA328P_IO_H
#define UNITYOUTPUTIMPLEMENTATIONS_ATMEGA328P_IO_H

#include <stdint.h>
#include <avr/io.h>

typedef enum sck_rate {f_osc_4, f_osc_16, f_osc_64, f_osc_128} sck_rate;


//SPCR
static const uint8_t spi_clock_rate_select_0 = 0;
static const uint8_t spi_clock_rate_select_1 = 1;
static const uint8_t spi_clock_phase = 2;
static const uint8_t spi_clock_polarity = 3;
static const uint8_t spi_master_slave_select = 4;
static const uint8_t spi_data_order = 5;
static const uint8_t spi_enable = 6;
static const uint8_t spi_interrupt_enable = 7 ;

//SPSR
static const uint8_t spi_interrupt_flag = 7;

static const uint8_t spi_sck_pin = 5; //DDRB PIN 13
static const uint8_t spi_miso_pin = 4; //DDRB PIN 12
static const uint8_t spi_mosi_pin = 3; //DDRB PIN 11
static const uint8_t spi_ss_pin = 2; //DDRB PIN 10
extern uint8_t * const spi_control_register_ptr;


static uint64_t cpu_frequency = F_CPU;
static const uint8_t usart_transmitter_enable_bit = TXEN0;
static const uint8_t usart_character_size_offset = UCSZ00;
static const uint8_t usart_data_register_empty = UDRE0;
static volatile uint8_t *const usart_baud_rate_register_low = &UBRR0L;
static volatile uint8_t *const usart_baud_rate_register_high = &UBRR0H;
static volatile uint8_t *const usart_control_and_status_register_a = &UCSR0A;
static volatile uint8_t *const usart_control_and_status_register_b = &UCSR0B;
static volatile uint8_t *const usart_control_and_status_register_c = &UCSR0C;
static volatile uint8_t *const usart_data_register = &UDR0;


#endif //UNITYOUTPUTIMPLEMENTATIONS_IO_H

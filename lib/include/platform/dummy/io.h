#ifndef IO_H
#define IO_H
#include <stdint.h>

enum {f_osc_4, f_osc_16, f_osc_64, f_osc_128};

static const uint8_t spi_clock_rate_select_0 = 0;
static const uint8_t spi_clock_rate_select_1 = 1;
static const uint8_t spi_clock_phase = 2;
static const uint8_t spi_clock_polarity = 3;
static const uint8_t spi_master_slave_select = 4;
static const uint8_t spi_data_order = 5;
static const uint8_t spi_enable = 6;
static const uint8_t spi_interrupt_enable = 7 ;

static const uint8_t spi_sck_pin = 5; //DDRB PIN 13
static const uint8_t spi_miso_pin = 4; //DDRB PIN 12
static const uint8_t spi_mosi_pin = 3; //DDRB PIN 11
static const uint8_t spi_ss_pin = 2; //DDRB PIN 10
extern uint8_t * const spi_control_register_ptr;



#endif 
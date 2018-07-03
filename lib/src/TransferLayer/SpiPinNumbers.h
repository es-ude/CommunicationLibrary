#ifndef COMMUNICATIONMODULE_SPIPINNUMBERS_H_H
#define COMMUNICATIONMODULE_SPIPINNUMBERS_H_H

#include <stdint.h>

/**
 * In our SPI implementation we assume that the Hardware Implementation
 * of the SPI is always the same. I.e. there are always three registers
 * that control hardware interface and these three registers always behave
 * the same for all our avr platforms. Based on the datasheets for the
 * atmega328p, attiny64 and atmega32u4 we can define  the following
 * constant values for bit numbers. Please note that these refer only to bit numbers
 * used with the spi control and status registers. You may not assume, that
 * other pin/bit numbers do not change with the platform, e.g. MISO/MOSI/Clock Signal line.
 */

static const uint8_t spi_interrupt_enable_bit  = 7;
static const uint8_t spi_enable_bit            = 6;
static const uint8_t data_order_bit            = 5;
static const uint8_t master_slave_select_bit   = 4;
static const uint8_t clock_polarity_bit        = 3;
static const uint8_t clock_phase_bit           = 2;
static const uint8_t clock_rate_select_bit0    = 1;
static const uint8_t clock_rate_select_bit1    = 0;
static const uint8_t spi_interrupt_flag_bit    = 7;
static const uint8_t write_collision_flag_bit  = 6;
static const uint8_t double_spi_speed_bit      = 0;

#endif //COMMUNICATIONMODULE_SPIPINNUMBERS_H_H

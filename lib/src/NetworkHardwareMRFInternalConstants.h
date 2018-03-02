#ifndef COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H
#define COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H

#include "lib/include/RuntimeLibraryInterface.h"

static const uint8_t mrf_register_power_amplifier_control2 = 0x18;
static const uint8_t mrf_register_software_reset = 0x2A;
static const uint8_t mrf_register_tx_stabilization = 0x2E;
static const uint8_t mrf_register_interrupt_status = 0x31;
static const uint8_t mrf_register_interrupt_control = 0x32;
static const uint8_t mrf_register_rf_mode_control = 0x36;
static const uint8_t mrf_register_base_band2 = 0x3A;
static const uint8_t mrf_register_base_band6 = 0x3E;
static const uint8_t mrf_register_energy_detection_threshold_for_clear_channel_assessment = 0x3F;

static const uint16_t mrf_register_rf_control0 = 0x200;
static const uint16_t mrf_register_rf_control1 = 0x201;
static const uint16_t mrf_register_rf_control2 = 0x202;
static const uint16_t mrf_register_rf_control3 = 0x203;
static const uint16_t mrf_register_rf_control4 = 0x204;
static const uint16_t mrf_register_rf_control5 = 0x205;
static const uint16_t mrf_register_rf_control6 = 0x206;
static const uint16_t mrf_register_rf_control7 = 0x207;
static const uint16_t mrf_register_rf_control8 = 0x208;
static const uint16_t mrf_register_sleep_clock_control0 = 0x211;
static const uint16_t mrf_register_sleep_clock_control1 = 0x220;

static const uint8_t mrf_bit_fifo_enable = 0x07;
static const uint8_t mrf_tx_normal_fifo_length = 0x80;
static const uint16_t mrf_tx_fifo_start = 0x0;
static const uint16_t mrf_rx_fifo_start = 0x300;
static const uint8_t mrf_rx_fifo_length = 0x90;

#endif //COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H

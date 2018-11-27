
#ifndef COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H
#define COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H

#include "CommunicationModule/Peripheral.h"

static const uint8_t mrf_register_receive_mac_control = 0x00;
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

static const uint8_t mrf_register_pan_id_low_byte = 0x01;
static const uint8_t mrf_register_pan_id_high_byte = 0x02;
static const uint8_t mrf_register_short_address_low_byte = 0x03;
static const uint8_t mrf_register_short_address_high_byte = 0x04;
static const uint8_t mrf_register_extended_address0 = 0x05;
static const uint8_t mrf_register_tx_normal_fifo_control = 0x1B;

static const uint8_t mrf_fifo_enable = 0x08;
static const uint8_t mrf_tx_normal_fifo_length = 0x80;
static const uint16_t mrf_tx_fifo_start = 0x0;
static const uint16_t mrf_rx_fifo_start = 0x300;
static const uint8_t mrf_rx_fifo_length = 0x90;

static const uint8_t mrf_value_full_software_reset = 0x07;
static const uint8_t mrf_value_recommended_transmitter_on_time_before_beginning_a_packet = 0x18;
static const uint8_t mrf_value_recommended_interframe_spacing = 0x95;
static const uint8_t mrf_value_recommended_rf_optimize_control0 = 0x03;
static const uint8_t mrf_value_recommended_rf_optimize_control1 = 0x01;
static const uint8_t mrf_value_recommended_rf_control8 = 0x10;
static const uint8_t mrf_value_phase_locked_loop_enabled = 0x80;
static const uint8_t mrf_value_transmitter_power_minus30dB = 0x03 << 6;
static const uint8_t mrf_value_enable_tx_filter = 0x01 << 7;
static const uint8_t mrf_value_20MHz_clock_recovery_less_than_1ms = 0x01 << 4;
static const uint8_t mrf_value_use_internal_100kHz_oscillator = 0x80;
static const uint8_t mrf_value_disable_deprecated_clkout_sleep_clock_feature = 1 << 5;
static const uint8_t mrf_value_minimum_sleep_clock_divisor_for_internal_oscillator = 1;
static const uint8_t mrf_value_clear_channel_assessment_energy_detection_only = 0x80;
static const uint8_t mrf_value_recommended_energy_detection_threshold = 0x60;
static const uint8_t mrf_value_append_rssi_value_to_rxfifo = 0x40;
static const uint8_t mrf_value_rf_state_machine_reset_state = 0x04;
static const uint8_t mrf_value_rf_state_machine_operating_state = 0x00;
static const uint8_t mrf_value_delay_interval_after_state_machine_reset = 200;
static const uint8_t mrf_value_rx_interrupt_enabled = (uint8_t) ~(1 << 3);

#endif //COMMUNICATIONMODULE_NETWORKHARDWAREMRFIMPL_H

#pragma once

#include "spi_interface.h"

typedef struct {
    spi_interface *spi_handler;
} device_commands;

void device_commands_init(device_commands *self, spi_interface *spi_handler);

void spi_interface_pulse_ce(spi_interface *self);

void spi_interface_enable_ce(spi_interface *self);

void spi_interface_disable_ce(spi_interface *self);

// Commands

void device_commands_flush_tx(device_commands *self);

void device_commands_flush_rx(device_commands *self);

void device_commands_w_tx_payload(device_commands *self, uint8_t *payload, uint payload_length);

void device_commands_w_tx_payload_no_ack(device_commands *self, uint8_t *payload, uint payload_length);

void device_commands_r_rx_payload(device_commands *self, uint8_t *output, uint output_length);

// Registers

void device_commands_get_pwr_up(device_commands *self, bool *value);

void device_commands_set_pwr_up(device_commands *self, bool value);

void device_commands_get_prim_rx(device_commands *self, bool *value);

void device_commands_set_prim_rx(device_commands *self, bool value);

void device_commands_get_ard(device_commands *self, uint8_t *value);

void device_commands_set_ard(device_commands *self, uint8_t value);

void device_commands_get_arc(device_commands *self, uint8_t *value);

void device_commands_set_arc(device_commands *self, uint8_t value);

void device_commands_get_rf_ch(device_commands *self, uint8_t *value);

void device_commands_set_rf_ch(device_commands *self, uint8_t value);

void device_commands_get_rf_dr_low(device_commands *self, bool *value);

void device_commands_set_rf_dr_low(device_commands *self, bool value);

void device_commands_get_rf_dr_high(device_commands *self, bool *value);

void device_commands_set_rf_dr_high(device_commands *self, bool value);

void device_commands_get_rf_pwr(device_commands *self, uint8_t *value);

void device_commands_set_rf_pwr(device_commands *self, uint8_t value);

void device_commands_get_rx_dr(device_commands *self, bool *value);

void device_commands_clear_rx_dr(device_commands *self);

void device_commands_get_tx_ds(device_commands *self, bool *value);

void device_commands_clear_tx_ds(device_commands *self);

void device_commands_get_max_rt(device_commands *self, bool *value);

void device_commands_clear_max_rt(device_commands *self);

void device_commands_get_rx_addr(device_commands *self, uint pipe, uint8_t *value);

void device_commands_set_rx_addr(device_commands *self, uint pipe, uint8_t *value);

void device_commands_get_tx_addr(device_commands *self, uint8_t *value);

void device_commands_set_tx_addr(device_commands *self, uint8_t *value);

void device_commands_get_rx_pw(device_commands *self, uint pipe, uint8_t *value);

void device_commands_set_rx_pw(device_commands *self, uint pipe, uint8_t value);

void device_commands_get_rx_empty(device_commands *self, bool *value);

void device_commands_get_en_dyn_ack(device_commands *self, bool *value);

void device_commands_set_en_dyn_ack(device_commands *self, bool value);

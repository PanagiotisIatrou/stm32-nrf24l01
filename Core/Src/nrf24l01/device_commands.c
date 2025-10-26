#include "nrf24l01/device_commands.h"

#include <stdio.h>

void device_commands_init(device_commands *self, spi_interface *spi_handler) { self->spi_handler = spi_handler; }

static void
device_commands_read_register(device_commands *self, uint8_t address, uint8_t *output, uint8_t output_length) {
    uint8_t command = COMMAND_CODE_R_REGISTER | address;
    spi_interface_send_command(self->spi_handler, command, NULL, 0, output, output_length);
}

static void device_commands_write_register(device_commands *self, uint8_t address, uint8_t *data, uint8_t data_length) {
    uint8_t command = COMMAND_CODE_W_REGISTER | address;
    spi_interface_send_command(self->spi_handler, command, data, data_length, NULL, 0);
}

void device_commands_pulse_ce(device_commands *self) { spi_interface_pulse_ce(self->spi_handler); }

void device_commands_enable_ce(device_commands *self) { spi_interface_enable_ce(self->spi_handler); }

void device_commands_disable_ce(device_commands *self) { spi_interface_disable_ce(self->spi_handler); }

// Commands

void device_commands_flush_tx(device_commands *self) {
    spi_interface_send_command(self->spi_handler, COMMAND_CODE_FLUSH_TX, NULL, 0, NULL, 0);
}

void device_commands_flush_rx(device_commands *self) {
    spi_interface_send_command(self->spi_handler, COMMAND_CODE_FLUSH_RX, NULL, 0, NULL, 0);
}

void device_commands_w_tx_payload(device_commands *self, uint8_t *payload, uint32_t payload_length) {
    spi_interface_send_command(self->spi_handler, COMMAND_CODE_W_TX_PAYLOAD, payload, payload_length, NULL, 0);
}

void device_commands_w_tx_payload_no_ack(device_commands *self, uint8_t *payload, uint32_t payload_length) {
    spi_interface_send_command(self->spi_handler, COMMAND_CODE_W_TX_PAYLOAD_NO_ACK, payload, payload_length, NULL, 0);
}

void device_commands_r_rx_payload(device_commands *self, uint8_t *output, uint32_t output_length) {
    spi_interface_send_command(self->spi_handler, COMMAND_CODE_R_RX_PAYLOAD, NULL, 0, output, output_length);
}

void device_commands_r_rx_pl_wid(device_commands *self, uint8_t *output) {
    spi_interface_send_command(self->spi_handler, COMMAND_CODE_R_RX_PL_WID, NULL, 0, output, 1);
}

// Registers

void device_commands_get_crco(device_commands *self, bool *value) {
    uint8_t config_register;
    device_commands_read_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
    *value = (config_register >> 2) & 0x01;
}

void device_commands_set_crco(device_commands *self, bool value) {
    uint8_t config_register;
    device_commands_read_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
    config_register = (config_register & 0xFB) | (value << 2);
    device_commands_write_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
}

void device_commands_get_pwr_up(device_commands *self, bool *value) {
    uint8_t config_register;
    device_commands_read_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
    *value = (config_register >> 1) & 0x01;
}

void device_commands_set_pwr_up(device_commands *self, bool value) {
    uint8_t config_register;
    device_commands_read_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
    config_register = (config_register & 0xFD) | (value << 1);
    device_commands_write_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
}

void device_commands_get_prim_rx(device_commands *self, bool *value) {
    uint8_t config_register;
    device_commands_read_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
    *value = config_register & 0x01;
}

void device_commands_set_prim_rx(device_commands *self, bool value) {
    uint8_t config_register;
    device_commands_read_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
    config_register = (config_register & 0xFE) | value;
    device_commands_write_register(self, REGISTER_ADDRESS_CONFIG, &config_register, 1);
}

void device_commands_get_erx(device_commands *self, uint32_t pipe, bool *value) {
    uint8_t en_rxaddr_register;
    device_commands_read_register(self, REGISTER_ADDRESS_EN_RXADDR, &en_rxaddr_register, 1);
    *value = (en_rxaddr_register >> pipe) & 0x01;
}

void device_commands_set_erx(device_commands *self, uint32_t pipe, bool value) {
    uint8_t en_rxaddr_register;
    device_commands_read_register(self, REGISTER_ADDRESS_EN_RXADDR, &en_rxaddr_register, 1);
    en_rxaddr_register = (en_rxaddr_register & (~(1 << pipe))) | (value << pipe);
    device_commands_write_register(self, REGISTER_ADDRESS_EN_RXADDR, &en_rxaddr_register, 1);
}

void device_commands_get_ard(device_commands *self, uint8_t *value) {
    uint8_t setup_retr_register;
    device_commands_read_register(self, REGISTER_ADDRESS_SETUP_RETR, &setup_retr_register, 1);
    *value = (setup_retr_register >> 4) & 0x0F;
}

void device_commands_set_ard(device_commands *self, uint8_t value) {
    uint8_t setup_retr_register;
    device_commands_read_register(self, REGISTER_ADDRESS_SETUP_RETR, &setup_retr_register, 1);
    setup_retr_register = (setup_retr_register & 0x0F) | (value << 4);
    device_commands_write_register(self, REGISTER_ADDRESS_SETUP_RETR, &setup_retr_register, 1);
}

void device_commands_get_arc(device_commands *self, uint8_t *value) {
    uint8_t setup_retr_register;
    device_commands_read_register(self, REGISTER_ADDRESS_SETUP_RETR, &setup_retr_register, 1);
    *value = setup_retr_register & 0x0F;
}

void device_commands_set_arc(device_commands *self, uint8_t value) {
    uint8_t setup_retr_register;
    device_commands_read_register(self, REGISTER_ADDRESS_SETUP_RETR, &setup_retr_register, 1);
    setup_retr_register = (setup_retr_register & 0xF0) | value;
    device_commands_write_register(self, REGISTER_ADDRESS_SETUP_RETR, &setup_retr_register, 1);
}

void device_commands_get_rf_ch(device_commands *self, uint8_t *value) {
    uint8_t rf_ch_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_CH, &rf_ch_register, 1);
    *value = rf_ch_register & 0x7F;
}

void device_commands_set_rf_ch(device_commands *self, uint8_t value) {
    uint8_t rf_ch_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_CH, &rf_ch_register, 1);
    rf_ch_register = (rf_ch_register & 0x80) | value;
    device_commands_write_register(self, REGISTER_ADDRESS_RF_CH, &rf_ch_register, 1);
}

void device_commands_get_rf_dr_low(device_commands *self, bool *value) {
    uint8_t rf_setup_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
    *value = (rf_setup_register >> 5) & 0x01;
}

void device_commands_set_rf_dr_low(device_commands *self, bool value) {
    uint8_t rf_setup_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
    rf_setup_register = (rf_setup_register & 0xDF) | (value << 5);
    device_commands_write_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
}

void device_commands_get_rf_dr_high(device_commands *self, bool *value) {
    uint8_t rf_setup_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
    *value = (rf_setup_register >> 3) & 0x01;
}

void device_commands_set_rf_dr_high(device_commands *self, bool value) {
    uint8_t rf_setup_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
    rf_setup_register = (rf_setup_register & 0xF7) | (value << 3);
    device_commands_write_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
}

void device_commands_get_rf_pwr(device_commands *self, uint8_t *value) {
    uint8_t rf_setup_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
    *value = (rf_setup_register >> 1) & 0x03;
}

void device_commands_set_rf_pwr(device_commands *self, uint8_t value) {
    uint8_t rf_setup_register;
    device_commands_read_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
    rf_setup_register = (rf_setup_register & 0xF9) | (value << 1);
    device_commands_write_register(self, REGISTER_ADDRESS_RF_SETUP, &rf_setup_register, 1);
}

void device_commands_get_rx_dr(device_commands *self, bool *value) {
    uint8_t status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
    *value = (status_register >> 6) & 0x01;
}

void device_commands_clear_rx_dr(device_commands *self) {
    uint8_t status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
    status_register |= 0x40;
    device_commands_write_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
}

void device_commands_get_tx_ds(device_commands *self, bool *value) {
    uint8_t status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
    *value = (status_register >> 5) & 0x01;
}

void device_commands_clear_tx_ds(device_commands *self) {
    uint8_t status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
    status_register |= 0x20;
    device_commands_write_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
}

void device_commands_get_max_rt(device_commands *self, bool *value) {
    uint8_t status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
    *value = (status_register >> 4) & 0x01;
}

void device_commands_clear_max_rt(device_commands *self) {
    uint8_t status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
    status_register |= 0x10;
    device_commands_write_register(self, REGISTER_ADDRESS_STATUS, &status_register, 1);
}

void device_commands_get_rx_addr_full(device_commands *self, uint32_t pipe, uint8_t *value) {
    uint8_t address = REGISTER_ADDRESS_RX_ADDR_P0 + pipe;
    device_commands_read_register(self, address, value, 5);
}

void device_commands_set_rx_addr_full(device_commands *self, uint32_t pipe, uint8_t *value) {
    uint8_t address = REGISTER_ADDRESS_RX_ADDR_P0 + pipe;
    device_commands_write_register(self, address, value, 5);
}

void device_commands_get_rx_addr_lsb(device_commands *self, uint32_t pipe, uint8_t *value) {
    uint8_t address = REGISTER_ADDRESS_RX_ADDR_P0 + pipe;
    uint8_t rx_addr_register_lsb;
    device_commands_read_register(self, address, &rx_addr_register_lsb, 1);
    *value = rx_addr_register_lsb;
}

void device_commands_set_rx_addr_lsb(device_commands *self, uint32_t pipe, uint8_t value) {
    uint8_t address = REGISTER_ADDRESS_RX_ADDR_P0 + pipe;
    device_commands_write_register(self, address, &value, 1);
}

void device_commands_get_tx_addr_full(device_commands *self, uint8_t *value) {
    device_commands_read_register(self, REGISTER_ADDRESS_TX_ADDR, value, 5);
}

void device_commands_set_tx_addr_full(device_commands *self, uint8_t *value) {
    device_commands_write_register(self, REGISTER_ADDRESS_TX_ADDR, value, 5);
}

void device_commands_get_tx_addr_lsb(device_commands *self, uint8_t *value) {
    uint8_t tx_addr_register_lsb;
    device_commands_read_register(self, REGISTER_ADDRESS_TX_ADDR, &tx_addr_register_lsb, 1);
    *value = tx_addr_register_lsb;
}

void device_commands_set_tx_addr_lsb(device_commands *self, uint8_t value) {
    device_commands_write_register(self, REGISTER_ADDRESS_TX_ADDR, &value, 1);
}

void device_commands_get_rx_pw(device_commands *self, uint32_t pipe, uint8_t *value) {
    uint8_t address = REGISTER_ADDRESS_RX_PW_P0 + pipe;
    uint8_t rx_pw_register;
    device_commands_read_register(self, address, &rx_pw_register, 1);
    *value = rx_pw_register & 0x3F;
}

void device_commands_set_rx_pw(device_commands *self, uint32_t pipe, uint8_t value) {
    uint8_t address = REGISTER_ADDRESS_RX_PW_P0 + pipe;
    uint8_t rx_pw_register;
    device_commands_read_register(self, address, &rx_pw_register, 1);
    rx_pw_register = (rx_pw_register & 0xC0) | value;
    device_commands_write_register(self, address, &rx_pw_register, 1);
}

void device_commands_get_rx_empty(device_commands *self, bool *value) {
    uint8_t fifo_status_register;
    device_commands_read_register(self, REGISTER_ADDRESS_FIFO_STATUS, &fifo_status_register, 1);
    *value = fifo_status_register & 0x01;
}

void device_commands_get_dpl(device_commands *self, uint32_t pipe, bool *value) {
    uint8_t dynpd_register;
    device_commands_read_register(self, REGISTER_ADDRESS_DYNPD, &dynpd_register, 1);
    *value = (dynpd_register >> pipe) & 0x01;
}

void device_commands_set_dpl(device_commands *self, uint32_t pipe, bool value) {
    uint8_t dynpd_register;
    device_commands_read_register(self, REGISTER_ADDRESS_DYNPD, &dynpd_register, 1);
    dynpd_register = (dynpd_register & (~(1 << pipe))) | (value << pipe);
    device_commands_write_register(self, REGISTER_ADDRESS_DYNPD, &dynpd_register, 1);
}

void device_commands_get_en_dpl(device_commands *self, bool *value) {
    uint8_t feature_register;
    device_commands_read_register(self, REGISTER_ADDRESS_FEATURE, &feature_register, 1);
    *value = (feature_register >> 2) & 0x01;
}

void device_commands_set_en_dpl(device_commands *self, bool value) {
    uint8_t feature_register;
    device_commands_read_register(self, REGISTER_ADDRESS_FEATURE, &feature_register, 1);
    feature_register = (feature_register & 0xFB) | (value << 2);
    device_commands_write_register(self, REGISTER_ADDRESS_FEATURE, &feature_register, 1);
}

void device_commands_get_en_dyn_ack(device_commands *self, bool *value) {
    uint8_t feature_register;
    device_commands_read_register(self, REGISTER_ADDRESS_FEATURE, &feature_register, 1);
    *value = feature_register & 0x01;
}

void device_commands_set_en_dyn_ack(device_commands *self, bool value) {
    uint8_t feature_register;
    device_commands_read_register(self, REGISTER_ADDRESS_FEATURE, &feature_register, 1);
    feature_register = (feature_register & 0xFE) | value;
    device_commands_write_register(self, REGISTER_ADDRESS_FEATURE, &feature_register, 1);
}

#pragma once

#include "spi_interface.h"

/**
 * Command codes for the nrf24l01 device
 */
typedef enum {
    COMMAND_CODE_R_REGISTER = 0x00,
    COMMAND_CODE_W_REGISTER = 0x20,
    COMMAND_CODE_R_RX_PAYLOAD = 0x61,
    COMMAND_CODE_W_TX_PAYLOAD = 0xA0,
    COMMAND_CODE_FLUSH_TX = 0xE1,
    COMMAND_CODE_FLUSH_RX = 0xE2,
    COMMAND_CODE_W_TX_PAYLOAD_NO_ACK = 0xB0,
    COMMAND_CODE_R_RX_PL_WID = 0x60,
} CommandCode;

/**
 * Register addresses for the nrf24l01 device
 */
typedef enum {
    REGISTER_ADDRESS_CONFIG = 0x00,
    REGISTER_ADDRESS_EN_RXADDR = 0x02,
    REGISTER_ADDRESS_SETUP_RETR = 0x04,
    REGISTER_ADDRESS_RF_CH = 0x05,
    REGISTER_ADDRESS_RF_SETUP = 0x06,
    REGISTER_ADDRESS_STATUS = 0x07,
    REGISTER_ADDRESS_RX_ADDR_P0 = 0x0A,
    REGISTER_ADDRESS_TX_ADDR = 0x10,
    REGISTER_ADDRESS_RX_PW_P0 = 0x11,
    REGISTER_ADDRESS_FIFO_STATUS = 0x17,
    REGISTER_ADDRESS_DYNPD = 0x1C,
    REGISTER_ADDRESS_FEATURE = 0x1D,
} RegisterAddress;

/**
 * Struct used to interact with the nrf24l01 device through commands
 */
typedef struct {
    spi_interface *spi_handler;
} device_commands;

/**
 * Initializes the device_commands struct with the given spi_interface.
 * @param self Pointer to the device_commands struct to initialize
 * @param spi_handler Pointer to the spi_interface to use for communication. Must be
 *                    initialized beforehand.
 */
void device_commands_init(device_commands *self, spi_interface *spi_handler);

/**
 * Pulses the CE pin of the SPI interface to trigger certain actions on the nrf24l01 device.
 * @param self Pointer to the spi_interface struct to use.
 */
void spi_interface_pulse_ce(spi_interface *self);

/**
 * Enables the CE pin of the SPI interface to put the nrf24l01 device in active mode.
 * @param self Pointer to the spi_interface struct to use.
 */
void spi_interface_enable_ce(spi_interface *self);

/**
 * Disables the CE pin of the SPI interface to put the nrf24l01 device in standby mode.
 * @param self Pointer to the spi_interface struct to use.
 */
void spi_interface_disable_ce(spi_interface *self);

// Commands

/**
 * Removes all data from the TX FIFO.
 * @param self Pointer to the device_commands struct to use.
 */
void device_commands_flush_tx(device_commands *self);

/**
 * Removes all data from the RX FIFO.
 * @param self Pointer to the device_commands struct to use.
 */
void device_commands_flush_rx(device_commands *self);

/**
 * Queues the given payload for transmission, requesting an acknowledgment.
 * @param self Pointer to the device_commands struct to use.
 * @param payload Pointer to the array of bytes to send.
 * @param payload_length Length of the data to send in bytes.
 */
void device_commands_w_tx_payload(device_commands *self, uint8_t *payload, uint payload_length);

/**
 * Queues the given payload for transmission without requesting an acknowledgment.
 * @param self Pointer to the device_commands struct to use.
 * @param payload Pointer to the array of bytes to send.
 * @param payload_length Length of the data to send in bytes.
 */
void device_commands_w_tx_payload_no_ack(device_commands *self, uint8_t *payload, uint payload_length);

/**
 * Reads the received payload from the head of the RX FIFO into the provided output buffer.
 * This operation also removes the payload from the RX FIFO.
 * @param self Pointer to the device_commands struct to use.
 * @param output Pointer to the array of bytes where the received data will be stored.
 * @param output_length Length of the data to read in bytes.
 */
void device_commands_r_rx_payload(device_commands *self, uint8_t *output, uint output_length);

/**
 * Reads the width of the payload at the head of the RX FIFO.
 * @param self Pointer to the device_commands struct to use.
 * @param output Pointer to a variable where the payload width will be stored.
 */
void device_commands_r_rx_pl_wid(device_commands *self, uint8_t *output);

// Registers

/**
 * Gets the value of CRCO from the CONFIG register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the CRCO value will be stored.
 */
void device_commands_get_crco(device_commands *self, bool *value);

/**
 * Sets the value of CRCO in the CONFIG register.
 * @param self Pointer to the device_commands struct to use.
 * @param value true for 2 bytes, false for 1 byte.
 */
void device_commands_set_crco(device_commands *self, bool value);

/**
 * Gets the value of PWR_UP from the CONFIG register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the PWR_UP value will be stored.
 */
void device_commands_get_pwr_up(device_commands *self, bool *value);

/**
 * Sets the value of PWR_UP in the CONFIG register.
 * @param self Pointer to the device_commands struct to use.
 * @param value true to power up, false to power down.
 */
void device_commands_set_pwr_up(device_commands *self, bool value);

/**
 * Gets the value of PRIM_RX from the CONFIG register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the PRIM_RX value will be stored.
 */
void device_commands_get_prim_rx(device_commands *self, bool *value);

/**
 * Sets the value of PRIM_RX in the CONFIG register.
 * @param self Pointer to the device_commands struct to use.
 * @param value true for RX mode, false for TX mode.
 */
void device_commands_set_prim_rx(device_commands *self, bool value);

/**
 * Gets the value of ERX_Px from the EN_RXADDR register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value Pointer to a variable where the ERX_Px value will be stored.
 */
void device_commands_get_erx(device_commands *self, uint pipe, bool *value);

/**
 * Sets the value of ERX_Px in the EN_RXADDR register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value true to enable, false to disable.
 */
void device_commands_set_erx(device_commands *self, uint pipe, bool value);

/**
 * Gets the value of ARD from the SETUP_RETR register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the ARD value will be stored.
 */
void device_commands_get_ard(device_commands *self, uint8_t *value);

/**
 * Sets the value of ARD in the SETUP_RETR register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The ARD value (0-15).
 */
void device_commands_set_ard(device_commands *self, uint8_t value);

/**
 * Gets the value of ARC from the SETUP_RETR register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the ARC value will be stored.
 */
void device_commands_get_arc(device_commands *self, uint8_t *value);

/**
 * Sets the value of ARC in the SETUP_RETR register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The ARC value (0-15).
 */
void device_commands_set_arc(device_commands *self, uint8_t value);

/**
 * Gets the value of RF_CH from the RF_CH register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the RF_CH value will be stored (0-125).
 */
void device_commands_get_rf_ch(device_commands *self, uint8_t *value);

/**
 * Sets the value of RF_CH in the RF_CH register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The RF_CH value (0-125).
 */
void device_commands_set_rf_ch(device_commands *self, uint8_t value);

/**
 * Gets the value of RF_DR_LOW from the RF_SETUP register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the RF_DR_LOW value will be stored.
 */
void device_commands_get_rf_dr_low(device_commands *self, bool *value);

/**
 * Sets the value of RF_DR_LOW in the RF_SETUP register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The RF_DR_LOW value
 */
void device_commands_set_rf_dr_low(device_commands *self, bool value);

/**
 * Gets the value of RF_DR_HIGH from the RF_SETUP register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the RF_DR_HIGH value will be stored.
 */
void device_commands_get_rf_dr_high(device_commands *self, bool *value);

/**
 * Sets the value of RF_DR_HIGH in the RF_SETUP register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The RF_DR_HIGH value
 */
void device_commands_set_rf_dr_high(device_commands *self, bool value);

/**
 * Gets the value of RF_PWR from the RF_SETUP register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the RF_PWR value will be stored (0-3).
 */
void device_commands_get_rf_pwr(device_commands *self, uint8_t *value);

/**
 * Sets the value of RF_PWR in the RF_SETUP register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The RF_PWR value (0-3).
 */
void device_commands_set_rf_pwr(device_commands *self, uint8_t value);

/**
 * Gets the value of RX_DR from the STATUS register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the RX_DR value will be stored.
 */
void device_commands_get_rx_dr(device_commands *self, bool *value);

/**
 * Clears the RX_DR bit in the STATUS register by writing 1 to it.
 * @param self Pointer to the device_commands struct to use.
 */
void device_commands_clear_rx_dr(device_commands *self);

/**
 * Gets the value of TX_DS from the STATUS register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the TX_DS value will be stored.
 */
void device_commands_get_tx_ds(device_commands *self, bool *value);

/**
 * Clears the TX_DS bit in the STATUS register by writing 1 to it.
 * @param self Pointer to the device_commands struct to use.
 */
void device_commands_clear_tx_ds(device_commands *self);

/**
 * Gets the value of MAX_RT from the STATUS register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the MAX_RT value will be stored.
 */
void device_commands_get_max_rt(device_commands *self, bool *value);

/**
 * Clears the MAX_RT bit in the STATUS register by writing 1 to it.
 * @param self Pointer to the device_commands struct to use.
 */
void device_commands_clear_max_rt(device_commands *self);

/**
 * Gets the full 5-byte RX_ADDR_Px address for the specified data pipe.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value Pointer to an array where the 5-byte address will be stored.
 */
void device_commands_get_rx_addr_full(device_commands *self, uint pipe, uint8_t *value);

/**
 * Sets the full 5-byte RX_ADDR_Px address for the specified data pipe.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value Pointer to an array containing the 5-byte address to set.
 */
void device_commands_set_rx_addr_full(device_commands *self, uint pipe, uint8_t *value);

/**
 * Gets the LSB of the RX_ADDR_Px register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value Pointer to a variable where the LSB of the address will be stored.
 */
void device_commands_get_rx_addr_lsb(device_commands *self, uint pipe, uint8_t *value);

/**
 * Sets the LSB of the RX_ADDR_Px register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value The LSB of the address to set.
 */
void device_commands_set_rx_addr_lsb(device_commands *self, uint pipe, uint8_t value);

/**
 * Gets the full 5-byte TX_ADDR address.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to an array where the 5-byte address will be stored.
 */
void device_commands_get_tx_addr_full(device_commands *self, uint8_t *value);

/**
 * Sets the full 5-byte TX_ADDR address.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to an array containing the 5-byte address to set.
 */
void device_commands_set_tx_addr_full(device_commands *self, uint8_t *value);

/**
 * Gets the LSB of the TX_ADDR register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the LSB of the address will be stored.
 */
void device_commands_get_tx_addr_lsb(device_commands *self, uint8_t *value);

/**
 * Sets the LSB of the TX_ADDR register.
 * @param self Pointer to the device_commands struct to use.
 * @param value The LSB of the address to set.
 */
void device_commands_set_tx_addr_lsb(device_commands *self, uint8_t value);

/**
 * Gets the value of RX_PW_Px from the RX_PW_Px register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value Pointer to a variable where the payload width will be stored (0-32).
 */
void device_commands_get_rx_pw(device_commands *self, uint pipe, uint8_t *value);

/**
 * Sets the value of RX_PW_Px in the RX_PW_Px register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value The payload width to set (0-32).
 */
void device_commands_set_rx_pw(device_commands *self, uint pipe, uint8_t value);

/**
 * Gets the value of RX_EMPTY from the FIFO_STATUS register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the RX_EMPTY value will be stored.
 */
void device_commands_get_rx_empty(device_commands *self, bool *value);

/**
 * Gets the value of DPL_Px from the DYNPD register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value Pointer to a variable where the DPL_Px value will be stored.
 */
void device_commands_get_dpl(device_commands *self, uint pipe, bool *value);

/**
 * Sets the value of DPL_Px in the DYNPD register for the specified pipe x.
 * @param self Pointer to the device_commands struct to use.
 * @param pipe The data pipe number (0-5).
 * @param value true to enable, false to disable.
 */
void device_commands_set_dpl(device_commands *self, uint pipe, bool value);

/**
 * Gets the value of EN_DPL from the FEATURE register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the EN_DPL value will be stored.
 */
void device_commands_get_en_dpl(device_commands *self, bool *value);

/**
 * Sets the value of EN_DPL in the FEATURE register.
 * @param self Pointer to the device_commands struct to use.
 * @param value true to enable, false to disable.
 */
void device_commands_set_en_dpl(device_commands *self, bool value);

/**
 * Gets the value of EN_DYN_ACK from the FEATURE register.
 * @param self Pointer to the device_commands struct to use.
 * @param value Pointer to a variable where the EN_DYN_ACK value will be stored.
 */
void device_commands_get_en_dyn_ack(device_commands *self, bool *value);

/**
 * Sets the value of EN_DYN_ACK in the FEATURE register.
 * @param self Pointer to the device_commands struct to use.
 * @param value true to enable, false to disable.
 */
void device_commands_set_en_dyn_ack(device_commands *self, bool value);

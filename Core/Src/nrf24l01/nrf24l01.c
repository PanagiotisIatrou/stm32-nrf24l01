#include "nrf24l01.h"

#include <stdio.h>
#include <string.h>

bool nrf24l01_init(nrf24l01 *self, uint8_t *address_prefix, SPI_HandleTypeDef* spi, GPIO_TypeDef *csn_port, uint16_t csn_pin, GPIO_TypeDef *ce_port, uint16_t ce_pin) {
    spi_interface_init(&self->spi_handler, spi, csn_port, csn_pin, ce_port, ce_pin);
    device_commands_init(&self->commands_handler, &self->spi_handler);

    // Allow NO-ACK packets
    device_commands_set_en_dyn_ack(&self->commands_handler, 1);

    // Enable dynamic packet width
    device_commands_set_en_dpl(&self->commands_handler, 1);

    // Disable all pipes
    for (int i = 0; i < 6; i++) {
        device_commands_set_erx(&self->commands_handler, i, 0);
    }

    // Set RX pipe address 0, 1 and TX address to address_prefix + 0x00
    uint8_t address[5];
    memcpy(address, address_prefix, 4);
    address[4] = 0x00;
    device_commands_set_tx_addr_full(&self->commands_handler, address);
    device_commands_set_rx_addr_full(&self->commands_handler, 0, address);
    device_commands_set_rx_addr_full(&self->commands_handler, 1, address);

    // Reset other pipe addresses
    for (int i = 2; i < 6; i++) {
        device_commands_set_rx_addr_lsb(&self->commands_handler, i, 0x00);
    }

    // Flush TX/RX FIFO
    device_commands_flush_tx(&self->commands_handler);
    device_commands_flush_rx(&self->commands_handler);

    return true;
}

bool nrf24l01_get_power_state(nrf24l01 *self) {
    bool power_state;
    device_commands_get_pwr_up(&self->commands_handler, &power_state);
    return power_state;
}

void nrf24l01_power_up(nrf24l01 *self) {
    device_commands_set_pwr_up(&self->commands_handler, 1);

    // Wait for the Tpd2stby delay
    HAL_Delay(5000);
}

void nrf24l01_power_down(nrf24l01 *self) {
    device_commands_set_pwr_up(&self->commands_handler, 0);
}

void nrf24l01_set_pipe0_write(nrf24l01 *self, uint8_t address) {
    device_commands_set_tx_addr_lsb(&self->commands_handler, address);

    device_commands_set_rx_addr_lsb(&self->commands_handler, 0, address);
    device_commands_set_rx_pw(&self->commands_handler, 0, 0);
    device_commands_set_dpl(&self->commands_handler, 0, 1);
    device_commands_set_erx(&self->commands_handler, 0, 1);
}

void nrf24l01_set_pipe_read(nrf24l01 *self, uint32_t pipe, uint8_t address) {
    device_commands_set_rx_addr_lsb(&self->commands_handler, pipe, address);
    device_commands_set_rx_pw(&self->commands_handler, pipe, 32);
    device_commands_set_dpl(&self->commands_handler, pipe, 1);
    device_commands_set_erx(&self->commands_handler, pipe, 1);
}

uint8_t nrf24l01_get_channel(nrf24l01 *self) {
    uint8_t channel;
    device_commands_get_rf_ch(&self->commands_handler, &channel);
    return channel;
}

void nrf24l01_set_channel(nrf24l01 *self, uint8_t channel) {
    if (channel > 125) {
        printf("Valid channel range: [0, 125]. Given is %d\r\n", channel);
        return;
    }

    device_commands_set_rf_ch(&self->commands_handler, channel);
}

DataRate nrf24l01_get_data_rate(nrf24l01 *self) {
    // Get RF_DR_LOW and RF_DR_HIGH
    bool rf_dr_low, rf_dr_high;
    device_commands_get_rf_dr_low(&self->commands_handler, &rf_dr_low);
    device_commands_get_rf_dr_high(&self->commands_handler, &rf_dr_high);

    // Figure out DataRate
    if (rf_dr_low) {
        return DATA_RATE_LOW;
    } else if (rf_dr_high) {
        return DATA_RATE_HIGH;
    } else {
        return DATA_RATE_MEDIUM;
    }
}

void nrf24l01_set_data_rate(nrf24l01 *self, DataRate data_rate) {
    // Set RF_DR_LOW
    bool rf_dr_low = data_rate == DATA_RATE_LOW;
    device_commands_set_rf_dr_low(&self->commands_handler, rf_dr_low);

    // Set RF_DR_HIGH
    bool rf_dr_high = data_rate == DATA_RATE_HIGH;
    device_commands_set_rf_dr_high(&self->commands_handler, rf_dr_high);
}

PowerLevel nrf24l01_get_power_level(nrf24l01 *self) {
    // Extract RF_PWR
    uint8_t rf_pwr;
    device_commands_get_rf_pwr(&self->commands_handler, &rf_pwr);

    // Figure out the PowerLevel
    if (rf_pwr == 0) {
        return POWER_LEVEL_LOW;
    } else if (rf_pwr == 1) {
        return POWER_LEVEL_MEDIUM;
    } else if (rf_pwr == 2) {
        return POWER_LEVEL_HIGH;
    } else {
        return POWER_LEVEL_VERY_HIGH;
    }
}

void nrf24l01_set_power_level(nrf24l01 *self, PowerLevel power_level) {
    bool msb = power_level == POWER_LEVEL_HIGH || power_level == POWER_LEVEL_VERY_HIGH;
    bool lsb = power_level == POWER_LEVEL_MEDIUM || power_level == POWER_LEVEL_VERY_HIGH;
    uint8_t rf_pwr = lsb | (msb << 1);
    device_commands_set_rf_pwr(&self->commands_handler, rf_pwr);
}

uint8_t nrf24l01_get_retransmit_delay(nrf24l01 *self) {
    uint8_t ard;
    device_commands_get_ard(&self->commands_handler, &ard);
    return ard;
}

void nrf24l01_set_retransmit_delay(nrf24l01 *self, uint8_t delay) {
    if (delay > 15) {
        printf("Valid retransmit delay range: [0, 15]. Given is %d\r\n", delay);
        return;
    }

    device_commands_set_ard(&self->commands_handler, delay);
}

uint8_t nrf24l01_get_retransmit_count(nrf24l01 *self) {
    uint8_t arc;
    device_commands_get_arc(&self->commands_handler, &arc);
    return arc;
}

void nrf24l01_set_retransmit_count(nrf24l01 *self, uint8_t count) {
    if (count > 15) {
        printf("Valid retransmit count range: [0, 15]. Given is %d\r\n", count);
        return;
    }

    device_commands_set_arc(&self->commands_handler, count);
}

void set_crc_bytes(nrf24l01 *self, CrcBytes count) {
    bool value = count == CRC_BYTES_1 ? 0 : 1;
    device_commands_set_crco(&self->commands_handler, value);
}

void nrf24l01_send_packets(nrf24l01 *self, uint8_t **value, int count, uint8_t *packet_lengths, bool resend_lost_packets) {
    // Set TX mode
    device_commands_set_prim_rx(&self->commands_handler, 0);

    // Clear any leftover packets
    device_commands_flush_tx(&self->commands_handler);

    // Preload the FIFO clamp
    int preload_count = (count > 3 ? 3 : count);
    for (int i = 0; i < preload_count; i++) {
        // Write the payload
        device_commands_w_tx_payload(&self->commands_handler, value[i], packet_lengths[i]);
    }

    // Start sending
    spi_interface_enable_ce(&self->spi_handler);
    for (int i = 0; i < count; i++) {
        // Wait for TX_DS or MAX_RT
        while (true) {
            bool tx_ds;
            device_commands_get_tx_ds(&self->commands_handler, &tx_ds);

            bool max_rt;
            device_commands_get_max_rt(&self->commands_handler, &max_rt);

            if (tx_ds) {
                device_commands_clear_tx_ds(&self->commands_handler);

                // Send the next packet if it exists
                if (i + preload_count < count) {
                    // Write the payload
                    int index = i + preload_count;
                    device_commands_w_tx_payload(&self->commands_handler, value[index], packet_lengths[index]);
                }
                break;
            }
            if (max_rt) {
                if (!resend_lost_packets) {
                    spi_interface_disable_ce(&self->spi_handler);
                }

                device_commands_clear_max_rt(&self->commands_handler);

                if (!resend_lost_packets) {
                    device_commands_flush_tx(&self->commands_handler);

                    // Re-queue the flushed packets except the lost one
                    for (int j = 1; j < 4; j++) {
                        if (i + j < count) {
                            // Write the payload
                            int index = i + j;
                            device_commands_w_tx_payload(&self->commands_handler, value[index], packet_lengths[index]);
                        }
                    }

                    spi_interface_enable_ce(&self->spi_handler);
                    break;
                }
            }
        }
    }

    spi_interface_disable_ce(&self->spi_handler);
}

void nrf24l01_send_packets_no_ack(nrf24l01 *self, uint8_t **value, int count, uint8_t *packet_lengths) {
    // Set TX mode
    device_commands_set_prim_rx(&self->commands_handler, 0);

    // Clear any leftover packets
    device_commands_flush_tx(&self->commands_handler);

    // Preload the FIFO clamp
    int preload_count = (count > 3 ? 3 : count);
    for (int i = 0; i < preload_count; i++) {
        // Write the payload
        device_commands_w_tx_payload_no_ack(&self->commands_handler, value[i], packet_lengths[i]);
    }

    // Send the packets
    spi_interface_enable_ce(&self->spi_handler);
    for (int i = 0; i < count; i++) {
        // Wait for TX_DS
        while (true) {
            bool tx_ds;
            device_commands_get_tx_ds(&self->commands_handler, &tx_ds);
            if (tx_ds) {
                device_commands_clear_tx_ds(&self->commands_handler);

                if (i + preload_count < count) {
                    // Write the payload
                    int index = i + preload_count;
                    device_commands_w_tx_payload_no_ack(&self->commands_handler, value[index], packet_lengths[index]);
                }

                break;
            }
        }
    }

    spi_interface_disable_ce(&self->spi_handler);
}

int nrf24l01_receive_packets(nrf24l01 *self, uint8_t **packets, int count, uint32_t timeout) {
    // Set RX mode
    device_commands_set_prim_rx(&self->commands_handler, 1);

    // Clear any leftover packets
    device_commands_flush_rx(&self->commands_handler);

    spi_interface_enable_ce(&self->spi_handler);
    uint32_t packets_read = 0;
    uint32_t last_packet_time = HAL_GetTick();
    while (true) {
        // Read RX_DR
        bool rx_dr;
        device_commands_get_rx_dr(&self->commands_handler, &rx_dr);
        if (!rx_dr) {
            int64_t time_ms = HAL_GetTick() - last_packet_time;
            if (time_ms > timeout && packets_read > 0) {
                return packets_read;
            }
            continue;
        }

        // Read packets as long as RX FIFO is not empty
        bool packets_left = true;
        while (packets_left) {
            last_packet_time = HAL_GetTick();

            // Read the payload width
            uint8_t payload_width;
            device_commands_r_rx_pl_wid(&self->commands_handler, &payload_width);

            // Flush RX if the payload is bigger than 32 bytes
            if (payload_width > 32) {
                device_commands_flush_rx(&self->commands_handler);
                break;
            }

            // Read the payload
            device_commands_r_rx_payload(&self->commands_handler, packets[packets_read], payload_width);
            packets_read++;
            if (packets_read == count) {
                break;
            }

            // Check RX_EMPTY to see if there are more packets
            bool rx_empty;
            device_commands_get_rx_empty(&self->commands_handler, &rx_empty);
            packets_left = !rx_empty;
        }

        // Clear RX_DR
        device_commands_clear_rx_dr(&self->commands_handler);

        if (packets_read == count) {
            break;
        }
    }

    spi_interface_disable_ce(&self->spi_handler);
    return packets_read;
}

void nrf24l01_receive_packets_inf(nrf24l01 *self, void (*value_callback)(uint8_t* packet, uint8_t packet_length)) {
    // Set RX mode
    device_commands_set_prim_rx(&self->commands_handler, 1);

    // Clear any leftover packets
    device_commands_flush_rx(&self->commands_handler);

    spi_interface_enable_ce(&self->spi_handler);
    while (true) {
        // Read RX_DR
        bool rx_dr;
        device_commands_get_rx_dr(&self->commands_handler, &rx_dr);
        if (!rx_dr) {
            continue;
        }

        // Read packets as long as RX FIFO is not empty
        bool packets_left = true;
        while (packets_left) {
            // Read the payload width
            uint8_t payload_width;
            device_commands_r_rx_pl_wid(&self->commands_handler, &payload_width);

            // Flush RX if the payload is bigger than 32 bytes
            if (payload_width > 32) {
                device_commands_flush_rx(&self->commands_handler);
                break;
            }

            // Read the payload
            uint8_t packet[payload_width];
            device_commands_r_rx_payload(&self->commands_handler, packet, payload_width);
            value_callback(packet, payload_width);

            // Check RX_EMPTY to see if there are more packets
            bool rx_empty;
            device_commands_get_rx_empty(&self->commands_handler, &rx_empty);
            packets_left = !rx_empty;
        }

        // Clear RX_DR
        device_commands_clear_rx_dr(&self->commands_handler);
    }

    spi_interface_disable_ce(&self->spi_handler);
}

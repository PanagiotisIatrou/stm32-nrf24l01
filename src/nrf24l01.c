#include "nrf24l01.h"

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "common.h"

bool nrf24l01_init(nrf24l01 *self, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce) {
    // Check if the provided SPI pin combination is valid
    spi_inst_t* spi = pins_to_spi(mosi, miso, sck);
    if (spi == NULL) {
        printf("Given pins do not corespond to the given SPI\n");
        return false;
    }

    spi_interface_init(&self->spi_handler, spi, mosi, miso, sck, csn, ce);
    device_commands_init(&self->commands_handler, &self->spi_handler);

    // Allow NO-ACK packets
    device_commands_set_en_dyn_ack(&self->commands_handler, 1);

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
    sleep_us(5000);
}

void nrf24l01_power_down(nrf24l01 *self) {
    device_commands_set_pwr_up(&self->commands_handler, 0);
}

void nrf24l01_config_tx(nrf24l01 *self, uint8_t *address) {
    device_commands_set_prim_rx(&self->commands_handler, 0);
    device_commands_set_tx_addr(&self->commands_handler, address);
    device_commands_set_rx_addr(&self->commands_handler, 0, address);
}

void nrf24l01_config_rx(nrf24l01 *self, uint8_t *address) {
    device_commands_set_prim_rx(&self->commands_handler, 1);
    device_commands_set_rx_addr(&self->commands_handler, 1, address);
    device_commands_set_rx_pw(&self->commands_handler, 1, 32);
}

uint8_t nrf24l01_get_channel(nrf24l01 *self) {
    uint8_t channel;
    device_commands_get_rf_ch(&self->commands_handler, &channel);
    return channel;
}

void nrf24l01_set_channel(nrf24l01 *self, uint8_t channel) {
    if (channel > 125) {
        printf("Valid channel range: [0, 125]. Given is %d\n", channel);
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
        printf("Valid retransmit delay range: [0, 15]. Given is %d\n", delay);
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
        printf("Valid retransmit count range: [0, 15]. Given is %d\n", count);
        return;
    }

    device_commands_set_arc(&self->commands_handler, count);
}

void nrf24l01_send_packets(nrf24l01 *self, uint8_t **value, int count) {
    // Clear any leftover packets
    device_commands_flush_tx(&self->commands_handler);
    
    // Send the packets
    for (int i = 0; i < count; i++) {
        // Write the payload
        device_commands_w_tx_payload(&self->commands_handler, value[i], 32);

        // Start the transmission
        spi_interface_pulse_ce(&self->spi_handler);

        // Wait for TX_DS or MAX_RT
        uint8_t status;
        while (true) {
            bool tx_ds;
            device_commands_get_tx_ds(&self->commands_handler, &tx_ds);

            bool max_rt;
            device_commands_get_max_rt(&self->commands_handler, &max_rt);

            if (tx_ds) {
                device_commands_clear_tx_ds(&self->commands_handler);
                break;
            }
            if (max_rt) {
                device_commands_clear_max_rt(&self->commands_handler);

                // Pulse CE to restart the transmission of the packet
                spi_interface_pulse_ce(&self->spi_handler);
            }
        }
    }
}

void nrf24l01_send_packets_fast(nrf24l01 *self, uint8_t **value, int count) {
    // Clear any leftover packets
    device_commands_flush_tx(&self->commands_handler);

    // Preload the FIFO clamp
    int preload_count = (count > 3 ? 3 : count);
    for (int i = 0; i < preload_count; i++) {
        // Write the payload
        device_commands_w_tx_payload(&self->commands_handler, value[i], 32);
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
                    device_commands_w_tx_payload(&self->commands_handler, value[i + preload_count], 32);
                }
                break;
            }
            if (max_rt) {
                device_commands_clear_max_rt(&self->commands_handler);
            }
        }
    }

    spi_interface_disable_ce(&self->spi_handler);
}

void nrf24l01_send_packets_no_ack(nrf24l01 *self, uint8_t **value, int count) {
    // Clear any leftover packets
    device_commands_flush_tx(&self->commands_handler);

    // Send the packets
    for (int i = 0; i < count; i++) {
        // Write the payload
        device_commands_w_tx_payload_no_ack(&self->commands_handler, value[i], 32);

        // Start the transmission
        spi_interface_pulse_ce(&self->spi_handler);

        // Wait for TX_DS
        while (true) {
            bool tx_ds;
            device_commands_get_tx_ds(&self->commands_handler, &tx_ds);
            if (tx_ds) {
                device_commands_clear_tx_ds(&self->commands_handler);
                break;
            }
        }
    }
}

void nrf24l01_send_packets_no_ack_fast(nrf24l01 *self, uint8_t **value, int count) {
    // Clear any leftover packets
    device_commands_flush_tx(&self->commands_handler);

    // Preload the FIFO clamp
    int preload_count = (count > 3 ? 3 : count);
    for (int i = 0; i < preload_count; i++) {
        // Write the payload
        device_commands_w_tx_payload_no_ack(&self->commands_handler, value[i], 32);
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
                    device_commands_w_tx_payload_no_ack(&self->commands_handler, value[i + preload_count], 32);
                }

                break;
            }
        }
    }

    spi_interface_disable_ce(&self->spi_handler);
}

void nrf24l01_receive_packets(nrf24l01 *self, uint8_t **packets, int count) {
    device_commands_flush_rx(&self->commands_handler);

    spi_interface_enable_ce(&self->spi_handler);

    int packets_read = 0;
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
            // Read the payload
            device_commands_r_rx_payload(&self->commands_handler, packets[packets_read], 32);
            if (packets_read == packets[packets_read][0]) {
                packets_read++;
                if (packets_read == count) {
                    break;
                }
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
}

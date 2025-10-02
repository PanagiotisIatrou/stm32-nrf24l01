#include "nrf24l01.h"

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "common.h"

bool nrf24l01_init(nrf24l01 *device, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce) {
    // Check if the provided SPI pin combination is valid
    device->spi = pins_to_spi(mosi, miso, sck);
    if (device->spi == NULL) {
        return false;
    }

    // Initialize SPI
    if (device->spi == spi0 && !initialized_spi0) {
        initialized_spi0 = true;
        spi_init(device->spi, 4000000);
    } else if (device->spi == spi1 && !initialized_spi1) {
        initialized_spi1 = true;
        spi_init(device->spi, 4000000);
    }

    // Initialize the device struct
    device->mosi = mosi;
    device->miso = miso;
    device->sck = sck;
    device->csn = csn;
    device->ce = ce;

    // Setup CSN pin
    gpio_init(csn);
    gpio_set_dir(csn, GPIO_OUT);
    gpio_put(csn, 1);

    // Setup CE pin
    gpio_init(ce);
    gpio_set_dir(ce, GPIO_OUT);
    gpio_put(ce, 0);

    // Setup MISO, SCK and MOSI pins
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    // Initialize the register map
    register_map_init(&device->register_map, device->csn, device->spi);

    // Set EN_DYN_ACK to allow NO ACK packets
    uint8_t feature;
    register_map_read_register(&device->register_map, 0x1D, &feature, 1);
    feature |= 0b00000001;
    register_map_write_register(&device->register_map, 0x1D, &feature, 1);

    // Flush TX FIFO
    gpio_put(device->csn, 0);
    uint8_t cmd = 0b11100001;
    spi_write_blocking(device->spi, &cmd, 1);
    gpio_put(device->csn, 1);

    // Flush RX FIFO
    gpio_put(device->csn, 0);
    cmd = 0b11100010;
    spi_write_blocking(device->spi, &cmd, 1);
    gpio_put(device->csn, 1);

    return true;
}

bool nrf24l01_get_power_state(nrf24l01 *device) {
    // Read CONFIG
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Extract PWR_UP bit
    return ((config & 0b00000010) >> 1);
}

void nrf24l01_power_up(nrf24l01 *device) {
    // Write PWR_UP = 1 to the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);
    config |= 0b00000010;
    register_map_write_register(&device->register_map, 0x00, &config, 1);

    // Wait for the Tpd2stby delay
    sleep_us(5000);
}

void nrf24l01_power_down(nrf24l01 *device) {
    // Write PWR_UP = 0 to the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);
    config &= 0b11111101;
    register_map_write_register(&device->register_map, 0x00, &config, 1);

    // Wait for the Tpd2stby delay
    sleep_us(5000);
}

void nrf24l01_config_tx(nrf24l01 *device, uint8_t *value) {
    // Set as primary TX
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);
    config &= 0b11111110;
    register_map_write_register(&device->register_map, 0x00, &config, 1);
    
    // Write TX_ADDR
    register_map_write_register(&device->register_map, 0x10, value, 5);

    // Write RX_ADDR_P0
    register_map_write_register(&device->register_map, 0x0A, value, 5);
}

void nrf24l01_config_rx(nrf24l01 *device, uint8_t *value) {
    // Set as primary RX
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);
    config |= 0b00000001;
    register_map_write_register(&device->register_map, 0x00, &config, 1);

    // Write RX_ADDR_P1
    register_map_write_register(&device->register_map, 0x0B, value, 5);

    // Write RX_PW_P1
    uint8_t len = 32;
    register_map_write_register(&device->register_map, 0x12, &len, 1);
}

uint8_t nrf24l01_get_channel(nrf24l01 *device, uint8_t channel) {
    // Read RF_CH
    uint8_t rf_ch;
    register_map_read_register(&device->register_map, 0x05, &rf_ch, 1);

    // Extract the channel bits
    return (rf_ch & 0b01111111);
}

void nrf24l01_set_channel(nrf24l01 *device, uint8_t channel) {
    if (channel > 125) {
        printf("Valid channel range: [0, 125]. Given is %d\n", channel);
        return;
    }

    // Read RF_CH
    uint8_t rf_ch;
    register_map_read_register(&device->register_map, 0x05, &rf_ch, 1);

    // Write RF_CH
    rf_ch = (rf_ch & 0b10000000) | channel;
    register_map_write_register(&device->register_map, 0x05, &rf_ch, 1);
}

void nrf24l01_set_data_rate(nrf24l01 *device, DataRate data_rate) {
    // Read RF_SETUP
    uint8_t rf_setup;
    register_map_read_register(&device->register_map, 0x06, &rf_setup, 1);

    // Edit the register
    bool rf_dr_low = data_rate == DATA_RATE_LOW;
    bool rf_dr_high = data_rate == DATA_RATE_HIGH;
    if (rf_dr_low) {
        rf_setup |= 0b00100000;
    } else {
        rf_setup &= 0b11011111;
    }
    if (rf_dr_high) {
        rf_setup |= 0b00001000;
    } else {
        rf_setup &= 0b11110111;
    }

    // Write RF_SETUP
    register_map_write_register(&device->register_map, 0x06, &rf_setup, 1);
}

void nrf24l01_set_power_level(nrf24l01 *device, PowerLevel power_level) {
    // Read RF_SETUP
    uint8_t rf_setup;
    register_map_read_register(&device->register_map, 0x06, &rf_setup, 1);

    // Edit the register
    bool msb = power_level == POWER_LEVEL_HIGH || power_level == POWER_LEVEL_VERY_HIGH;
    bool lsb = power_level == POWER_LEVEL_MEDIUM || power_level == POWER_LEVEL_VERY_HIGH;
    if (msb) {
        rf_setup |= 0b00000100;
    } else {
        rf_setup &= 0b11111011;
    }
    if (lsb) {
        rf_setup |= 0b00000010;
    } else {
        rf_setup &= 0b11111101;
    }
    
    // Write RF_SETUP
    register_map_write_register(&device->register_map, 0x06, &rf_setup, 1);
}

void nrf24l01_set_retransmit_delay(nrf24l01 *device, uint8_t delay) {
    if (delay > 15) {
        printf("Valid retransmit delay range: [0, 15]. Given is %d\n", delay);
        return;
    }

    // Read SETUP_RETR
    uint8_t setup_retr;
    register_map_read_register(&device->register_map, 0x04, &setup_retr, 1);

    // Edit the register
    setup_retr = (setup_retr & 0x0F) | (delay << 4);

    // Write SETUP_RETR
    register_map_write_register(&device->register_map, 0x04, &setup_retr, 1);
}

void nrf24l01_set_retransmit_count(nrf24l01 *device, uint8_t count) {
    if (count > 15) {
        printf("Valid retransmit count range: [0, 15]. Given is %d\n", count);
        return;
    }

    // Read SETUP_RETR
    uint8_t setup_retr;
    register_map_read_register(&device->register_map, 0x04, &setup_retr, 1);

    // Edit the register
    setup_retr = (setup_retr & 0xF0) | count;

    // Write SETUP_RETR
    register_map_write_register(&device->register_map, 0x04, &setup_retr, 1);
}

void nrf24l01_send_packets(nrf24l01 *device, uint8_t **value, int count) {
    // Send the packets
    for (int i = 0; i < count; i++) {
        // Write the payload
        gpio_put(device->csn, 0);
        uint8_t cmd = 0b10100000;
        uint8_t buffer[1 + 32];
        buffer[0] = cmd;
        memcpy(&buffer[1], value[i], 32);
        spi_write_blocking(device->spi, buffer, 33);
        gpio_put(device->csn, 1);

        // Start the transmission
        gpio_put(device->ce, 1);
        sleep_us(15);
        gpio_put(device->ce, 0);

        // Wait for either TX_DS or MAX_RT
        uint8_t status;
        while (true) {
            // Read TX_DS and MAX_RT
            register_map_read_register(&device->register_map, 0x07, &status, 1);
            bool tx_ds = (status & 0b00100000) >> 5;
            bool max_rt = (status & 0b00010000) >> 4;
            if (tx_ds) {
                // Clear TX_DS
                uint8_t cleared = 0b00100000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);
                break;
            }
            if (max_rt) {
                // Clear MAX_rt
                uint8_t cleared = 0b00010000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);

                // Pulse CE to restart the transmission of the packet
                gpio_put(device->ce, 1);
                sleep_us(15);
                gpio_put(device->ce, 0);
            }
        }
    }
}

void nrf24l01_send_packets_fast(nrf24l01 *device, uint8_t **value, int count) {
    // Clear any leftover packets
    gpio_put(device->csn, 0);
    uint8_t cmd = 0b11100001; // FLUSH_TX
    spi_write_blocking(device->spi, &cmd, 1);
    gpio_put(device->csn, 1);

    // Preload the FIFO clamp
    int preload_count = (count > 3 ? 3 : count);
    for (int i = 0; i < preload_count; i++) {
        // Write the payload
        gpio_put(device->csn, 0);
        uint8_t cmd = 0b10100000;
        uint8_t buffer[1 + 32];
        buffer[0] = cmd;
        memcpy(&buffer[1], value[i], 32);
        spi_write_blocking(device->spi, buffer, 33);
        gpio_put(device->csn, 1);
    }

    // Start sending
    gpio_put(device->ce, 1);
    for (int i = 0; i < count; i++) {
        while (true) {
            // Wait for TX_DS or MAX_RT
            uint8_t status;
            register_map_read_register(&device->register_map, 0x07, &status, 1);
            bool tx_ds = (status & 0b00100000) >> 5;
            bool max_rt = (status & 0b00010000) >> 4;
            if (tx_ds) {
                // Clear TX_DS
                uint8_t cleared = 0b00100000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);

                // Send the next packet if it exists
                if (i + preload_count < count) {
                    // Write the payload
                    gpio_put(device->csn, 0);
                    uint8_t cmd = 0b10100000;
                    uint8_t buffer[1 + 32];
                    buffer[0] = cmd;
                    memcpy(&buffer[1], value[i + preload_count], 32);
                    spi_write_blocking(device->spi, buffer, 33);
                    gpio_put(device->csn, 1);
                }

                break;
            }
            if (max_rt) {
                // Clear MAX_RT
                uint8_t cleared = 0b00010000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);
            }
        }
    }

    gpio_put(device->ce, 0);
}

void nrf24l01_send_packets_no_ack(nrf24l01 *device, uint8_t **value, int count) {
    // Clear any leftover packets
    gpio_put(device->csn, 0);
    uint8_t cmd = 0b11100001; // FLUSH_TX
    spi_write_blocking(device->spi, &cmd, 1);
    gpio_put(device->csn, 1);

    // Send the packets
    for (int i = 0; i < count; i++) {
        // Write the payload
        gpio_put(device->csn, 0);
        uint8_t cmd = 0b10110000;
        uint8_t buffer[1 + 32];
        buffer[0] = cmd;
        memcpy(&buffer[1], value[i], 32);
        spi_write_blocking(device->spi, buffer, 33);
        gpio_put(device->csn, 1);

        // Start the transmission
        gpio_put(device->ce, 1);
        sleep_us(15);
        gpio_put(device->ce, 0);

        // Wait for TX_DS
        while (true) {
            uint8_t status;
            register_map_read_register(&device->register_map, 0x07, &status, 1);
            bool tx_ds = (status & 0b00100000) >> 5;
            if (tx_ds) {
                // Clear TX_DS
                uint8_t cleared = 0b00100000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);
                break;
            }
        }
    }
}

void nrf24l01_send_packets_no_ack_fast(nrf24l01 *device, uint8_t **value, int count) {
    // Clear any leftover packets
    gpio_put(device->csn, 0);
    uint8_t cmd = 0b11100001; // FLUSH_TX
    spi_write_blocking(device->spi, &cmd, 1);
    gpio_put(device->csn, 1);

    // Preload the FIFO clamp
    int preload_count = (count > 3 ? 3 : count);
    for (int i = 0; i < preload_count; i++) {
        // Write the payload
        gpio_put(device->csn, 0);
        uint8_t cmd = 0b10110000;
        uint8_t buffer[1 + 32];
        buffer[0] = cmd;
        memcpy(&buffer[1], value[i], 32);
        spi_write_blocking(device->spi, buffer, 33);
        gpio_put(device->csn, 1);
    }


    // Send the packets
    gpio_put(device->ce, 1);
    for (int i = 0; i < count; i++) {
        // Wait for TX_DS
        while (true) {
            uint8_t status;
            register_map_read_register(&device->register_map, 0x07, &status, 1);
            bool tx_ds = (status & 0b00100000) >> 5;
            if (tx_ds) {
                // Clear TX_DS
                uint8_t cleared = 0b00100000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);

                if (i + preload_count < count) {
                    // Write the payload
                    gpio_put(device->csn, 0);
                    uint8_t cmd = 0b10110000;
                    uint8_t buffer[1 + 32];
                    buffer[0] = cmd;
                    memcpy(&buffer[1], value[i + preload_count], 32);
                    spi_write_blocking(device->spi, buffer, 33);
                    gpio_put(device->csn, 1);
                }

                break;
            }
        }
    }

    gpio_put(device->ce, 0);
}

void nrf24l01_receive_packet(nrf24l01 *device) {
    gpio_put(device->ce, 1);
    sleep_us(130);

    uint64_t last_packet_time = time_us_64();

    int count = 0;
    while (true) {
        // Read RX_DR
        uint8_t status;
        register_map_read_register(&device->register_map, 0x07, &status, 1);
        bool rx_dr = (status & 0b01000000) >> 6;

        if (!rx_dr) {
            if (time_us_64() - last_packet_time > 5000000) {
                printf("Packet receive timeout after %d packets\n", count);
                break;
            }
            continue;
        }

        bool packets_left = true;
        while (packets_left) {
            last_packet_time = time_us_64();
            // Read the payload
            gpio_put(device->csn, 0);
            uint8_t cmd = 0b01100001;
            spi_write_blocking(device->spi, &cmd, 1);
            uint8_t bytes[32];
            spi_read_blocking(device->spi, 0xFF, bytes, 32);
            gpio_put(device->csn, 1);

            // Clear RX_DR
            uint8_t cleared = 0b01000000;
            register_map_write_register(&device->register_map, 0x07, &cleared, 1);

            if (count % 3 == 0 && bytes[0] != 0xA1
                || count % 3 == 1 && bytes[0] != 0xB2
                || count % 3 == 2 && bytes[0] != 0xC3) {
                printf("Packet error at count %d: 0x%02X\n", count, bytes[0]);
            }
            count++;

            if (count == 5000) {
                printf("Received 5000 packets\n");
                return;
            }

            // Check RX_EMPTY to see if there are more packets
            uint8_t fifo_status;
            register_map_read_register(&device->register_map, 0x17, &fifo_status, 1);
            uint8_t fifo_empty = fifo_status & 0b00000001;
            packets_left = !fifo_empty;
        }
    }
    gpio_put(device->ce, 0);
}

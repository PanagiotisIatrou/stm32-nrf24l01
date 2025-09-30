#include "nrf24l01.h"

#include <stdio.h>
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

    return true;
}

void nrf24l01_power_up(nrf24l01 *device) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    config |= 0b00000010;
    register_map_write_register(&device->register_map, 0x00, &config, 1);

    // Wait for the Tpd2stby delay
    sleep_us(5000);
}

void nrf24l01_set_as_primary_tx(nrf24l01 *device) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    config &= 0b11111110;
    register_map_write_register(&device->register_map, 0x00, &config, 1);
}

void nrf24l01_set_as_primary_rx(nrf24l01 *device) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    config |= 0b00000001;
    register_map_write_register(&device->register_map, 0x00, &config, 1);
}

void nrf24l01_config_tx(nrf24l01 *device, uint8_t *value) {
    // Write TX_ADDR
    register_map_write_register(&device->register_map, 0x10, value, 5);

    // Write RX_ADDR_P0
    register_map_write_register(&device->register_map, 0x0A, value, 5);
}

void nrf24l01_config_rx(nrf24l01 *device, uint8_t *value) {
    // Write RX_ADDR_P1
    register_map_write_register(&device->register_map, 0x0B, value, 5);

    // Write RX_PW_P1
    uint8_t len = 32;
    register_map_write_register(&device->register_map, 0x12, &len, 1);
}

void nrf24l01_send_packets(nrf24l01 *device, uint8_t **value, int count) {
    // Send the packets
    for (int i = 0; i < count; i++) {
        // Write the payload
        gpio_put(device->csn, 0);
        uint8_t cmd = 0b10100000;
        spi_write_blocking(device->spi, &cmd, 1);
        spi_write_blocking(device->spi, value[i], 32);
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
        gpio_put(device->csn, 0);
        uint8_t cmd = 0b10100000;
        spi_write_blocking(device->spi, &cmd, 1);
        spi_write_blocking(device->spi, value[i], 32);
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
                    gpio_put(device->csn, 0);
                    uint8_t cmd = 0b10100000;
                    spi_write_blocking(device->spi, &cmd, 1);
                    spi_write_blocking(device->spi, value[i + preload_count], 32);
                    gpio_put(device->csn, 1);
                }

                break;
            }
            if (max_rt) {
                // Clear MAX_RT
                uint8_t cleared = 0b00010000;
                register_map_write_register(&device->register_map, 0x07, &cleared, 1);

                // Pulse CE to restart the transmission of the packet
                gpio_put(device->ce, 0);
                sleep_us(15);
                gpio_put(device->ce, 1);
            }
        }
    }

    gpio_put(device->ce, 0);
}

void nrf24l01_receive_packet(nrf24l01 *device, uint8_t *value) {
    int count = 0;
    while (true) {
        // Read RX_DR
        uint8_t status;
        register_map_read_register(&device->register_map, 0x07, &status, 1);
        bool rx_dr = (status & 0b01000000) >> 6;

        if (rx_dr) {
            // Clear RX_DR
            uint8_t cleared = 0b01000000;
            register_map_write_register(&device->register_map, 0x07, &cleared, 1);

            bool packets_left = true;
            while (packets_left) {
                // Read the payload
                gpio_put(device->csn, 0);
                uint8_t cmd = 0b01100001;
                spi_write_blocking(device->spi, &cmd, 1);
                uint8_t bytes[32];
                spi_read_blocking(device->spi, 0xFF, bytes, 32);
                gpio_put(device->csn, 1);

                // value[count] = bytes[0];
                // printf("Packet first byte: 0x%02X\n", bytes[0]);
                count++;
                // if (count == 1000) {
                //     return;
                // }

                // Check RX_EMPTY to see if there are more packets
                uint8_t fifo_status;
                register_map_read_register(&device->register_map, 0x17, &fifo_status, 1);
                bool rx_empty = (fifo_status & 0b00000001);
                packets_left = !rx_empty;
            }
        }
    }
}

void nrf24l01_start_listening(nrf24l01 *device) {
    gpio_put(device->ce, 1);
    sleep_us(130);
}

void nrf24l01_stop_listening(nrf24l01 *device) {
    gpio_put(device->ce, 0);
}

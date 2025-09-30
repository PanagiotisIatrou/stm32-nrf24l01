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
        spi_init(device->spi, 1000000);
    } else if (device->spi == spi1 && !initialized_spi1) {
        initialized_spi1 = true;
        spi_init(device->spi, 1000000);
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

void nrf24l01_send_packet(nrf24l01 *device, uint8_t *value) {
    // Write the payload
    gpio_put(device->csn, 0);
    uint8_t cmd = 0b10100000;
    spi_write_blocking(device->spi, &cmd, 1);
    spi_write_blocking(device->spi, value, 32);
    gpio_put(device->csn, 1);

    // Enable TX mode
    gpio_put(device->ce, 1);
    sleep_us(140); // 10 + 130

    // Wait for either TX_DS or MAX_RT
    uint8_t status;
    while (true) {
        // Read the config register
        register_map_read_register(&device->register_map, 0x07, &status, 1);
        bool tx_ds = (status & 0b00100000) >> 5;
        bool max_rt = (status & 0b00010000) >> 4;
        if (tx_ds) {
            printf("tx_ds\n");
            break;
        }
        if (max_rt) {
            printf("max_rt\n");
            break;
        }
    }

    // Disable TX mode
    gpio_put(device->ce, 0);

    // Clear TX_DS and MAX_RT
    uint8_t cleared = status | 0b00110000;
    register_map_write_register(&device->register_map, 0x07, &cleared, 1);
}

void nrf24l01_receive_packet(nrf24l01 *device, bool *value) {
    // Read RX_DR
    uint8_t status;
    register_map_read_register(&device->register_map, 0x07, &status, 1);
    *value = (status & 0b01000000) >> 6;

    if (*value) {
        gpio_put(device->csn, 0);

        // Write the command
        uint8_t cmd = 0b01100001;
        spi_write_blocking(device->spi, &cmd, 1);

        // Write the bytes
        uint8_t bytes[32];
        spi_read_blocking(device->spi, 0xFF, bytes, 32);

        gpio_put(device->csn, 1);

        printf("Byte 0: 0x%02X\n", bytes[0]);
        printf("Byte 31: 0x%02X\n", bytes[31]);
    }
}

void nrf24l01_start_listening(nrf24l01 *device) {
    gpio_put(device->ce, 1);
    sleep_us(130);
}

void nrf24l01_stop_listening(nrf24l01 *device) {
    gpio_put(device->ce, 0);
}

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "src/nrf24l01.h"
#include <stdlib.h>
#include <string.h>

void rx() {
    nrf24l01 device;
    if (!nrf24l01_init(&device, 11, 12, 10, 21, 20)) {
        printf("Could not initialize RX device\n");
    }

    // Power up the device
    nrf24l01_power_up(&device);

    // Configure device as RX
    nrf24l01_set_as_primary_rx(&device);

    // Configure RX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x07 };
    nrf24l01_config_rx(&device, address);

    // Flush TX FIFO
    gpio_put(device.csn, 0);
    uint8_t cmd = 0b11100001;
    spi_write_blocking(device.spi, &cmd, 1);
    gpio_put(device.csn, 1);

    // Flush RX FIFO
    gpio_put(device.csn, 0);
    cmd = 0b11100010;
    spi_write_blocking(device.spi, &cmd, 1);
    gpio_put(device.csn, 1);

    // Enable RX device
    nrf24l01_start_listening(&device);

    uint8_t received[3];
    nrf24l01_receive_packet(&device, received);

    // Disable RX device
    nrf24l01_stop_listening(&device);
}

void tx() {
    sleep_ms(2000);

    // Initialize the devices
    nrf24l01 device;
    if (!nrf24l01_init(&device, 19, 16, 18, 27, 26)) {
        printf("Could not initialize TX device\n");
    }

    // Power up the device
    nrf24l01_power_up(&device);

    // Configure device as TX
    nrf24l01_set_as_primary_tx(&device);

    // Configure TX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x07 };
    nrf24l01_config_tx(&device, address);

    // Flush TX FIFO
    gpio_put(device.csn, 0);
    uint8_t cmd = 0b11100001;
    spi_write_blocking(device.spi, &cmd, 1);
    gpio_put(device.csn, 1);

    // Flush RX FIFO
    gpio_put(device.csn, 0);
    cmd = 0b11100010;
    spi_write_blocking(device.spi, &cmd, 1);
    gpio_put(device.csn, 1);

    uint8_t packet0[32] = {
        0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
        0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
        0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
        0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1
    };
    uint8_t packet1[32] = {
        0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
        0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
        0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
        0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2
    };
    uint8_t packet2[32] = {
        0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
        0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
        0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
        0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3
    };
    uint8_t *payload[5000];

    for (size_t i = 0; i < 5000; i++) {
        payload[i] = malloc(32);
        if (!payload[i]) {
            printf("Malloc failed at index %zu\n", i);
            // Free previously allocated packets
            for (size_t j = 0; j < i; j++) free(payload[j]);
            return;
        }
        // Round-robin fill
        switch (i % 3) {
            case 0: memcpy(payload[i], packet0, 32); break;
            case 1: memcpy(payload[i], packet1, 32); break;
            case 2: memcpy(payload[i], packet2, 32); break;
        }
    }
    printf("Starting transmission of 5000 packets...\n");

    uint64_t start_time = time_us_64();

    nrf24l01_send_packets_fast(&device, payload, 5000);

    uint64_t end_time = time_us_64();
    uint64_t elapsed_time_us = end_time - start_time;
    printf("Execution time: %llu microseconds\n", elapsed_time_us);
}

int main()
{
    // Enable stdio and wait for serial monitor to connect
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Wait for init
    sleep_ms(100);
    
    multicore_launch_core1(tx);
    rx();

    sleep_ms(100000);
    return 0;
}

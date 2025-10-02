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

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_VERY_HIGH);

    // Configure as RX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x07 };
    nrf24l01_config_rx(&device, address);

    nrf24l01_receive_packet(&device);
}

void tx() {
    sleep_ms(2000);

    // Initialize the devices
    nrf24l01 device;
    if (!nrf24l01_init(&device, 19, 16, 18, 27, 26)) {
        printf("Could not initialize TX device\n");
    }

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_VERY_HIGH);

    // Configure as TX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x07 };
    nrf24l01_config_tx(&device, address);

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
        switch (i % 3) {
            case 0: payload[i] = packet0; break;
            case 1: payload[i] = packet1; break;
            case 2: payload[i] = packet2; break;
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

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "src/nrf24l01.h"
#include <stdlib.h>
#include <string.h>

#define RUNS 100

int count = 0;
void value_callback(uint8_t* packet) {
    count++;
    if (count >= RUNS * 128) {
        printf("Received %d packets\n", count);
    }
}

void rx() {
    // Initialize SPI1
    spi_init(spi1, 4000000);

    nrf24l01 device;
    if (!nrf24l01_init(&device, 11, 12, 10, 21, 20)) {
        printf("Could not initialize RX device\n");
    }

    nrf24l01_power_up(&device);

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_LOW);

    // Configure as RX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x07 };
    nrf24l01_config_rx(&device, address);

    nrf24l01_receive_packets_inf(&device, value_callback);
    // uint8_t *packets[128];
    // for (uint8_t i = 0; i < 128; i++) {
    //     packets[i] = malloc(32);
    // }
    // for (int i = 0; i < RUNS; i++) {
    //     nrf24l01_receive_packets(&device, packets, 128);
    // }
    printf("Finished receiving packets\n");
    sleep_ms(10000000);
}

void tx() {
    sleep_ms(2000);

    // Initialize SPI0
    spi_init(spi0, 4000000);

    // Initialize the devices
    nrf24l01 device;
    if (!nrf24l01_init(&device, 19, 16, 18, 27, 26)) {
        printf("Could not initialize TX device\n");
    }

    nrf24l01_power_up(&device);

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_LOW);

    // Configure as TX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x07 };
    nrf24l01_config_tx(&device, address);

    // Prepare the packets
    uint8_t *payload[128];
    printf("Preparing 128 packets...\n");
    for (uint8_t i = 0; i < 128; i++) {
        payload[i] = malloc(32);
        for (int j = 0; j < 32; j++) {
            payload[i][j] = i;
        }
    }
    printf("Starting transmission of packets...\n");

    uint64_t start_time = time_us_64();

    for (uint32_t k = 0; k < RUNS; k++) {
        nrf24l01_send_packets(&device, payload, 128);
    }

    uint64_t end_time = time_us_64();
    uint64_t elapsed_time_us = end_time - start_time;
    printf("Execution time: %llu microseconds %d\n", elapsed_time_us, count);
    sleep_ms(10000000);
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

    sleep_ms(10000000);
    return 0;
}

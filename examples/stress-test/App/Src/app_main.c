#include "app_main.h"

#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#include "nrf24l01.h"

#define CONFIGURE_TX
// #define CONFIGURE_RX

#define RUNS 10

int count = 0;

void value_callback(uint8_t *packet, uint8_t packet_length) {
    count++;
    if (count >= RUNS * 128) {
        printf("Received %d packets\r\n", count);
    }
}

void rx() {
    printf("Starting RX\r\n");

    // Initialize the device
    nrf24l01 device;
    uint8_t address_prefix[4] = {0xB3, 0xB4, 0xB5, 0xB6};
    if (!nrf24l01_init(&device, address_prefix, &hspi1, GPIOB, GPIO_PIN_0, GPIOB, GPIO_PIN_1)) {
        printf("Could not initialize TX device\r\n");
    }

    nrf24l01_power_up(&device);

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_LOW);

    // Configure as RX
    nrf24l01_set_pipe_read(&device, 1, 0x15);

    // nrf24l01_receive_packets_inf(&device, value_callback);
    uint8_t *packets[128];
    for (uint8_t i = 0; i < 128; i++) {
        packets[i] = malloc(32);
    }
    for (int i = 0; i < RUNS; i++) {
        int c;
        if ((c = nrf24l01_receive_packets(&device, packets, 128, 10)) != 128) {
            printf("Some packets were lost %d\n", c);
        }
    }
    printf("Finished receiving packets\n");
}

void tx() {
    HAL_Delay(1000);
    printf("Starting TX\r\n");

    // Initialize the device
    nrf24l01 device;
    uint8_t address_prefix[4] = {0xB3, 0xB4, 0xB5, 0xB6};
    if (!nrf24l01_init(&device, address_prefix, &hspi1, GPIOB, GPIO_PIN_0, GPIOB, GPIO_PIN_1)) {
        printf("Could not initialize TX device\r\n");
    }

    nrf24l01_power_up(&device);

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_LOW);

    // Configure as TX
    nrf24l01_set_pipe0_write(&device, 0x15);

    // Prepare the packets
    uint8_t *payload[128];
    uint8_t payload_lengths[128];
    printf("Preparing 128 packets...\r\n");
    for (uint8_t i = 0; i < 128; i++) {
        payload[i] = malloc(32);
        for (int j = 0; j < 32; j++) {
            payload[i][j] = i;
        }
        payload_lengths[i] = 32;
    }
    printf("Starting transmission of packets...\r\n");

    uint32_t start_time = HAL_GetTick();

    for (uint32_t k = 0; k < RUNS; k++) {
        nrf24l01_send_packets(&device, payload, 128, payload_lengths, true);
    }

    uint32_t elapsed_time_ms = HAL_GetTick() - start_time;
    printf("Execution time: %lu ms, count = %d\r\n", elapsed_time_ms, count);
}

void app_main() {
    printf("\x1B[2J\r\n");
    printf("Starting...\r\n");

#if defined(CONFIGURE_TX) && defined(CONFIGURE_RX)
    printf("Select only one of CONFIGURE_TX and CONFIGURE_RX\r\n");
#elif defined(CONFIGURE_TX)
    tx();
#elif defined(CONFIGURE_RX)
    rx();
#endif
}

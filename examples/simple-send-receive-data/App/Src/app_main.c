#include "app_main.h"

#include "main.h"
#include <stdio.h>
#include <string.h>

#include "nrf24l01.h"

// #define CONFIGURE_TX
#define CONFIGURE_RX

void rx() {
    printf("Starting RX\r\n");

    // Initialize the device
    nrf24l01 device;
    uint8_t address_prefix[4] = {0xB3, 0xB4, 0xB5, 0xB6};
    if (!nrf24l01_init(&device, address_prefix, &hspi1, GPIOB, GPIO_PIN_0, GPIOB, GPIO_PIN_1)) {
        printf("Could not initialize RX device\r\n");
    }

    nrf24l01_power_up(&device);

    nrf24l01_set_channel(&device, 32);

    nrf24l01_set_data_rate(&device, DATA_RATE_HIGH);

    nrf24l01_set_power_level(&device, POWER_LEVEL_LOW);

    // Configure as RX
    nrf24l01_set_pipe_read(&device, 1, 0x15);

    // Receive 2 packets
    uint8_t packet0[32];
    uint8_t packet1[32];
    uint8_t *packets[2] = {packet0, packet1};
    if (nrf24l01_receive_packets(&device, packets, 2, 10) != 2) {
        printf("Some packets were lost\n");
        return;
    }

    // Extract the string from the first packet
    uint8_t string_length = packets[0][0];
    char string[string_length + 1];
    memcpy(string, &packets[0][1], string_length);
    string[string_length] = '\0';
    printf("Received string: %s\r\n", string);

    // Extract the value from the second packet
    uint8_t value = packets[1][0];
    printf("Received value: %d\r\n", value);
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

    // Create the data to send
    char *string = "Hello World!";
    uint8_t value = 42;

    // Send 2 packets, one with the string and the other with the value
    uint8_t payload_lengths[2] = {32, 32};
    uint8_t packet0[32] = {0};
    packet0[0] = strlen(string);
    memcpy(&packet0[1], string, strlen(string));
    uint8_t packet1[32] = {0};
    packet1[0] = value;
    uint8_t *packets[2] = {packet0, packet1};
    nrf24l01_send_packets(&device, packets, 2, payload_lengths, true);

    printf("Sent the 2 packets\r\n");
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

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "src/nrf24l01.h"

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

    // Flush RX and TX FIFOs
    uint8_t flush = 0b11100001;
    register_map_write_register(&device.register_map, 0xE1, &flush, 1);
    flush = 0b11100010;
    register_map_write_register(&device.register_map, 0xE2, &flush, 1);

    // Enable RX device
    nrf24l01_start_listening(&device);

    bool received;
    nrf24l01_receive_packet(&device, &received);

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

    // Flush RX and TX FIFOs
    uint8_t flush = 0b11100001;
    register_map_write_register(&device.register_map, 0xE1, &flush, 1);
    flush = 0b11100010;
    register_map_write_register(&device.register_map, 0xE2, &flush, 1);

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
    uint8_t *payload[2] = { packet0, packet1 };
    for (int i = 0; ; i++) {
        printf("\n======== Sending packets %d =======\n", i);
        nrf24l01_send_packets(&device, payload, 2);
        sleep_ms(1000);
    }
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

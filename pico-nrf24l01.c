#include <stdio.h>
#include "pico/stdlib.h"

#include "src/nrf24l01.h"

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
    sleep_ms(1000);

    // Initialize the devices
    nrf24l01 device0, device1;
    if (!nrf24l01_init(&device0, 19, 16, 18, 14, 15)) {
        printf("Could not initialize device0\n");
    }
    if (!nrf24l01_init(&device1, 3, 4, 2, 12, 13)) {
        printf("Could not initialize device1\n");
    }

    // Power up the devices
    nrf24l01_power_up(&device0);
    nrf24l01_power_up(&device1);

    // Configure device0 as TX and device1 as RX
    nrf24l01_set_as_primary_tx(&device0);
    nrf24l01_set_as_primary_rx(&device1);

    // Configure TX/RX
    uint8_t address[5] = { 0xB3, 0xB4, 0xB5, 0xB6, 0x05 };
    nrf24l01_config_tx(&device0, address);
    nrf24l01_config_rx(&device1, address);

    // Enable both devices
    gpio_put(15, 1);
    gpio_put(13, 1);
    sleep_us(130);
}

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

    nrf24l01 device0;
    if (!nrf24l01_init(&device0, 19, 16, 18, 14, 15)) {
        printf("Could not initialize device0\n");
    }

    // Read register
    uint8_t config = nrf24l01_read_config(&device0);
    printf("Value: %d\n", config);

    // Write PWR_UP = 1 to the register
    config |= 0b00000010;
    nrf24l01_write_config(&device0, config);

    // Read register
    config = nrf24l01_read_config(&device0);
    printf("Value: %d\n", config);



    nrf24l01 device1;
    if (!nrf24l01_init(&device1, 3, 4, 2, 12, 13)) {
        printf("Could not initialize device1\n");
    }

    // Read register
    config = nrf24l01_read_config(&device1);
    printf("Value: %d\n", config);

    // Write PWR_UP = 1 to the register
    config |= 0b00000010;
    nrf24l01_write_config(&device1, config);

    // Read register
    config = nrf24l01_read_config(&device1);
    printf("Value: %d\n", config);
}

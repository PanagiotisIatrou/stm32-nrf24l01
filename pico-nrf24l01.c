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

    bool prim_rx;
    nrf24l01_read_prim_rx(&device0, &prim_rx);
    printf("prim_rx before: %d\n", prim_rx);

    nrf24l01_write_prim_rx(&device0, 1);

    nrf24l01_read_prim_rx(&device0, &prim_rx);
    printf("prim_rx after: %d\n", prim_rx);


    nrf24l01 device1;
    if (!nrf24l01_init(&device1, 3, 4, 2, 12, 13)) {
        printf("Could not initialize device1\n");
    }

    nrf24l01_read_prim_rx(&device1, &prim_rx);
    printf("prim_rx before: %d\n", prim_rx);

    nrf24l01_write_prim_rx(&device1, 1);

    nrf24l01_read_prim_rx(&device1, &prim_rx);
    printf("prim_rx after: %d\n", prim_rx);
}

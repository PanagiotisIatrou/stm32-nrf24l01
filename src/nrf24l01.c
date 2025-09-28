#include "nrf24l01.h"

#include "hardware/spi.h"
#include "common.h"

bool nrf24l01_init(nrf24l01 *device, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce) {
    // Check if the provided SPI pin combination is valid
    spi_inst_t* spi = pins_to_spi(mosi, miso, sck);
    if (spi == NULL) {
        return false;
    }

    // Initialize the device struct
    device->mosi = mosi,
    device->miso = miso,
    device->sck = sck,
    device->csn = csn,
    device->ce = ce,

    // Setup CSN pin
    gpio_init(csn);
    gpio_set_dir(csn, GPIO_OUT);
    gpio_put(csn, 1);

    // Setup CE pin
    gpio_init(ce);
    gpio_set_dir(ce, GPIO_OUT);
    gpio_put(ce, 0);

    // Setup MISO, SCK and MOSI pins
    if (spi == spi0 && !initialized_spi0) {
        initialized_spi0 = true;
        spi_init(spi, 1000000);
    } else if (spi == spi1 && !initialized_spi1) {
        initialized_spi1 = true;
        spi_init(spi, 1000000);
    }
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    return true;
}

uint8_t nrf24l01_read_config(nrf24l01 *device) {
    gpio_put(device->csn, 0);
    uint8_t cmd = 0x00;
    spi_write_blocking(spi0, &cmd, 1);
    uint8_t config = 0;
    spi_read_blocking(spi0, 0x00, &config, 1);
    gpio_put(device->csn, 1);

    return config;
}

void nrf24l01_write_config(nrf24l01 *device, uint8_t value) {
    gpio_put(device->csn, 0);
    uint8_t cmd = 0x20 | 0x00;
    spi_write_blocking(spi0, &cmd, 1);
    spi_write_blocking(spi0, &value, 1);
    gpio_put(device->csn, 1);
}

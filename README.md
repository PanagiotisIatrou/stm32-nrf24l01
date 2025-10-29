# stm32-nrf24l01

<a target="_blank" href="https://www.paypal.com/donate/?hosted_button_id=J65KNQYEK88ML">
  <img src="https://img.shields.io/badge/Donate-PayPal-green.svg">
</a>

STM32 C library for the nRF24L01 wireless RF transceiver. Also compatible with nRF24L01+.

## How to connect

**Note** that for stability purposes, it is recommended to add 2 capacitors (electrolytic 10uF and ceramic 0.1uF) between VCC and GND of the nRF24L01 module. Alternatively, you can use a dedicated nRF24L01 Breakout Adapter board that includes the necessary capacitors and a voltage regulator which allows powering the nRF24L01 module from a 5V source, instead of 3.3V.

<img src="images/nrf24l01-adapter.png" width="300px">

In order to connect the nRF24L01 module to the STM32F103C8T6 board (blue pill), use the following connections.

<table>
  <tr>
    <td><b>STM32</b></td>
    <td><b>nRF24L01</b></td>
  </tr>
  <tr>
    <td>B0</td>
    <td>CSN</td>
  </tr>
  <tr>
    <td>B1</td>
    <td>CE</td>
  </tr>
  <tr>
    <td>3.3</td>
    <td>VCC</td>
  </tr>
  <tr>
    <td>G</td>
    <td>GND</td>
  </tr>
  <tr>
    <td>A5</td>
    <td>SCK</td>
  </tr>
  <tr>
    <td>A6</td>
    <td>MISO</td>
  </tr>
  <tr>
    <td>A7</td>
    <td>MOSI</td>
  </tr>
</table>

Note that SCK, MOSI and MISO pins are connected to the SPI1 pins. Pins from other SPI peripherals could also be used. CSN and CE pins are assigned to arbitrary GPIO pins and can be changed as needed.  
A similar connection scheme can be used for other STM32 boards.

<img src="images/connections.png" width="600px">

In the image above, a breakout adapter board is used.

## How to use

### Step 1

Initialize following (either using CubeMX or manually):
- The SPI peripheral
  - Mode: Full-Duplex Master
  - Prescaler: Any value that results in a baud rate higher than 2MB/s
  - First Bit: MSB First
- CSN, CE pins configured as outputs

### Step 2

Include the library to your project by copying the `stm32-nrf24l01` folder to `project_root/Middlewares`.

### Step 3

Link the library files to the project.  
In STM32CubeIDE navigate to `Project -> Properties -> C/C++ General -> Paths and Symbols`. Then, select the include tab, click *Add*, then click *Workspace* and select the `Middlewares/stm32-nrf24l01/Inc` folder.  
Then navigate to the *Source Location* tab and do the same for the `Middlewares/stm32-nrf24l01/Src` folder.

### Step 4

Include the library with `#include "nrf24l01.h"`. Done!

## Features

## Resources

- [nRF24L01+ Product Specification v1.0](https://docs.nordicsemi.com/bundle/nRF24L01P_PS_v1.0/resource/nRF24L01P_PS_v1.0.pdf)

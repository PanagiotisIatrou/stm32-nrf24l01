#pragma once

#include "hardware/spi.h"

#include "spi_interface.h"
#include "device_commands.h"

/**
 * Options for the Data rate.
 */
typedef enum {
    DATA_RATE_LOW,
    DATA_RATE_MEDIUM,
    DATA_RATE_HIGH,
} DataRate;

/**
 * Options for the Power level.
 */
typedef enum {
    POWER_LEVEL_LOW,
    POWER_LEVEL_MEDIUM,
    POWER_LEVEL_HIGH,
    POWER_LEVEL_VERY_HIGH,
} PowerLevel;

/**
 * Options for the length of the CRC code.
 */
typedef enum {
    CRC_BYTES_1,
    CRC_BYTES_2
} CrcBytes;

/**
 * Contains functionality for controlling a nRF24l01 device. Before configuring,
 * it is essential that the user has initialized the corresponding SPI peripheral.
 */
typedef struct nrf24l01 {
    spi_interface spi_handler;
    device_commands commands_handler;
} nrf24l01;

/**
 * Initializes a nRF24l01 device. Checks if the provided pins correspond to a valid
 * SPI peripheral and initializes them.
 * @param self The nrf24l01 struct to initialize.
 * @param address_prefix 4 bytes of the prefix all pipes will use.
 * @param mosi The SPI MOSI GPIO pin connected to the device.
 * @param miso The SPI MISO GPIO pin connected to the device.
 * @param sck The SPI SCK GPIO pin connected to the device.
 * @param csn The CSN GPIO pin connected to the device.
 * @param ce The CE GPIO pin connected to the device.
 * @return Whether initialization was successful or not.
 */
bool nrf24l01_init(nrf24l01 *self, uint8_t *address_prefix, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce);

/**
 * @param self The nrf24l01 struct to act upon.
 * @return True if the device is on, false if not.
 */
bool nrf24l01_get_power_state(nrf24l01 *self);

/**
 * Powers up the device.
 * @param self The nrf24l01 struct to act upon.
 */
void nrf24l01_power_up(nrf24l01 *self);

/**
 * Powers down the device.
 * @param self The nrf24l01 struct to act upon.
 */
void nrf24l01_power_down(nrf24l01 *self);

/**
 * Configures Pipe 0 of the nrf24l01 device for sending packets.
 * @param self The nrf24l01 struct to act upon.
 * @param address The address to associate Pipe 0 with.
 */
void nrf24l01_set_pipe0_write(nrf24l01 *self, uint8_t address);

/**
 * Configures the specified pipe of the nrf24l01 device for receiving packets.
 * @param self The nrf24l01 struct to act upon.
 * @param pipe The nrf24l01 pipe to act upon. Takes values 0-5. NOTE that if
 *             device is configured as both receiver and transmitter, Pipe 0
 *             should be avoided since it is the only one that can be used
 *             for transmission.
 * @param address The address to associate the given pipe with.
 */
void nrf24l01_set_pipe_read(nrf24l01 *self, uint pipe, uint8_t address);

/**
 * @param self The nrf24l01 struct to act upon.
 * @return The channel the device is currently set to. Valid range is [0, 125].
 */
uint8_t nrf24l01_get_channel(nrf24l01 *self);

/**
 * Sets the channel the device will operate on.
 * @param self The nrf24l01 struct to act upon.
 * @param channel The channel to set. Valid range is [0, 125].
 */
void nrf24l01_set_channel(nrf24l01 *self, uint8_t channel);

/**
 * @param self The nrf24l01 struct to act upon.
 * @return The current Data rate setting of the device. Can be one of
 *         DATA_RATE_LOW, DATA_RATE_MEDIUM or DATA_RATE_HIGH.
 */
DataRate nrf24l01_get_data_rate(nrf24l01 *self);

/**
 * Sets the Data rate of the device.
 * @param self The nrf24l01 struct to act upon.
 * @param data_rate The Data rate to set. Can be one of DATA_RATE_LOW,
 *                  DATA_RATE_MEDIUM or DATA_RATE_HIGH.
 */
void nrf24l01_set_data_rate(nrf24l01 *self, DataRate data_rate);

/**
 * @param self The nrf24l01 struct to act upon.
 * @return The current Power level setting of the device. Can be one of
 *         POWER_LEVEL_LOW, POWER_LEVEL_MEDIUM, POWER_LEVEL_HIGH or
 *         POWER_LEVEL_VERY_HIGH.
 */
PowerLevel nrf24l01_get_power_level(nrf24l01 *self);

/**
 * Sets the Power level of the device.
 * @param self The nrf24l01 struct to act upon.
 * @param power_level The Power level to set. Can be one of POWER_LEVEL_LOW,
 *                    POWER_LEVEL_MEDIUM, POWER_LEVEL_HIGH or POWER_LEVEL_VERY_HIGH.
 */
void nrf24l01_set_power_level(nrf24l01 *self, PowerLevel power_level);

/**
 * @param self The nrf24l01 struct to act upon.
 * @return The current retransmit delay setting of the device. Valid range is [0, 15].
 */
uint8_t nrf24l01_get_retransmit_delay(nrf24l01 *self);

/**
 * Sets the retransmit delay of the device.
 * @param self The nrf24l01 struct to act upon.
 * @param delay The retransmit delay to set. Valid range is [0, 15]. Each unit
 *              corresponds to 250us. For example, a value of 0 means 250us delay,
 *              a value of 1 means 500us delay, and so on, up to a value of 15
 *              which means 4000us (4ms) delay.
 */
void nrf24l01_set_retransmit_delay(nrf24l01 *self, uint8_t delay);

/**
 * @param self The nrf24l01 struct to act upon.
 * @return The current retransmit count setting of the device. Valid range is [0, 15].
 *         Also read the documentation of nrf24l01_set_retransmit_count for more info.
 */
uint8_t nrf24l01_get_retransmit_count(nrf24l01 *self);

/**
 * Sets the retransmit count of the device.
 * @param self The nrf24l01 struct to act upon.
 * @param count The retransmit count to set. In case of using send_packet functions,
 *              if resend_lost_packets is true, this is ignored. If it is false,
 *              the packet will be resent up to 'count' times before being dropped.
 *              Valid range is [0, 15].
 */
void nrf24l01_set_retransmit_count(nrf24l01 *self, uint8_t count);

/**
 * Sets the number of bytes used for the CRC code.
 * @param self The nrf24l01 struct to act upon.
 * @param count The number of bytes to use for the CRC code. Can be either
 *              CRC_BYTES_1 or CRC_BYTES_2.
 */
void set_crc_bytes(nrf24l01 *self, CrcBytes count);

/**
 * Sends multiple packets. Used when reliable transmission is required, sacrificing speed.
 * @param self The nrf24l01 struct to act upon.
 * @param value An array of pointers to the packets to send. The packets can have
 *              different lengths.
 * @param count The number of packets to send.
 * @param packet_lengths An array containing the lengths of each packet to send.
 *                       Each length must be in the range [1, 32].
 * @param resend_lost_packets If true, lost packets will be resent indefinitely
 *                            until acknowledged This results in all of the packets
 *                            sent reliably, though rarely, it can lead to duplicates.
 *                            If it is set to false, lost packets will be resent up
 *                            to the number set by nrf24l01_set_retransmit_count
 *                            before being dropped. Can rarely lead to lost packets,
 *                            but guarantees no duplicates.
 *                            NOTE: Both cases are affected by the signal quality.
 */
void nrf24l01_send_packets(nrf24l01 *self, uint8_t **value, int count, uint8_t *packet_lengths, bool resend_lost_packets);

/**
 * Sends multiple packets. Used when reliable transmission is not required, favoring speed.
 * Depending on the signal quality, some packets may be lost.
 * @param self The nrf24l01 struct to act upon.
 * @param value An array of pointers to the packets to send. The packets can have
 *              different lengths.
 * @param count The number of packets to send.
 * @param packet_lengths An array containing the lengths of each packet to send.
 *                       Each length must be in the range [1, 32].
 */
void nrf24l01_send_packets_no_ack(nrf24l01 *self, uint8_t **value, int count, uint8_t *packet_lengths);

/**
 * Receives a specified number of packets.
 * @param self The nrf24l01 struct to act upon.
 * @param packets An array of pointers to buffers where the received packets will be stored
 * @param count The number of packets to receive.
 * @param timeout The maximum time to wait for packets in milliseconds. If the timeout
 *                is reached before all packets are received, the function will return
 *                the number of packets that were actually received up to that point.
 *                This doesn't apply to the first packet, which will be waited for indefinitely.
 * @return The number of packets actually received. If no packets were lost, this will
 *         be equal to 'count'.
 */
int nrf24l01_receive_packets(nrf24l01 *self, uint8_t **packets, int count, uint timeout);

/**
 * Continuously receives packets, calling the provided callback function
 * for each received packet.
 * @param self The nrf24l01 struct to act upon.
 * @param value_callback A callback function that will be called for each received packet.
 *                       The function be of type void and should take two parameters:
 *                       a pointer to the received packet and the length of the packet.
 */
void nrf24l01_receive_packets_inf(nrf24l01 *self, void (*value_callback)(uint8_t* packet, uint8_t packet_length));

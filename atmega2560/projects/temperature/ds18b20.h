#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

/* 
 * DS18B20 1-Wire Temperature Sensor Driver for ATmega2560 + FreeRTOS
 * 
 * Pin Configuration:
 * - Connect DS18B20 data pin to a GPIO pin (configure below)
 * - Use 4.7kΩ pull-up resistor between data and VCC
 * - Power: 3.0V to 5.5V
 */

/* Configure the 1-Wire data pin here */
#define DS18B20_PORT    PORTD   /* Port for 1-Wire data */
#define DS18B20_PIN     PD2     /* Pin for 1-Wire data (use any available pin) */
#define DS18B20_DDR     DDRD    /* Data direction register */
#define DS18B20_PIN_REG PIND    /* PIN register for reading pin state */

/* DS18B20 Error Codes */
#define DS18B20_SUCCESS                       1
#define DS18B20_ERROR_NONE                    1
#define DS18B20_ERROR_NO_DEVICE               -1  /* No device present on bus */
#define DS18B20_ERROR_CONVERSION_START        -2  /* Failed to start temperature conversion */
#define DS18B20_ERROR_TIMEOUT                 -3  /* Conversion timed out */
#define DS18B20_ERROR_CRC                     -4  /* CRC verification failed */
#define DS18B20_ERROR_READ_SCRATCHPAD         -5  /* Failed to read scratchpad */
#define DS18B20_ERROR_READ_TEMP_DATA          -6  /* Failed to read temperature data */
#define DS18B20_ERROR_WRITE_SCRATCHPAD        -7  /* Failed to write scratchpad */
#define DS18B20_ERROR_COPY_SCRATCHPAD         -8  /* Failed to copy scratchpad to EEPROM */

/* DS18B20 Commands */
#define DS18B20_CMD_SKIP_ROM              0xCC
#define DS18B20_CMD_READ_SCRATCHPAD       0xBE
#define DS18B20_CMD_WRITE_SCRATCHPAD      0x4E
#define DS18B20_CMD_CONVERT_TEMP          0x44
#define DS18B20_CMD_COPY_SCRATCHPAD       0x48
#define DS18B20_CMD_RECALL_EEPROM         0xB8
#define DS18B20_CMD_READ_POWER_SUPPLY     0xB4

/* DS18B20 Resolution settings */
#define DS18B20_RESOLUTION_9_BIT   0x1F  /* 9-bit, ~187.5ms conversion */
#define DS18B20_RESOLUTION_10_BIT  0x3F  /* 10-bit, ~375ms conversion */
#define DS18B20_RESOLUTION_11_BIT  0x5F  /* 11-bit, ~750ms conversion */
#define DS18B20_RESOLUTION_12_BIT  0x7F  /* 12-bit, ~1500ms conversion */


void delay_usec(int);

/* Function Prototypes */

/**
 * @brief Initialize the DS18B20 driver
 * @note Call this before using any other DS18B20 functions
 */
void ds18b20_init(void);

/**
 * @brief Reset the 1-Wire bus and check for presence
 * @return DS18B20_SUCCESS if a device is present, DS18B20_ERROR_NO_DEVICE if no device detected
 */
int8_t ds18b20_reset(void);

/**
 * @brief Write a byte to the 1-Wire bus
 * @param byte The byte to write
 */
void ds18b20_write_byte(uint8_t byte);

/**
 * @brief Read a byte from the 1-Wire bus
 * @return The read byte
 */
uint8_t ds18b20_read_byte(void);

/**
 * @brief Skip ROM command (for single device on bus)
 * @return DS18B20_SUCCESS if successful, DS18B20_ERROR_NO_DEVICE if error
 */
int8_t ds18b20_skip_rom(void);

/**
 * @brief Start temperature conversion
 * @return DS18B20_SUCCESS if successful, negative error code on failure
 */
int8_t ds18b20_start_conversion(void);

/**
 * @brief Read temperature from scratchpad
 * @param temperature Pointer to store the temperature value
 * @return DS18B20_SUCCESS if successful, negative error code on failure
 */
int8_t ds18b20_read_temperature(float *temperature);

/**
 * @brief Read temperature with blocking wait
 * @param temperature Pointer to store the temperature value
 * @param conversion_time_ms Maximum time to wait for conversion (typically 750-1500ms)
 * @return DS18B20_SUCCESS if successful, negative error code on failure
 */
int8_t ds18b20_read_temperature_blocking(float *temperature, uint16_t conversion_time_ms);

/**
 * @brief Configure the resolution (9-12 bits)
 * @param resolution One of DS18B20_RESOLUTION_* constants
 * @return DS18B20_SUCCESS if successful, negative error code on failure
 */
int8_t ds18b20_set_resolution(uint8_t resolution);

/**
 * @brief Get the resolution in milliseconds for delay calculation
 * @param resolution One of DS18B20_RESOLUTION_* constants
 * @return Conversion time in milliseconds
 */
uint16_t ds18b20_get_conversion_time(uint8_t resolution);

#endif /* DS18B20_H */

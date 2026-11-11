/*
 * ds18b20.c
 * DS18B20 1-Wire Temperature Sensor Driver for ATmega2560 + FreeRTOS
 * 
 * This driver uses precise timing loops for 1-Wire protocol.
 * It is FreeRTOS-compatible - uses no blocking FreeRTOS calls in timing-critical sections.
 */

#include "ds18b20.h"

/* 
 * 1-Wire Timing Constants (in microseconds) for 16MHz ATmega2560
 * These values are calibrated for accurate 1-Wire communication
 */

/* Reset and presence detection: master sends low for 480μs */
#define DS18B20_RST_LOW_US        580

/* Master receives presence pulse (device pulls low for 60-240μs) */
/* Sample window: 15-60μs after releasing bus per 1-Wire spec */
#define DS18B20_RST_SAMPLE_US     75

/* Reset recovery time before next operation */
#define DS18B20_RST_RECOVERY_US   480

/* Write 0: master sends low for 60-120μs */
#define DS18B20_WRITE_0_LOW_US    60
#define DS18B20_WRITE_0_SAMPLE_US 10

/* Write 1: master sends low for ~15μs, then releases */
#define DS18B20_WRITE_1_LOW_US    15
#define DS18B20_WRITE_1_SAMPLE_US 10

/* Read slot: master releases bus, samples after ~15μs */
#define DS18B20_READ_SLOT_US      60
#define DS18B20_READ_SAMPLE_US    15

/* 
 * This provides precise microsecond delays without blocking FreeRTOS scheduler
 */
static void ds18b20_delay_us(uint16_t us)
{
    delay_usec(us-1);
}

/**
 * Set pin as output
 */
static inline void ds18b20_set_output(void)
{
    DS18B20_DDR |= (1 << DS18B20_PIN);
}

/**
 * Set pin as input (high-Z)
 * Note: Pull-up is disabled during presence detection to allow device to pull line low
 * Pull-up is re-enabled after sampling for better noise immunity
 */
static inline void ds18b20_set_input(void)
{
    DS18B20_DDR &= ~(1 << DS18B20_PIN);
    /* Disable pull-up for clean presence pulse detection */
//    DS18B20_PORT &= ~(1 << DS18B20_PIN);
}

/**
 * Set pin as input with pull-up enabled
 */
static inline void ds18b20_set_input_pullup(void)
{
    DS18B20_DDR &= ~(1 << DS18B20_PIN);
    /* Enable pull-up for defined logic level */
   // DS18B20_PORT |= (1 << DS18B20_PIN);
}

/**
 * Set pin high (pull-up)
 */
static inline void ds18b20_set_high(void)
{
    DS18B20_PORT |= (1 << DS18B20_PIN);
}

/**
 * Set pin low
 */
static inline void ds18b20_set_low(void)
{
    DS18B20_PORT &= ~(1 << DS18B20_PIN);
}

/**
 * Read pin state
 * Use PINx register to read actual pin state, not PORTx which reads output latch
 */
static inline uint8_t ds18b20_read_pin(void)
{
    /* Reading PINx register gives actual pin state even when configured as input */
//    return (DS18B20_PIN_REG & (1 << (DS18B20_PIN & 0x07))) ? 1 : 0;
    return (DS18B20_PIN_REG & (1 << DS18B20_PIN)) ? 1 : 0;
}

/* ============================================================================
 * 1-Wire Protocol Functions
 * ========================================================================== */

void ds18b20_init(void)
{
    /* Initialize the data pin as output, low */
    ds18b20_set_output();
    ds18b20_set_low();
}

int8_t ds18b20_reset(void)
{
    uint8_t presence;
    
    /* Set pin as output and pull low for reset pulse */
    ds18b20_set_output();
    ds18b20_set_low();
    ds18b20_delay_us(DS18B20_RST_LOW_US);
    
    /* Release bus and set as input (no pull-up) to sample presence pulse */
    ds18b20_set_input();
    ds18b20_delay_us(DS18B20_RST_SAMPLE_US);
    
    /* Sample the presence pulse */
    presence = ds18b20_read_pin();
    
    /* Wait for reset recovery time */
    ds18b20_delay_us(DS18B20_RST_RECOVERY_US);
    
    /* Re-enable pull-up for noise immunity after reset sequence */
//    ds18b20_set_input_pullup();
    
    /* Return negative error code for no device, positive for success */
    return presence == 0 ? DS18B20_SUCCESS : DS18B20_ERROR_NO_DEVICE;
}

void ds18b20_write_byte(uint8_t byte)
{
    uint8_t i;
    uint8_t bit;
    
    for(i = 0; i < 8; i++)
    {
        bit = byte & 0x01;
        
        if(bit)
        {
            /* Write 1: pull low for short time, then release */
            ds18b20_set_output();
            ds18b20_set_low();
            ds18b20_delay_us(DS18B20_WRITE_1_LOW_US);
            ds18b20_set_input();
            ds18b20_delay_us(DS18B20_WRITE_1_SAMPLE_US);
        }
        else
        {
            /* Write 0: pull low for full slot time */
            ds18b20_set_output();
            ds18b20_set_low();
            ds18b20_delay_us(DS18B20_WRITE_0_LOW_US);
            ds18b20_delay_us(DS18B20_WRITE_0_SAMPLE_US);
            ds18b20_set_input();
        }
        
        /* Slot recovery time */
        ds18b20_delay_us(2);
        
        byte >>= 1;
    }
}

uint8_t ds18b20_read_byte(void)
{
    uint8_t byte = 0;
    uint8_t i;
    
    for(i = 0; i < 8; i++)
    {
        /* Start read slot: pull low briefly */
        ds18b20_set_output();
        ds18b20_set_low();
        ds18b20_delay_us(6);
        
        /* Release bus and sample */
        ds18b20_set_input();
        ds18b20_delay_us(DS18B20_READ_SAMPLE_US);
        
        /* Read the bit */
        if(ds18b20_read_pin())
        {
            byte |= (1 << i);
        }
        
        /* Complete the time slot */
        ds18b20_delay_us(DS18B20_READ_SLOT_US - DS18B20_READ_SAMPLE_US - 6);
    }
    
    return byte;
}

/* ============================================================================
 * DS18B20 Specific Functions
 * ========================================================================== */

int8_t ds18b20_skip_rom(void)
{
    int8_t reset_result = ds18b20_reset();
    if(reset_result != DS18B20_SUCCESS)
    {
        return reset_result;  /* Propagate the error code */
    }
    
    ds18b20_write_byte(DS18B20_CMD_SKIP_ROM);
    
    return DS18B20_SUCCESS;
}

int8_t ds18b20_start_conversion(void)
{
    int8_t skip_result = ds18b20_skip_rom();
    if(skip_result != DS18B20_SUCCESS)
    {
        return skip_result;  /* Propagate the error code */
    }
    
    ds18b20_write_byte(DS18B20_CMD_CONVERT_TEMP);
    
    return DS18B20_SUCCESS;
}

/* CRC calculation helper */
static uint8_t ds18b20_calc_crc(uint8_t crc, uint8_t byte)
{
    uint8_t i;
    
    for(i = 0; i < 8; i++)
    {
        uint8_t bit = (byte ^ crc) & 0x01;
        crc >>= 1;
        
        if(bit)
        {
            crc ^= 0x8C;
        }
        
        byte >>= 1;
    }
    
    return crc;
}

int8_t ds18b20_read_temperature(float *temperature)
{
    int8_t skip_result;
    uint8_t scratchpad[9];
    uint8_t i;
    uint8_t crc;
    int16_t temp_raw;
    
    skip_result = ds18b20_skip_rom();
    if(skip_result != DS18B20_SUCCESS)
    {
        return skip_result;
    }
    
    ds18b20_write_byte(DS18B20_CMD_READ_SCRATCHPAD);
    
    /* Read scratchpad (9 bytes) */
    for(i = 0; i < 9; i++)
    {
        scratchpad[i] = ds18b20_read_byte();
    }
    
    /* Calculate and verify CRC */
    crc = 0;
    for(i = 0; i < 8; i++)
    {
        crc = ds18b20_calc_crc(crc, scratchpad[i]);
    }
    
    if(crc != scratchpad[8])
    {
        return DS18B20_ERROR_CRC;  /* CRC error */
    }
    
    /* Combine temperature bytes (little-endian) */
    temp_raw = ((int16_t)scratchpad[1] << 8) | scratchpad[0];
    
    /* 
     * Convert to temperature
     * For 12-bit mode: resolution is 0.0625°C per count
     * temp_raw is signed 16-bit value
     */
    *temperature = (float)temp_raw / 16.0f;
    
    return DS18B20_SUCCESS;
}

int8_t ds18b20_read_temperature_blocking(float *temperature, uint16_t conversion_time_ms)
{
    int8_t result;
    uint32_t start_time;
    uint32_t elapsed;
    
    /* Start conversion */
    result = ds18b20_start_conversion();
    if(result != DS18B20_SUCCESS)
    {
        return result;  /* Propagate the error code from start_conversion */
    }
    
    /* Wait for conversion to complete */
    start_time = xTaskGetTickCount();
    
    while(1)
    {
        /* Check if conversion is done by reading the scratchpad */
        result = ds18b20_reset();
        if(result != DS18B20_SUCCESS)
        {
            return result;  /* Device not present */
        }
        
        result = ds18b20_skip_rom();
        if(result != DS18B20_SUCCESS)
        {
            return result;  /* Skip ROM failed */
        }
        
        ds18b20_write_byte(DS18B20_CMD_READ_SCRATCHPAD);
        
        /* Read temperature bytes only (first 2 bytes) */
        int16_t temp_raw = ((uint16_t)ds18b20_read_byte()) | 
                          ((uint16_t)ds18b20_read_byte() << 8);
        
        /* If temp is 0x0550 (default after reset), conversion is not complete */
        if(temp_raw != 0x0550)
        {
            /* Verify with CRC */
            uint8_t crc = 0;
            uint8_t i;
            uint8_t scratchpad[9];
            
            /* Need to restart for full read */
            result = ds18b20_reset();
            if(result != DS18B20_SUCCESS)
            {
                return result;  /* Device not present */
            }
            
            result = ds18b20_skip_rom();
            if(result != DS18B20_SUCCESS)
            {
                return result;  /* Skip ROM failed */
            }
            
            ds18b20_write_byte(DS18B20_CMD_READ_SCRATCHPAD);
            
            for(i = 0; i < 9; i++)
            {
                scratchpad[i] = ds18b20_read_byte();
            }
            
            /* Verify CRC */
            crc = 0;
            for(i = 0; i < 8; i++)
            {
                crc = ds18b20_calc_crc(crc, scratchpad[i]);
            }
            
            if(crc == scratchpad[8])
            {
                temp_raw = ((int16_t)scratchpad[1] << 8) | scratchpad[0];
                *temperature = (float)temp_raw / 16.0f;
                return DS18B20_SUCCESS;
            }
            else
            {
                return DS18B20_ERROR_CRC;  /* CRC verification failed */
            }
        }
        
        /* Check timeout */
        elapsed = xTaskGetTickCount() - start_time;
        if(elapsed > pdMS_TO_TICKS(conversion_time_ms))
        {
            return DS18B20_ERROR_TIMEOUT;  /* Conversion timed out */
        }
        
        /* Small delay before next check */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int8_t ds18b20_set_resolution(uint8_t resolution)
{
    int8_t result;
    uint8_t config;
    
    /* Read scratchpad to get current configuration */
    result = ds18b20_skip_rom();
    if(result != DS18B20_SUCCESS)
    {
        return result;  /* Propagate the error code */
    }
    
    ds18b20_write_byte(DS18B20_CMD_READ_SCRATCHPAD);
    
    /* Read temperature bytes and configuration */
    ds18b20_read_byte();  /* TH */
    ds18b20_read_byte();  /* TL */
    config = ds18b20_read_byte();  /* Configuration */
    
    /* Update resolution bits (bits 5 and 6) */
    config &= ~(0x60);  /* Clear RS1, RS0 */
    config |= (resolution & 0x60);
    
    /* Write back to scratchpad */
    result = ds18b20_reset();
    if(result != DS18B20_SUCCESS)
    {
        return result;  /* Device not present */
    }
    
    result = ds18b20_skip_rom();
    if(result != DS18B20_SUCCESS)
    {
        return result;  /* Skip ROM failed */
    }
    
    ds18b20_write_byte(DS18B20_CMD_WRITE_SCRATCHPAD);
    
    ds18b20_write_byte(0);  /* TH - alarm high */
    ds18b20_write_byte(0);  /* TL - alarm low */
    ds18b20_write_byte(config);
    
    /* Copy to EEPROM */
    result = ds18b20_reset();
    if(result != DS18B20_SUCCESS)
    {
        return result;  /* Device not present */
    }
    
    result = ds18b20_skip_rom();
    if(result != DS18B20_SUCCESS)
    {
        return result;  /* Skip ROM failed */
    }
    
    ds18b20_write_byte(DS18B20_CMD_COPY_SCRATCHPAD);
    
    /* Wait for copy to complete */
    ds18b20_delay_us(10000);  /* 10ms max copy time */
    
    return DS18B20_SUCCESS;
}

uint16_t ds18b20_get_conversion_time(uint8_t resolution)
{
    switch(resolution & 0x60)
    {
        case 0x00:  /* 9-bit */
            return 188;
        case 0x20:  /* 10-bit */
            return 375;
        case 0x40:  /* 11-bit */
            return 750;
        case 0x60:  /* 12-bit */
        default:
            return 1500;
    }
}

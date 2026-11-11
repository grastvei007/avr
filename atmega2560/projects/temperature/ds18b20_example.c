/*
 * ds18b20_example.c
 * Example usage of DS18B20 driver with FreeRTOS
 */

#include "ds18b20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"

QueueHandle_t tempQueue;

/* Example Task: Read temperature periodically */
void vTaskReadTemperature(void *pvParameters)
{
    float temperature;
    uint16_t conversion_time;
    
    /* Initialize DS18B20 */
    ds18b20_init();
    
    /* Configure resolution (12-bit for best accuracy) */
    ds18b20_set_resolution(DS18B20_RESOLUTION_12_BIT);
    
    /* Get conversion time for 12-bit resolution */
    conversion_time = ds18b20_get_conversion_time(DS18B20_RESOLUTION_12_BIT);
    
    for(;;)
    {
        /* Read temperature with blocking wait */
        if(ds18b20_read_temperature_blocking(&temperature, conversion_time))
        {
            /* Send to queue for processing */
            if(tempQueue != NULL)
            {
                xQueueSend(tempQueue, &temperature, pdMS_TO_TICKS(100));
            }
            
            /* Send via USART for debugging */
            vUART0_SendString("<temp:");
            char tempBuffer[20];
            /* Convert float to string */
            sprintf(tempBuffer, "%.2f>", temperature);
            vUART0_SendString(tempBuffer);
            vUART0_SendString("\n");
        }
        else
        {
            vUART0_SendString("<temp:error>\n");
        }
        
        /* Wait 1 second between readings */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Example: How to use in main.c
 *
 * Add these lines to your main() before vTaskStartScheduler():
 *
 * // Create queue for temperature data
 * tempQueue = xQueueCreate(10, sizeof(float));
 * if(tempQueue == NULL)
 * {
 *     // Handle queue creation failure
 * }
 *
 * // Create temperature reading task
 * xTaskCreate(vTaskReadTemperature, "temp", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
 */
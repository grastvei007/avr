/*
 * freertos.c
 * adc test
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "croutine.h"

#include "usart.h"
#include "message.h"
#include "ds18b20.h"

#define RX_NAME_SIZE 10
#define TEMP_TASK_STACK_SIZE 256

QueueHandle_t txQueue;
QueueHandle_t rxQueue;
QueueHandle_t nameValueQueue;
QueueHandle_t tempQueue;


TaskHandle_t sendDeviceNameTaskHandle = NULL;

volatile uint8_t rx_name[RX_NAME_SIZE];

enum valueTypes
{
    eFloat = 1,
    eInt = 2,
    eBool = 3,
    eString = 4,
    eInt_8 = 5
};

typedef enum rx_states_
{
    idle,
    name,
    value
}rx_states;
rx_states state = idle;


void vTaskSendDeviceName(void *pvParameters);
void vTaskRx(void *pvParameters);
void vTaskNameValueRecieved(void *pvParameters);
void vTaskReadTemperature(void *pvParameters);


typedef union{
    float f;
    uint8_t bytes[4];
}float_bytes;

typedef union{
    int i;
    int8_t bytes[4];
}int_bytes;

int main( void )
{
    txQueue = xQueueCreate(64, sizeof(uint8_t));
    rxQueue = xQueueCreate(64, sizeof(uint8_t));
    nameValueQueue = xQueueCreate(10, RX_NAME_SIZE);
    tempQueue = xQueueCreate(10, sizeof(float));

    usart0_init();

	DDRB = 0xff;

    xTaskCreate(vTaskSendDeviceName, "name", configMINIMAL_STACK_SIZE, NULL, 1, &sendDeviceNameTaskHandle);
    xTaskCreate(vTaskRx, "rx", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskNameValueRecieved, "received", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();
	
	for(;;);
}


void vTaskSendDeviceName(void *pvParameters)
{
    char *deviceName = "<device:test>";

    for(;;)
    {
        vUART0_SendString(deviceName);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }    
}

void vTaskRx(void *pvParameters)
{
    uint8_t data;
    uint8_t index_name;
    for(;;)
    {
        if(xQueueReceive(rxQueue, &data, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            PORTB = ~PORTB;
            switch(state)
            {
                case idle:
                    switch(data)
                    {
                        case '<':
                            for(int i = 0; i<RX_NAME_SIZE; ++i)
                                rx_name[i] = 0;
                            index_name = 0;
                            state = name;
                            break;
                    }
                    break;
                case name:
                    switch(data)
                    {
                        case ':':
                            state = value;
                            break;
                        default:
                            if(index_name < RX_NAME_SIZE)
                                rx_name[index_name++] = data;
                    }
                    break;
                case value:
                    switch(data)
                    {
                        case '>':
                            // send rx_name to another task
                            xQueueSend(nameValueQueue, (const void *)rx_name, 0);
                            state = idle;
                            break;
                    }
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vTaskNameValueRecieved(void *pvParameters)
{
    uint8_t name[RX_NAME_SIZE];
    for(;;)
    {
        if(xQueueReceive(nameValueQueue, &name, portMAX_DELAY) == pdTRUE)
        {
            // have receve an <ok:> message name is then ok.
            // This means the host has comfirmed it received the device name.
            if(name[0] == 'o' && name[1] == 'k')
            {
                // Suspend the device name task
                if(sendDeviceNameTaskHandle != NULL)
                {
                    vTaskSuspend(sendDeviceNameTaskHandle);
                }
                
                // Create the temperature reading task with error checking
                BaseType_t taskResult;
                taskResult = xTaskCreate(vTaskReadTemperature, "temp", TEMP_TASK_STACK_SIZE, NULL, 1, NULL);
                
                // Verify task was created successfully
                if(taskResult == pdPASS)
                {
                    // Task created successfully - you should see output now
                    // Indicate success via LED or USART
                    vUART0_SendString("Temp task created\r\n");
                }
                else
                {
                    // Task creation failed - likely heap memory issue
                    vUART0_SendString("Temp task FAILED\r\n");
                }
            }
        }
    }
}


/* Example Task: Read temperature periodically */
void vTaskReadTemperature(void *pvParameters)
{
    float temperature;
    uint16_t conversion_time;
    
    // Diagnostic: confirm task is running
    vUART0_SendString("Temp task STARTED\r\n");
    
    /* Initialize DS18B20 */
    ds18b20_init();
    
    /* Configure resolution (12-bit for best accuracy) */
    ds18b20_set_resolution(DS18B20_RESOLUTION_12_BIT);
    
    /* Get conversion time for 12-bit resolution */
    conversion_time = ds18b20_get_conversion_time(DS18B20_RESOLUTION_12_BIT);
    
    for(;;)
    {
        /* Read temperature with blocking wait */
        int read = ds18b20_read_temperature_blocking(&temperature, conversion_time);
        if(read == DS18B20_SUCCESS)
        {
            /* Send to queue for processing */
            if(tempQueue != NULL)
            {
                xQueueSend(tempQueue, &temperature, pdMS_TO_TICKS(100));
            }
            
            char tempBuffer[12] = "<temp:1____>";
            float_bytes fb;
            fb.f = temperature;
            tempBuffer[7] = fb.bytes[0];
            tempBuffer[8] = fb.bytes[1];
            tempBuffer[9] = fb.bytes[2];
            tempBuffer[10] = fb.bytes[3];

            vUART0_SendString_n(tempBuffer, 12);
        }
        else
        {
            int8_t errorBuffer[10] = "<error:5_>";
            errorBuffer[8] = read;
            vUART0_SendString_n(errorBuffer, 10);
        }
        
        /* Wait 10 second between readings */
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

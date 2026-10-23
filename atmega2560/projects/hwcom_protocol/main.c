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

#define RX_NAME_SIZE 10

QueueHandle_t txQueue;
QueueHandle_t rxQueue;
QueueHandle_t adcQueue;
QueueHandle_t nameValueQueue;

TaskHandle_t sendDeviceNameTaskHandle = NULL;

volatile uint8_t rx_name[RX_NAME_SIZE];

typedef enum rx_states_
{
    idle,
    name,
    value
}rx_states;
rx_states state = idle;


void taskAnalogRead(void *pvParameters);
void vTaskSendDeviceName(void *pvParameters);
void vTaskRx(void *pvParameters);
void vTaskNameValueRecieved(void *pvParameters);



ISR(ADC_vect)
{
    uint16_t adcValue = ADC;
    xQueueSendFromISR(adcQueue, &adcValue, NULL);
}


void setupADC()
{
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (1 << REFS0);

    ADCSRA |= (1 << ADIE);

    ADCSRB = 0;
    ADCSRA |= (1 << ADATE);
}


int main( void )
{
    txQueue = xQueueCreate(64, sizeof(uint8_t));
    rxQueue = xQueueCreate(64, sizeof(uint8_t));
    adcQueue = xQueueCreate(1, sizeof(uint16_t));
    nameValueQueue = xQueueCreate(10, RX_NAME_SIZE);

    setupADC();
    usart0_init();

	DDRB = 0xff;

    xTaskCreate(vTaskSendDeviceName, "name", configMINIMAL_STACK_SIZE, NULL, 1, &sendDeviceNameTaskHandle);
    xTaskCreate(vTaskRx, "rx", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskNameValueRecieved, "received", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();
	
	for(;;);
}


void taskAnalogRead(void *pvParameters)
{
    uint16_t adcValue;
    char buffer[10];
    for(;;)
    {
        ADCSRA |= (1 << ADSC);
        
        if(xQueueReceive(adcQueue, &adcValue, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            int value = adcValue;
            itoa(value, buffer, 10);
            vUART0_SendString(buffer);
            vUART0_SendString("\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
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
                vTaskSuspend(sendDeviceNameTaskHandle);
                // start sending analog values to the host
                xTaskCreate(taskAnalogRead, "analogTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
            }
        }
    }
}

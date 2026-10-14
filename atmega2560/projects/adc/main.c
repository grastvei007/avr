/*
 * freertos.c
 * adc test
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "croutine.h"

#include "usart.h"

#define task1_TASK_PRIORITY					( tskIDLE_PRIORITY + 1 )

#define USART0_BAUD 9600
#define USART0_UBBR ((F_CPU / 16UL / USART0_BAUD) - 1)

QueueHandle_t txQueue;
QueueHandle_t adcQueue;


void taskAnalogRead(void *pvParameters);



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
    adcQueue = xQueueCreate(1, sizeof(uint16_t));

    setupADC();
    usart0_init();

	DDRB = 0xff;

    xTaskCreate(taskAnalogRead, "analogTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

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
            float value = adcValue / 5.0;
            ftoa(value, buffer, 10);
            vUART0_SendString(buffer);
            vUART0_SendString("\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

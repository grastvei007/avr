/*
 * w 
 * freertos.c
 *
 * Created: 2026/9/30
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "croutine.h"

#define task1_TASK_PRIORITY					( tskIDLE_PRIORITY + 1 )

#define USART0_BAUD 9600
#define USART0_UBBR ((F_CPU / 16UL / USART0_BAUD) - 1)

QueueHandle_t usartQueue;


void usart0_init();
void vUART0_SendString(const char *pcString);



ISR(USART0_UDRE_vect)
{
    uint8_t data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(xQueueReceiveFromISR(usartQueue, &data, &xHigherPriorityTaskWoken) == pdPASS)
    {
        UDR0 = data;
    }
    else
    {
        UCSR0B &= ~(1 << UDRIE0);
    }
}


void vTask1(void *pvParameters) 
{
    uint32_t value;
	for(;;){
       /* xQueueReceive(fromADCqueue, &value, portMAX_DELAY);

        if(value > 500)
            PORTB = 0xff;
        else
            PORTB = 0x00;*/
		PORTB = ~PORTB;
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

void usartTask(void *pvParameters)
{
    const char *msg = "Hello!";
    //uint32_t data = 'A';
    
    // Make sure DDRB is set for LED (assuming pin 13 on PORTB)
    DDRB |= (1 << 7);  // Set PB7 as output for debug LED
    
    for(;;)
    {
        PORTB ^= (1 << 7);  // Toggle debug LED
        vUART0_SendString(msg);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

int main( void )
{

    usartQueue = xQueueCreate(64, sizeof(uint8_t));

//	DDRB = 0xff;

    usart0_init();

//	xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE, NULL, task1_TASK_PRIORITY, NULL);
    xTaskCreate(usartTask, "usart", configMINIMAL_STACK_SIZE, NULL, task1_TASK_PRIORITY, NULL);

	vTaskStartScheduler();
	
	for(;;);
}


void usart0_init()
{
    UBRR0H = (uint8_t)(USART0_UBBR >> 8);
    UBRR0L = (uint8_t)(USART0_UBBR);

    UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // Enable TX and RX

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit data, no parity, 1 stop bit

    // Don't enable any TX interrupts here - they will be enabled by vUART0_SendString
    sei();
}

void vUART0_SendString(const char *pcString)
{
    uint8_t i;
    uint8_t len = strlen(pcString);
    
    // Send first character directly to UDR0
    if(len > 0)
    {
        // Wait for UDR to be empty before writing first char
        while(!(UCSR0A & (1 << UDRE0)));
        UDR0 = pcString[0];
        
        // Queue the remaining characters
        for(i = 1; i < len; ++i)
        {
            if (xQueueSend(usartQueue, &pcString[i], pdMS_TO_TICKS(100)) != pdPASS) {
                break;
            }
        }
    }
    
    // Enable interrupt for subsequent characters
    UCSR0B |= (1 << UDRIE0);
}

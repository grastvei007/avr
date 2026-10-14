#include "usart.h"

#include <avr/interrupt.h>
#include <string.h>

ISR(USART0_UDRE_vect)
{
    uint8_t data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(xQueueReceiveFromISR(txQueue, &data, &xHigherPriorityTaskWoken) == pdPASS)
    {
        UDR0 = data;
    }
    else
    {
        UCSR0B &= ~(1 << UDRIE0);
    }
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
            if (xQueueSend(txQueue, &pcString[i], pdMS_TO_TICKS(100)) != pdPASS) {
                break;
            }
        }
    }
    
    // Enable interrupt for subsequent characters
    UCSR0B |= (1 << UDRIE0);
}

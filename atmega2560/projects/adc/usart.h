#include <avr/io.h>
#include "FreeRTOS.h"
#include "queue.h"

#define USART0_BAUD 9600
#define USART0_UBBR ((F_CPU / 16UL / USART0_BAUD) - 1)

extern QueueHandle_t txQueue;

void usart0_init();
void vUART0_SendString(const char *pcString);

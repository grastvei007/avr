#ifndef USART_H
#define USART_H

#define F_CPU 12000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 12UL))) - 1)


void USART_init(void);
void USART_send(char data);
void USART_putstring(char* StringPtr);
void USART_putMessage(char *aMsg, int size);


#endif


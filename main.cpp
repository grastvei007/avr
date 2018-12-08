#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "message.h"
#include "messagehandler.h"
#include <stdlib.h>

#define F_CPU 12000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 12UL))) - 1)


void USART_init(void);
void USART_send( unsigned char data);
void USART_putstring(char* StringPtr);
void USART_putMessage(unsigned char *aMsg, int size);

MessageHandler handler;

int main()
{
	USART_init();
	
	sei();
	float val = 10.2;
	Message msg;
	msg.add("test", val);
	msg.finnish();

	while(true)
	{
		unsigned char *text;
		int size = msg.getMessage(text);
		USART_putMessage(text, size);
		free(text);  
//		USART_putstring("Hello");
		_delay_ms(5000);
	}

    return 0;
}


void USART_init(){
    UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);


    UCSR0B =(1 << RXEN0) | (1 << TXEN0);


 //   UCSR0C = (3<<UCSZ00);
//	UCSRB |= (1 << RCXIE);
//  UCSR0B |= (1 << RXCIE0);
	UCSR0C |= (1<<UCSZ00 | (1 << UCSZ01));
}



void USART_send( unsigned char data){

 while(!(UCSR0A & (1<<UDRE0)));
 UDR0 = data;

}

void USART_putstring(char* StringPtr){

	while(*StringPtr != 0x00){
		 USART_send(*StringPtr);
 		StringPtr++;
	}

}

void USART_putMessage(unsigned char *aMsg, int size)
{
	for(int i=0; i<size; ++i)
	{
		USART_send(aMsg[i]);
	}
}

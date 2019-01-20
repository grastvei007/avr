#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "message.h"
#include "messagehandler.h"
#include "messagetranslationsenter.h"
#include <stdlib.h>

#define F_CPU 12000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 12UL))) - 1)

void initTimer();

void USART_init(void);
void USART_send(char data);
void USART_putstring(char* StringPtr);
void USART_putMessage(char *aMsg, int size);

MessageHandler messageHandler;
MessageTranslationSenter mts;

volatile bool lock;


void messageHandlerCallback(Message *&msg)
{
	USART_putstring("translate");
	mts.translateMessage(msg);
	msg->destroy();
	free(msg);
}


void deviceNameRequest()
{
	USART_putstring("name");
	Message m;
	m.init();
	m.add("deviceName", "Atmega328Uno");
	m.finnish();
	char *msg;
	int size = m.getMessage(msg);
	m.destroy();
	USART_putMessage(msg, size);
	free(msg);
}

int main()
{
	initTimer();	

	USART_init();
	messageHandler.init(64);
	messageHandler.setCallback(messageHandlerCallback);

	mts.init();
	mts.setDeviceNameFunc(deviceNameRequest);
	mts.setDebugFunc(USART_putstring);
	lock = false;
	sei();
	float val = 10.2;
	Message msg;
	msg.add("test", val);
	msg.finnish();

	while(true)
	{
/*		unsigned char *text;
		int size = msg.getMessage(text);
		USART_putMessage(text, size);
		free(text);  
		_delay_ms(5000);*/
	}

    return 0;
}


ISR(TIMER1_COMPA_vect)
{
	if(lock)
		return;
	lock = true;
	// run loop
	messageHandler.run();
	lock = false;
}



void initTimer()
{
	OCR1A = F_CPU / 256 / 10; // 10 Hz
	TCCR1B |= 1 << WGM12;
	TCCR1B |= 1 << CS12;
	TIMSK1 |= 1 << OCIE1A;
}

void USART_init(){
    UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);

	 UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
 	//enable reception and RC complete interrupt
	 UCSR0B |= (1<<TXEN0) | (1<<RXEN0)|(1<<RXCIE0);
}


ISR(USART_RX_vect)
{
//	while(!(UCSR0A&(1<<RXC0))){};
	char c = UDR0;
//	USART_send(c);
	messageHandler.insertChar(c);
}


void USART_send(char data){

 while(!(UCSR0A & (1<<UDRE0)));
 UDR0 = data;

}

void USART_putstring(char* StringPtr){

	while(*StringPtr != 0x00){
		 USART_send(*StringPtr);
 		StringPtr++;
	}

}

void USART_putMessage(char *aMsg, int size)
{
	for(int i=0; i<size; ++i)
	{
		USART_send(aMsg[i]);
	}
}

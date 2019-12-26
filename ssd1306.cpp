#include "ssd1306.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "i2c.h"
#include "usart.h"
#include "font.h"


//namespace oled
//{
int8_t _da = 0x78; // display adress

void sendCommand(int8_t cmd)
{
	int r = i2c_start();
	if(r == 1)
		USART_putstring("ok");
	int8_t control = 0x00;
	r = i2c_sendAddres(_da);
	if(r == 0)
		USART_putstring("Device not found");
	i2c_write(control);
	i2c_write(cmd);
	i2c_stop();
}


/** \brief Init the display.

**/
void init_display()
{
//	 DDRC|=(1<<DDC4)|(1<<DDC5);
//	 PORTC|=(1<<PC2);
//	 _delay_ms(100);
	
	
	// Init sequence for 128x64 OLED module
	sendCommand(SSD1306_DISPLAYOFF);                    // 0xAE

	sendCommand(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	sendCommand(0x80);                 // the suggested ratio 0x80
	
	sendCommand(SSD1306_SETMULTIPLEX);                  // 0xA8
	sendCommand(0x3F);
	
	sendCommand(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	sendCommand(0x0);                                   // no offset
	
	sendCommand(SSD1306_SETSTARTLINE);// | 0x0);        // line #0
	
	sendCommand(SSD1306_CHARGEPUMP);                    // 0x8D
	sendCommand(0x14);  // using internal VCC
	
	//ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
	//ssd1306_command(0x00);          // 0x00 horizontal addressing
	
	sendCommand(SSD1306_SEGREMAP | 0x1); // rotate screen 180
	
	sendCommand(SSD1306_COMSCANDEC); // rotate screen 180
	
	sendCommand(SSD1306_SETCOMPINS);                    // 0xDA
	sendCommand(0x12);
	
	sendCommand(SSD1306_SETCONTRAST);                   // 0x81
	sendCommand(0xCF);
	
	sendCommand(SSD1306_SETPRECHARGE);                  // 0xd9
	sendCommand(0xF1);
	
	sendCommand(SSD1306_SETVCOMDETECT);                 // 0xDB
	sendCommand(0x40);
	
	sendCommand(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	
	sendCommand(SSD1306_NORMALDISPLAY);                 // 0xA6
	
	sendCommand(SSD1306_DISPLAYON);                     //switch on OLED
}


void sendByte(unsigned char byte)
{
	i2c_start();
	i2c_sendAddres(_da);
	i2c_write(0x40);
	i2c_write(byte);
	i2c_stop();
}


void setCursor(unsigned char x, unsigned char y)
{
    sendCommand(0xb0+x);                //set page address
    sendCommand(0x00+(8*y&0x0f));       //set low col address
    sendCommand(0x10+((8*y>>4)&0x0f));  //set high col address
}

void clear_display()
{
	unsigned char i = 0;
	unsigned char k = 0;

	for(i=0; i<8; ++i)
	{
		setCursor(i, 0);
		for(k=0; k<128; ++k)
		{
			sendByte(0);
		}
	}
}


void writeString(char *string)
{
	unsigned char i=0;
	while(*string)
	{
		for(i=0;i<8;i++)
		{
			sendByte(pgm_read_byte(myFont[*string-0x20]+i));
		}
		string++;
	}
}

//};//end namespace

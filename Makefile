## General Flags
PROJECT = june
MCU = atmega328p
TARGET = libJune.a
CC = avr-g++
F_CPU	:=  1200000LL
F_SCL	:=  100000L

## Options common to compile, link and assembly rules
COMMON = -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DF_SCL=$(F_SCL)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -MD -MP -MT -MF

## Objects that must be built in order to link
OBJECTS := adc.o pwm.o message.o messagehandler.o messagetranslationsenter.o usart.o portb.o tag.o i2c.o ssd1306.o motorencoder.o \
            circularbuffer.o

## Build
all: $(TARGET)

## Compile
adc.o: adc.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

pwm.o: pwm.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

message.o: message.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

messagehandler.o: messagehandler.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

messagetranslationsenter.o: messagetranslationsenter.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

portb.o: portb.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

usart.o: usart.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

tag.o: tag.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

i2c.o: i2c.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

ssd1306.o: ssd1306.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

motorencoder.o: motorencoder.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

circularbuffer.o: circularbuffer.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

## Archive into a library file (.a)
$(TARGET): $(OBJECTS)
	avr-ar -r $(TARGET) $(OBJECTS)

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(TARGET)

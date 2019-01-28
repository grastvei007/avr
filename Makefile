## General Flags
PROJECT = june
MCU = atmega328p
TARGET = libJune.a
CC = avr-g++
F_CPU	:=  1200000LL

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -DF_CPU=$(F_CPU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os  -funsigned-bitfields -fpack-struct -fshort-enums
#CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F) 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Objects that must be built in order to link
OBJECTS := adc.o pwm.o message.o messagehandler.o messagetranslationsenter.o

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

## Archive into a library file (.a)
$(TARGET): $(OBJECTS)
	avr-ar -rs $(TARGET) $(OBJECTS)

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(TARGET) #dep/*

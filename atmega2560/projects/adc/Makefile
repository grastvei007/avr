TARGET = adc_rtos

include ../../Makefile.incl

# Add current directory to VPATH so make can find source files
vpath %.c .

# Append additional project source files to the existing SRC variable
SRC += usart.c

# Re-expand OBJ to include the new source files
OBJ = $(patsubst %.c, %.o, $(SRC))

# Define our own build target that depends on the expanded OBJ (includes usart.o)
# The build target from Makefile.incl uses the original OBJ which doesn't include usart.o
build: $(OBJ) main.c
	@echo $(MSG_BUILD) $(TARGET)
	@$(CC) $(CFLAGS) main.c $(OBJ) -o $(TARGET).elf $(LDFLAGS)
	avr-size -B --mcu=$(MCU) $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

# Debug target to show variables
printvars:
	@echo "SRC = $(SRC)"
	@echo "OBJ = $(OBJ)"



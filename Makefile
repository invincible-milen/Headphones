 
# Library location
STM_COMMON = /home/milen/embedded/STM32F4-Discovery_FW_V1.1.0

# ST-Link location
STLINK = /opt/stlink-1.8.0

# Project name
PROJ_NAME = screentesting

# Source files
SRCS = main.c adc.c spi.c ST7735.c
SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s
SRCS += syscalls.c

# Compiler settings
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Compiler flags
CFLAGS = -g -O2 -Wall -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -I.

# Include directories
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include

# Object files
OBJS = $(SRCS:.c=.o)

# Set search path for source files
#vpath %.c $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src \

# Build commands
all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x8000000
	$(STLINK)/st-flash reset

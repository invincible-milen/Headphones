#ifndef SPI_H
#define SPI_H

#include "stm32f4xx.h"

#define spi1 0x01
#define spi2 0x02
#define spi3 0x03

//functions
void initSPI(uint8_t spiType);
void transmitSPI(uint8_t spiType ,uint8_t data);
#endif

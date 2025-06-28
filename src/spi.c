 
/*
 * SPI1
 * SCK  -   B3 / A5
 * MOSI -   B5 / A7
 * MISO -   B4 / A6
 *
 * SPI2
 * SCK  -   B13 / B10
 * MOSI -   B15 / C3
 * MISO -   B14 / C2
 *
 * SPI3
 * SCK  -   B3 / C10
 * MOSI -   B5 / C12
 * MISO -   B4 / C11
 */

/*********************************
 * NOTES
 * software slave select is used - set ssm bit in spi_cr1 register
 *
 * prior to changing cpol and cpha spi must be disabled by resetting spe bit
 * master and slave must have same timing mode (cpol and cpha)
 * the idle state of sck pin must be set to the one defined in cpol
 * data frame format (8 or 16 bits) must be selected in the dff bit in SPI_CR1 register
 * cpol - clock polarity - sets the ideal state of the line - bits in
 * cpha - clock phase - if set then the second edge is used else first edge is used
 *
 * data can be shifted lsb or msb first by changing the value of the LSBFIRST bit in SPI_CR1 register
 *
 * Master mode
 * BR[2:0] - serial clock baude rate in SPI_CR1 register
 * set CPOL and CPHA
 * set DFF
 * select LSBFIRST (not required in Texas Instrument mode)
 * set NSS pin
 * set FRF pin
 * MSTR and SPE pin must be set (they remain set only if NSS pin is connected to a high-level signal)
 *
 * Transmit sequence
 * write a byte of data into the Tx Buffer
 * TXE flag is set when data moves from Tx Buffer to the shift register, interupt is generated if TXEIE bit is set in SPI_CR2 register
 *
 * half duplex mode
 * BIDIMODE - sets wether the line is input or output when only one wire is there for data transfer. BIMODE in combination with RXONLY bit decides wether the spi is receive only or transmit only
 *
 * disabling spi
 * wait until TXE is 1 and RXE is 1
 * wait until bsy is 0
 * then make SPE 0
 * *******************************/

//include header
#include "stm32f4xx.h"
#include "spi.h"


//enable peripheral clock function
//spi1 apb2
//spi2 and spi3 apb1
void initClocks(uint8_t spiType){
    switch(spiType){
        case spi1:
            //a5, a6, a7
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            break;
        case spi2:
            //b13, b14, b15
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
            break;
        case spi3:
            //b3, b4, b5
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
            RCC->APB2ENR |= RCC_APB1ENR_SPI3EN;
            break;
    }
}

//set pins function
void setPins(uint8_t spiType){
    switch(spiType){
        case spi1:
            GPIOA->MODER &= ~((GPIO_MODER_MODER5) | (GPIO_MODER_MODER6) | (GPIO_MODER_MODER7));
            GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);

            GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);

            GPIOA->OSPEEDR &= ~((GPIO_OSPEEDER_OSPEEDR5) | (GPIO_OSPEEDER_OSPEEDR6) | (GPIO_OSPEEDER_OSPEEDR7));
            GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1);

            GPIOA->PUPDR &= ~((GPIO_PUPDR_PUPDR5) | (GPIO_PUPDR_PUPDR6) | (GPIO_PUPDR_PUPDR7));
            break;
        case spi2:
            GPIOB->MODER &= (~(GPIO_MODER_MODER13) & ~(GPIO_MODER_MODER14) & ~(GPIO_MODER_MODER15));
            GPIOB->MODER |= (GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1);

            GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);

            GPIOB->OSPEEDR &= (~(GPIO_OSPEEDER_OSPEEDR13) & ~(GPIO_OSPEEDER_OSPEEDR14) & ~(GPIO_OSPEEDER_OSPEEDR15));
            GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR15_1);

            GPIOB->PUPDR &= (~(GPIO_PUPDR_PUPDR13) & ~(GPIO_PUPDR_PUPDR14) & ~(GPIO_PUPDR_PUPDR15));
            break;
        case spi3:
            GPIOB->MODER &= (~(GPIO_MODER_MODER3) & ~(GPIO_MODER_MODER4) & ~(GPIO_MODER_MODER5));
            GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1);

            GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_3 | GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_5);

            GPIOB->OSPEEDR &= (~(GPIO_OSPEEDER_OSPEEDR3) & ~(GPIO_OSPEEDER_OSPEEDR4) & ~(GPIO_OSPEEDER_OSPEEDR5));
            GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR5_1);

            GPIOB->PUPDR &= (~(GPIO_PUPDR_PUPDR3) & ~(GPIO_PUPDR_PUPDR4) & ~(GPIO_PUPDR_PUPDR5));
            break;
    }
}

void setAlternatefunction(uint8_t spiType){
    switch(spiType){
        case spi1:
            GPIOA->AFR[0] &= ~(0xF << (7*4) | 0xF << (6*4) | 0xF << (5*4));
            GPIOA->AFR[0] |= ((0x5 << (7*4)) | (0x5 << (6*4)) | (0x5 << (5*4)));
            break;
        case spi2:
            GPIOB->AFR[1] &= ~(0xF << (7*4) | 0xF << (6*4) | 0xF << (5*4));
            GPIOB->AFR[1] |= ((0x05 << (7*4)) | (0x05 << (6*4)) | (0x05 << (5*4)));
            break;
        case spi3:
            GPIOB->AFR[0] &= ~(0xF << (5*4) | 0xF << (4*4) | 0xF << (3*4));
            GPIOB->AFR[0] |= ((0x06 << (5*4)) | (0x06 << (4*4)) | (0x06 << (3*4)));
            break;
    }
}


void setDFF8(uint8_t spiType){
    switch(spiType){
        case spi1:
            SPI1->CR1 &= ~SPI_CR1_DFF;
            break;
        case spi2:
            SPI2->CR1 &= ~SPI_CR1_DFF;
            break;
        case spi3:
            SPI3->CR1 &= ~SPI_CR1_DFF;
            break;
    }
}

void setDFF16(uint8_t spiType){
    switch(spiType){
        case spi1:
            SPI1->CR1 |= SPI_CR1_DFF;
            break;
        case spi2:
            SPI2->CR1 |= SPI_CR1_DFF;
            break;
        case spi3:
            SPI3->CR1 |= SPI_CR1_DFF;
            break;
    }
}

void configureSPI(uint8_t spiType){
    switch(spiType){
        case spi1:
            SPI1->CR1 &= ~(SPI_CR1_BIDIMODE |
                        SPI_CR1_LSBFIRST |
                        SPI_CR1_CPOL |
                        SPI_CR1_CPHA|
                           SPI_CR1_BR_0 |
                        SPI_CR1_BR_1 |
                        SPI_CR1_BR_2);
            SPI1->CR1 |= SPI_CR1_SSM |
                        SPI_CR1_SSI |
                        SPI_CR1_MSTR;

            SPI1->CR2 &= ~(0x10); //motorolla

            SPI1->CR1 |= SPI_CR1_SPE;


            break;
        case spi2:
            SPI2->CR1 &= ~(SPI_CR1_BIDIMODE |
                        SPI_CR1_LSBFIRST |
                        SPI_CR1_CPOL |
                        SPI_CR1_CPHA);
            SPI2->CR1 |= SPI_CR1_SSM |
                        SPI_CR1_SSI |
                        SPI_CR1_MSTR;

            SPI2->CR2 &= ~(0x10); //motorolla

            SPI2->CR1 |= SPI_CR1_SPE;

            break;
        case spi3:
            SPI3->CR1 &= ~(SPI_CR1_BIDIMODE |
                        SPI_CR1_LSBFIRST |
                        SPI_CR1_CPOL |
                        SPI_CR1_CPHA);
            SPI3->CR1 |= SPI_CR1_SSM |
                        SPI_CR1_SSI |
                        SPI_CR1_MSTR;

            SPI3->CR2 &= ~(0x10); //motorolla

            SPI3->CR1 |= SPI_CR1_SPE;


            break;
    }
    setDFF8(spiType);
}
//have set cpha to 0 and cpol to 1 ... i.e. idle high and reads at first trigger

//init function
void initSPI(uint8_t spiType){
    initClocks(spiType);
    setPins(spiType);
    setAlternatefunction(spiType);
    configureSPI(spiType);
}

void transmitSPI(uint8_t spiType ,uint8_t data){
    switch(spiType){
        case spi1:
            while(!(SPI1->SR & SPI_SR_TXE));
            SPI1->DR = data;
            while (SPI1->SR & SPI_SR_BSY);
            break;
        case spi2:
            while(!(SPI2->SR & SPI_SR_TXE));
            SPI2->DR = data;
            while (SPI2->SR & SPI_SR_BSY);
            break;
        case spi3:
            while(!(SPI3->SR & SPI_SR_TXE));
            SPI3->DR = data;
            while (SPI3->SR & SPI_SR_BSY);
            break;
    }
}

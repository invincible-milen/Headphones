#include "stm32f4xx.h"
#include "spi.h"
#include "ST7735.h"

uint16_t framebuffer[132][164];

//delay
void delay(uint32_t n){
    for(uint32_t i=0; i<n*1000; i++){
        __NOP();
    }
}

//DC - A0
//Reset - A1
void initGPIO(){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
    GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0);

    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_0 | GPIO_OTYPER_OT_1);

    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1);

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1);
}

void setDCHigh(){
    GPIOA->BSRRL = GPIO_BSRR_BS_0;
    delay(10);
}
void setDCLow(){
    GPIOA->BSRRH = (GPIO_BSRR_BR_0 >> 16);
    delay(10);
}

void setResetHigh(){
    GPIOA->BSRRL = GPIO_BSRR_BS_1;
    delay(10);
}
void setResetLow(){
    GPIOA->BSRRH = (GPIO_BSRR_BR_1 >> 16);
    delay(10);
}


/* SWRESET
 * SLPOUT
 * NORON
 * MADCTL
 * COLMOD
 */
void initST7735(){
    initGPIO();

    setResetLow();
    delay(50);
    setResetHigh();
    delay(50);

    setDCLow();
    transmitSPI(spi1 ,SWRESET);
    delay(150);

    transmitSPI(spi1 ,SLPOUT);
    delay(500);

    transmitSPI(spi1 ,NORON);
    delay(10);

    transmitSPI(spi1 ,MADCTL);
    delay(5);
    setDCHigh();
    transmitSPI(spi1 ,0xC0);
    delay(10);

    setDCLow();
    transmitSPI(spi1 ,COLMOD);
    delay(5);
    setDCHigh();
    transmitSPI(spi1 ,0x05);
    delay(10);
    setDCLow();
}

void selectRectangle(uint16_t XS, uint16_t YS, uint16_t XE, uint16_t YE){
    setDCLow();
    transmitSPI(spi1, CASET);
    delay(5);
    setDCHigh();
    transmitSPI(spi1, (XS>>8));
    transmitSPI(spi1, (XS&0xFF));
    transmitSPI(spi1, (XE>>8));
    transmitSPI(spi1, (XE&0xFF));
    delay(10);
    setDCLow();

    setDCLow();
    transmitSPI(spi1, RASET);
    delay(5);
    setDCHigh();
    transmitSPI(spi1, (YS>>8));
    transmitSPI(spi1, (YS&0xFF));
    transmitSPI(spi1, (YE>>8));
    transmitSPI(spi1, (YE&0xFF));
    delay(10);
    setDCLow();

    displayOn();
    delay(10);

}


void displayInvertOn(){
    setDCLow();
    transmitSPI(spi1, INVON);
    delay(5);

}

void displayInvertOff(){
    setDCLow();
    transmitSPI(spi1, INVOFF);
    delay(5);

}

void displayOn(){
    transmitSPI(spi1, DISPON);
    delay(120);
}

void fillRectangle(uint16_t XS, uint16_t YS, uint16_t XE, uint16_t YE, uint16_t colour){
    selectRectangle(XS, YS, XE, YE);

    setDCLow();
    transmitSPI(spi1, RAMWR);
    delay(5);
    setDCHigh();

    int n= (XE-XS+1) * (YE-YS+1);
    for(int i=0; i<n; i++){
        transmitSPI(spi1, (colour>>8));
        transmitSPI(spi1, (colour&0xFF));
    }
    delay(10);
    setDCLow();
}

void drawPixel(uint16_t X, uint16_t Y, uint16_t colour){
    fillRectangle(X, X, Y, Y, colour);
}

void fillScreen(uint16_t colour){
    fillRectangle(0, 0, 127, 159, colour);
}

void flushFrameBuffer(void){
    selectRectangle(0, 0, 127, 159);

    setDCLow();
    transmitSPI(spi1, RAMWR);
    delay(5);
    setDCHigh();

    for(int y=2; y<162; y++){
        for(int x=2; x<130; x++){
            uint16_t colour = framebuffer[x][y];
            transmitSPI(spi1, (colour>>8));
            transmitSPI(spi1, (colour&0xFF));
        }
    }
    delay(10);
    setDCLow();

    delay(10);
    setDCLow();
}

void scrollUp(void){
    for(int y=2; y<162; y++){
        for(int x=2; x<130; x++){
            framebuffer[x][y] = framebuffer[x][y+1];
        }
    }
}

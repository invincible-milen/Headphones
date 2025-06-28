#ifndef ST7735_H
#define ST7735_H


#include "spi.h"
#include "stm32f4xx.h"

#define SWRESET 0x01
#define SLPOUT  0x11
#define NORON   0x13
#define MADCTL  0x36
#define COLMOD  0x3A
#define CASET   0x2A
#define RASET   0x2B
#define RAMWR   0x2C
#define DISPON  0x29
#define INVON   0x21
#define INVOFF  0x20
#define FRMCTR1 0xB1
#define FRMCTR2 0xB2
#define FRMCTR3 0xB3
#define INVCTR  0xB4
#define PWCTR1  0xC0
#define PWCTR2  0xC1
#define PWCTR3  0xC2
#define PWCTR4  0xC3
#define PWCTR5  0xC4
#define VMCTR1  0xC5
#define GMCTRP1 0xE0
#define GMCTRN1 0xE1

// colours
#define AZURE   0x041F
#define BLACK   0x0000
#define BLUE    0x001F
#define BROWN   0x9260
#define CYAN    0x07FF
#define GRAY    0x8410
#define GREEN   0x07E0
#define MAGENTA 0xF81F
#define ORANGE  0xFBE0
#define PINK    0xFDF9
#define PURPLE  0x8010
#define RED     0xF800
#define VIOLET  0x781F
#define WHITE   0xFFFF
#define YELLOW  0xFFE0

extern uint16_t framebuffer [132][164]; //128+4 160+4

void initST7735();
void displayOn();
void displayInvertOn();
void displayInvertOff();
void fillRectangle(uint16_t XS, uint16_t YS, uint16_t XE, uint16_t YE, uint16_t colour);
void drawPixel(uint16_t X, uint16_t Y, uint16_t colour);
void fillScreen(uint16_t colour);

void flushFrameBuffer(void);
void scrollUp(void);


#endif

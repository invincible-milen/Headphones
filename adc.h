#ifndef ADC_H
#define ADC_H

#include "stm32f4xx.h"

void initADC(void);
void startADC(void);
uint16_t readADC(void);
#endif

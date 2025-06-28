 
#include "stm32f4xx.h"
#include "adc.h"

//A2 analog input
void initADC(void){
    //turn on clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    //set gpio to analog
    GPIOA->MODER |= GPIO_MODER_MODER2;
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);
    //set the data resolution
    ADC1->CR1 &= ~(ADC_CR1_RES); //12bit
    //select the channel
    ADC1->SQR1 &= ~(ADC_SQR1_L); //only one channel
    ADC1->SQR3 &= ~(ADC_SQR3_SQ1); //A2 as the input
    ADC1->SQR3 |= ADC_SQR3_SQ1_1;
    //channel sampling time
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP2; //3 cycles
    //align
    ADC1->CR2 &= ~(ADC_CR2_ALIGN);
    //enable the adc module
    ADC1->CR2 |= ADC_CR2_ADON;
    //continuous conversion
    ADC1->CR2 |= ADC_CR2_CONT;
}

void startADC(void){
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t readADC(void){
    while(!(ADC1->SR & ADC_SR_EOC));
    return (uint16_t) ADC1->DR;
}

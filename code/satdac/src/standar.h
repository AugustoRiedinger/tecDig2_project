#ifndef functions_H
#define functions_H

/*------------------------------------------------------------------------------
LIBRERIAS:
------------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stdio.h"
#include "math.h"
#include "misc.h"

/*------------------------------------------------------------------------------
DECLARACION DE ESTRUCTURAS:
------------------------------------------------------------------------------*/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;
EXTI_InitTypeDef        EXTI_InitStructure;
ADC_InitTypeDef         ADC_InitStructure;
ADC_CommonInitTypeDef   ADC_CommonInitStructure;
DMA_InitTypeDef         DMA_InitStructure;
NVIC_InitTypeDef        NVIC_InitStructure;
GPIO_InitTypeDef        GPIO_InitStructure;

/*------------------------------------------------------------------------------
DECLARACION FUNCIONES DE STANDAR.C :
------------------------------------------------------------------------------*/
void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin);
uint32_t FIND_CLOCK(GPIO_TypeDef *Port);
uint8_t FIND_PINSOURCE(uint32_t Pin);

#endif

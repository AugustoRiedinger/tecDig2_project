/********************************************************************************
  * @file    functions.h
  * @author  A. Riedinger & G. Stang.
  * @version 0.1
  * @date    09-11-21.
  * @brief   Definicion de librerias y valores para main.c
********************************************************************************/

/* Definicion del header:*/
#ifndef functions_H
#define functions_H

/*------------------------------------------------------------------------------
LIBRERIAS:
------------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stdio.h"
#include "stm32f4xx_tim.h"
#include "math.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_dma.h"
#include "string.h"

/*------------------------------------------------------------------------------
DECLARACION DE ESTRUCTURAS:
------------------------------------------------------------------------------*/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef  		TIM_OCInitStructure;
EXTI_InitTypeDef   		EXTI_InitStructure;
ADC_InitTypeDef 		ADC_InitStructure;
ADC_CommonInitTypeDef 	ADC_CommonInitStructure;
DMA_InitTypeDef 		DMA_InitStructure;
NVIC_InitTypeDef 		NVIC_InitStructure;
GPIO_InitTypeDef        GPIO_InitStructure;

/*------------------------------------------------------------------------------
DECLARACION FUNCIONES DE FUNCTIONS.C :
------------------------------------------------------------------------------*/
void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin);
void INIT_ADC1DMA(uint16_t* adcArray, uint32_t bufferSize);
void INIT_TIM3(uint32_t Freq);
void INIT_TIM2(uint32_t Freq);
void INIT_USART_RX_TX(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, uint32_t);
void INIT_ADC(GPIO_TypeDef* Port, uint16_t Pin);
int32_t READ_ADC(GPIO_TypeDef* Port, uint16_t Pin);

/*Cierre del header:*/
#endif

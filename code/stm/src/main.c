#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stdio.h"
#include "misc.h"

#include "stm32f4xx_rcc.h"

#include "stm32f4xx_adc.h"

#include "stm32f4xx_tim.h"

#include "math.h"

GPIO_InitTypeDef    GPIO_InitStructure;

NVIC_InitTypeDef    NVIC_InitStructure;

EXTI_InitTypeDef   EXTI_InitStructure;

ADC_InitTypeDef         ADC_InitStructure;
ADC_CommonInitTypeDef   ADC_CommonInitStructure;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

DMA_InitTypeDef     DMA_InitStructure;

int main(void)
{
  while (1)
  {
  }
}

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

#define _Servo      GPIOA
#define _servo      GPIO_Pin_3
#define __servo     GPIO_PinSource3

#define freqPWM 10e3

#define dutyCyclePWM 50

void INIT_TIM3(uint32_t freq);

void INIT_TIMPWM(void);

void INIT_PWM(void);

void INIT_SERVO(void);

void MOVE_SERVO(void);

int main(void){

    SystemInit();

    INIT_TIMPWM();

    INIT_PWM();

    INIT_SERVO();

  while (1)
  {
  }
}

void INIT_TIMPWM(void){
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_BaseStruct.TIM_Prescaler = 0;

    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_BaseStruct.TIM_Period = SystemCoreClock / freqPWM - 1;

    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
    TIM_Cmd(TIM4, ENABLE);
}

void INIT_PWM(void){
    TIM_OCInitTypeDef TIM_OCStruct;

    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

    uint32_t TIM_Period = SystemCoreClock / freqPWM - 1;

    TIM_OCStruct.TIM_Pulse = ((TIM_Period + 1) * dutyCyclePWM) / 100 - 1;

    TIM_OC1Init(TIM4, &TIM_OCStruct);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

void INIT_SERVO(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_PinAFConfig(_Servo, __servo, GPIO_AF_TIM4);

    GPIO_InitStruct.GPIO_Pin = _servo;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}

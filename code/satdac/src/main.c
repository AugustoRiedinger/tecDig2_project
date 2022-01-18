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

#define  TLCD_INIT_PAUSE        100000
#define  TLCD_PAUSE             50000
#define  TLCD_CLK_PAUSE         1000
#define  TLCD_MAXX              16
#define  TLCD_MAXY              2
#define  MaxDigCount            4095
#define  MaxMiliVoltRef         3320
#define  Res32Bit               32
#define  DAC_DHR12R2_ADDRESS    0x40007414
#define  TLCD_CMD_INIT_DISPLAY  0x28
#define  TLCD_CMD_ENTRY_MODE    0x06
#define  TLCD_CMD_DISP_M0       0x08
#define  TLCD_CMD_DISP_M1       0x0C
#define  TLCD_CMD_DISP_M2       0x0E
#define  TLCD_CMD_DISP_M3       0x0F
#define  TLCD_CMD_CLEAR         0x01

typedef enum
{
  TLCD_RS = 0,  // RS-Pin
  TLCD_E  = 1,  // E-Pin
  TLCD_D4 = 2,  // DB4-Pin
  TLCD_D5 = 3,  // DB5-Pin
  TLCD_D6 = 4,  // DB6-Pin
  TLCD_D7 = 5   // DB7-Pin
}TLCD_NAME_t;

#define  TLCD_ANZ   6

typedef enum {
  TLCD_OFF = 0,
  TLCD_ON,
  TLCD_CURSOR,
  TLCD_BLINK
}TLCD_MODE_t;

typedef struct {
  TLCD_NAME_t TLCD_NAME;
  GPIO_TypeDef* TLCD_PORT;
  const uint16_t TLCD_PIN;
  const uint32_t TLCD_CLK;
  BitAction TLCD_INIT;
}LCD_2X16_t;

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

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

#define _servo  GPIOG
#define servo   GPIO_Pin_3

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

void INIT_TIM3(uint32_t freq);

void MOVE_SERVO(void);

int main(void)
{
  while (1)
  {
  }
}

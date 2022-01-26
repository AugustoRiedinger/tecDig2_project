/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stdio.h"
#include "misc.h"

/*Control del clock:*/
#include "stm32f4xx_rcc.h"

/*Control de pines analogicos*/
#include "stm32f4xx_adc.h"

/*Control de timers:*/
#include "stm32f4xx_tim.h"

/*Matematicas:*/
#include "math.h"

/*----------------------------------------------------------------*/
/*DEFINICIONES:                                                   */
/*----------------------------------------------------------------*/

/* * * * * * * * * * * * * ESTRUCTURAS * * * * * * * * * * * * */
/*General:*/
GPIO_InitTypeDef        GPIO_InitStructure;
NVIC_InitTypeDef        NVIC_InitStructure;
EXTI_InitTypeDef        EXTI_InitStructure;

/*ADC:*/
ADC_InitTypeDef         ADC_InitStructure;
ADC_CommonInitTypeDef   ADC_CommonInitStructure;

/*TIMERS*/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/*DMA*/
DMA_InitTypeDef         DMA_InitStructure;

/* * * * * * * * * * * * * CONSTANTES * * * * * * * * * * * * */
/* - - - - HARDWARE - - - -*/
/*Servomotor:*/
#define _Servo      GPIOA
#define _servo      GPIO_Pin_3
#define __servo     GPIO_PinSource3

/* - - - - PARAMS. - - - -*/
/*Frecuencia PWM:*/
#define freqPWM 10e3

/*Ciclo de trabajo PWM:*/
#define dutyCyclePWM 50

/* * * * * * * * * * * * * FUNCIONES * * * * * * * * * * * * */
/*Inicializacion TIM3:*/
void INIT_TIM3(uint32_t freq);

/*Inicializacion TIM4:*/
void INIT_TIMPWM(void);

/*Inicializacion PWM:*/
void INIT_PWM(void);

/*Inicializacion servo:*/
void INIT_SERVO(void);

/*Mover servo:*/
void MOVE_SERVO(void);

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
    /*Inicio del sistema:*/
    SystemInit();

    /*Inicializacion de timers:*/
    INIT_TIMPWM();
    INIT_PWM();

    /*Servomotor:*/
    INIT_SERVO();

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
  }
}

/*----------------------------------------------------------------*/
/*FUNCIONES LOCALES:                                              */
/*----------------------------------------------------------------*/

void INIT_TIMPWM(void){
    /*Declaracion estructura particular:*/
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    /*Activacion del clock:*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /*Seteo del preescaler en 0 para obtener la maxima frecuencia:*/
    TIM_BaseStruct.TIM_Prescaler = 0;

    /*Seteo del conteo hacia arriba:*/
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;

    /*Calculo periodo:*/
    TIM_BaseStruct.TIM_Period = SystemCoreClock / freqPWM - 1;

    /*Seteo de parametros restantes y carga a la estructura:*/
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);

    /*Inicio del conteo:*/
    TIM_Cmd(TIM4, ENABLE);
}

void INIT_PWM(void){
    TIM_OCInitTypeDef TIM_OCStruct;

    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

    uint32_t Period= SystemCoreClock / freqPWM - 1;

    TIM_OCStruct.TIM_Pulse = ((Period + 1) * dutyCyclePWM) / 100 - 1;

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

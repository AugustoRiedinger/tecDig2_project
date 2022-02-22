/* * * * * * * * * * *         LIBRERIAS       * * * * * * * * * * * * * * */
#include "_timers.h"

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
/*Inicializacion del TIM3:*/
void INIT_TIM3(uint32_t Freq)
{

    /*Habilitacion del clock para el TIM3:*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /*Habilitacion de la interrupcion por agotamiento de cuenta del TIM3:*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*Actualización de los valores del TIM3:*/
    SystemCoreClockUpdate();
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM3, DISABLE);

    /*Definicion de la base de tiempo:*/
    uint32_t TimeBase = 200e3;

    /*Computar el valor del preescaler en base a la base de tiempo:*/
    uint16_t PrescalerValue = 0;
    PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TimeBase) - 1;

    /*Configuracion del tiempo de trabajo en base a la frecuencia:*/
    TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /*Habilitacion de la interrupcion:*/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /*Habilitacion del contador:*/
    TIM_Cmd(TIM3, ENABLE);
}

/*Inicializacion TIM2:*/
void INIT_TIM2(uint32_t Freq)
{
  /*Habilitacion de la senal de reloj para el periferico:*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /*Computacion del valor del preescaler:*/
  uint32_t TimeBase = 200e3;
  uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TimeBase) - 1;

  /*Configuracion del tiempo de interrupcion:*/
  TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  /*Cargar valores a la estructura del TIM4:*/
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /*Seleccion de TIM3 TRGO:*/
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  TIM_Cmd(TIM2, ENABLE);
}

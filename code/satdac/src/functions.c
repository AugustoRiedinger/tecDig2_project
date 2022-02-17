#include "functions.h"

/*------------------------------------------------------------------------------
DECLARACION DE FUNCIONES INTERNAS:
------------------------------------------------------------------------------*/
/*Encontrar el CLOCK de un determinado pin:*/
uint32_t FIND_CLOCK(GPIO_TypeDef *Port);
uint8_t FIND_PINSOURCE(uint32_t Pin);

/*Control del ADC:*/
ADC_TypeDef *FIND_ADC_TYPE(GPIO_TypeDef *Port, uint32_t Pin);
uint32_t FIND_RCC_APB(ADC_TypeDef *ADCX);
uint8_t FIND_CHANNEL(GPIO_TypeDef *Port, uint32_t Pin);

/*****************************************************************************
INIT_DO

        * @author	A. Riedinger.
        * @brief	Inicialiiza salidas digitales.
        * @returns	void
        * @param
                - Port	Puerto del pin a inicializar. Ej: GPIOX.
                - Clock Clock del pin a inicializar. Ej: RCC_AHB1Periph_GPIOX.
                - Pin	Pin a inicializar. Ej: GPIO_Pin_X
        * @ej
                - INIT_DO(GPIOX, RCC_AHB1Periph_GPIOX, GPIO_Pin_X);
******************************************************************************/
void INIT_DO(GPIO_TypeDef *Port, uint32_t Pin) {
  // Estructura de configuracion
  GPIO_InitTypeDef GPIO_InitStructure;

  // Habilitacion de la senal de reloj para el periferico:
  uint32_t Clock;
  Clock = FIND_CLOCK(Port);
  RCC_AHB1PeriphClockCmd(Clock, ENABLE);

  // Se configura el pin como entrada (GPI0_MODE_IN):
  GPIO_InitStructure.GPIO_Pin = Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  // Se aplica la configuracion definida anteriormente al puerto:
  GPIO_Init(Port, &GPIO_InitStructure);
}

/*****************************************************************************
INIT_ADC

        * @author	Catedra UTN-BHI TDII / A. Riedinger.
        * @brief	Inicializa una entrada analogica como ADC.
        * @returns	void
        * @param
                - Port		Puerto del ADC a inicializar. Ej: GPIOX.
                - Pin		Pin del ADC a inicializar. Ej: GPIO_Pin_X
        * @ej
                - INIT_ADC(GPIOX, GPIO_Pin_X);
******************************************************************************/
void INIT_ADC1DMA(uint16_t* adcArray, uint32_t bufferSize)
{
  /*Habilitacion del clock de los perficos del DMA y el ADC1::*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /*Inicio configuracion DMA2:*/
  DMA_DeInit(DMA2_Stream0);
  /*Se elige DMA canal 0 para trabajar con ADC1:*/
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  /*Registro donde se guardan los valores convertidos:*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  /*Direccion del arreglo donde se guardan los valores leidos:*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) adcArray;
  /*Establecer que el DMA transmitira a memoria:*/
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  /*Establecer la cantidad de valores a convertir:*/
  DMA_InitStructure.DMA_BufferSize = bufferSize;
  /*Se deshabilita el incremento de memoria por periferico:*/
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /*Se habilita el incremento de memoria para crear un arreglo dinamico:*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /*Ancho de bit del dato:*/
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  /*Ancho de cada elemento del arreglo:*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  /*Modo circuilar, cuando llega al final del arreglo vuelve a arrancar desde el primer valor:*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  /*Maxima prioridad al DMA:*/
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  /*Configuracion del handler para la interrupcion:*/
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

  /*Habilitacion del DMA:*/
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /*Inicio configuracion del ADC1:*/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  /*Se habilita el acceso del DMA:*/
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

  /*Cargar la configuracion:*/
  ADC_CommonInit(&ADC_CommonInitStructure);

  /*Resolucion del ADC en 12 bits:*/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  /*Habilita la conversion de varios canales simultaneamente:*/
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  /*Se deshabilita el modo continuo, ya que el ADC se va a activar por el TIM3:*/
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  /*Disparo por flanco ascendente:*/
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  /*Define el TMRGO del TIM3 como disparo del ADC:*/
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  /*Cantidad de datos que se van a convertir por disparo del ADC (tension y corriente):*/
  ADC_InitStructure.ADC_NbrOfConversion = 1;

  /*Inicializacion del ADC con los parametros establecidos:*/
  ADC_Init(ADC1, &ADC_InitStructure);

  /*Configuracion de los canales del ADC:*/
  /*Canal 13 ADC1 (PC13) con orden 2 para conversion de corriente:*/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_480Cycles);
  /*Habilitacion del pedido del DMA en el ADC1:*/
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /* PC1 para entrada analógica */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /*Habilitacion del ADC1:*/
  ADC_Cmd(ADC1, ENABLE);
}

/*****************************************************************************
INIT_TIM3

        * @author	A. Riedinger.
        * @brief	Inicializa las interrupciones del TIM3.
        * @returns	void
        * @param
        * @ej
                - INIT_TIM3();
******************************************************************************/
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

void INIT_USART_RX_TX(GPIO_TypeDef* Port1, uint16_t Pin1, GPIO_TypeDef* Port2, uint16_t Pin2, uint32_t BaudRate)
{
    /*USART clock enable:*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /*GPIO clock enable:*/
    RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port1), ENABLE);
    RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port2), ENABLE);

    /*GPIO Configuration:*/
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = Pin1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Port1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Pin2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Port2, &GPIO_InitStructure);

    /*Connect USART pins to AF:*/
    GPIO_PinAFConfig(Port1, FIND_PINSOURCE(Pin1), GPIO_AF_USART2);
    GPIO_PinAFConfig(Port2, FIND_PINSOURCE(Pin2), GPIO_AF_USART2);

    /*USARTx configuration:*/
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
}

/*------------------------------------------------------------------------------
 FUNCIONES INTERNAS:
------------------------------------------------------------------------------*/
uint32_t FIND_CLOCK(GPIO_TypeDef* Port)
{
    uint32_t Clock;

    if      (Port == GPIOA) Clock = RCC_AHB1Periph_GPIOA;
    else if (Port == GPIOB) Clock = RCC_AHB1Periph_GPIOB;
    else if (Port == GPIOC) Clock = RCC_AHB1Periph_GPIOC;
    else if (Port == GPIOD) Clock = RCC_AHB1Periph_GPIOD;
    else if (Port == GPIOE) Clock = RCC_AHB1Periph_GPIOE;
    else if (Port == GPIOF) Clock = RCC_AHB1Periph_GPIOF;
    else if (Port == GPIOG) Clock = RCC_AHB1Periph_GPIOG;
    return Clock;
}

uint8_t FIND_PINSOURCE(uint32_t Pin)
{
    if     (Pin == GPIO_Pin_0)  return GPIO_PinSource0;
    else if(Pin == GPIO_Pin_1)  return GPIO_PinSource1;
    else if(Pin == GPIO_Pin_2)  return GPIO_PinSource2;
    else if(Pin == GPIO_Pin_3)  return GPIO_PinSource3;
    else if(Pin == GPIO_Pin_4)  return GPIO_PinSource4;
    else if(Pin == GPIO_Pin_5)  return GPIO_PinSource5;
    else if(Pin == GPIO_Pin_6)  return GPIO_PinSource6;
    else if(Pin == GPIO_Pin_7)  return GPIO_PinSource7;
    else if(Pin == GPIO_Pin_8)  return GPIO_PinSource8;
    else if(Pin == GPIO_Pin_9)  return GPIO_PinSource9;
    else if(Pin == GPIO_Pin_10) return GPIO_PinSource10;
    else if(Pin == GPIO_Pin_11) return GPIO_PinSource11;
    else if(Pin == GPIO_Pin_12) return GPIO_PinSource12;
    else if(Pin == GPIO_Pin_13) return GPIO_PinSource13;
    else if(Pin == GPIO_Pin_14) return GPIO_PinSource14;
    else if(Pin == GPIO_Pin_15) return GPIO_PinSource15;
    else
         return 0;
}

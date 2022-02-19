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

void INIT_ADC1DMA(uint16_t* adcArray, uint32_t bufferSize)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) adcArray;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = bufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

  DMA_Cmd(DMA2_Stream0, ENABLE);

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;

  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_480Cycles);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  ADC_Cmd(ADC1, ENABLE);
}

void INIT_ADC(GPIO_TypeDef* Port, uint16_t Pin)
{
  uint32_t Clock;
  Clock = FIND_CLOCK(Port);

  ADC_TypeDef* ADCX;
  ADCX = FIND_ADC_TYPE(Port, Pin);

  uint32_t RCC_APB;
  RCC_APB = FIND_RCC_APB(ADCX);

  uint8_t Channel;
  Channel = FIND_CHANNEL(Port, Pin);

  GPIO_InitTypeDef        GPIO_InitStructure;
  ADC_InitTypeDef         ADC_InitStructure;
  ADC_CommonInitTypeDef   ADC_CommonInitStructure;

  //Habilitacion del Clock para el puerto donde esta conectado el ADC:
  RCC_AHB1PeriphClockCmd(Clock, ENABLE);

  //Configuracion del PIN del ADC como entrada ANALOGICA.
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin     = Pin;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL ;
  GPIO_Init(Port, &GPIO_InitStructure);

  //Activar ADC:
  RCC_APB2PeriphClockCmd(RCC_APB, ENABLE);

  //ADC Common Init:
  ADC_CommonStructInit(&ADC_CommonInitStructure);
  ADC_CommonInitStructure.ADC_Mode                = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler           = ADC_Prescaler_Div4; // max 36 MHz segun datasheet
  ADC_CommonInitStructure.ADC_DMAAccessMode       = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay    = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  //ADC Init:
  ADC_StructInit (&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode           = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode     = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion        = 1;
  ADC_Init(ADCX, &ADC_InitStructure);

  //Establecer la configuración de conversion:
  ADC_InjectedSequencerLengthConfig(ADCX, 1);
  ADC_SetInjectedOffset(ADCX, ADC_InjectedChannel_1, 0);
  ADC_InjectedChannelConfig(ADCX, Channel, 1, ADC_SampleTime_480Cycles);

  /* Poner en marcha ADC ----------------------------------------------------*/
  ADC_Cmd(ADCX, ENABLE);
}

int32_t READ_ADC(GPIO_TypeDef* Port, uint16_t Pin)
{
  uint32_t ADC_DATA;

  ADC_TypeDef* ADCX;
  ADCX = FIND_ADC_TYPE(Port, Pin);

  ADC_ClearFlag(ADCX, ADC_FLAG_JEOC);
  ADC_SoftwareStartInjectedConv(ADCX);
  while (ADC_GetFlagStatus(ADCX, ADC_FLAG_JEOC) == RESET);

  ADC_DATA = ADC_GetInjectedConversionValue(ADCX, ADC_InjectedChannel_1);
  return ADC_DATA;
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

//ADC:
ADC_TypeDef* FIND_ADC_TYPE (GPIO_TypeDef* Port, uint32_t Pin)
{
	ADC_TypeDef* ADCX;

	if 		((Port == GPIOA && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   || Pin == GPIO_Pin_2 ||
								Pin == GPIO_Pin_3 || Pin == GPIO_Pin_4   || Pin == GPIO_Pin_5 ||
								Pin == GPIO_Pin_6 || Pin == GPIO_Pin_7)) ||

			 (Port == GPIOB && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1)) ||

			 (Port == GPIOC && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   || Pin == GPIO_Pin_2 ||
					  	  	  	Pin == GPIO_Pin_3 || Pin == GPIO_Pin_4   || Pin == GPIO_Pin_5)))
		ADCX = ADC1;

	else if ((Port == GPIOF && (Pin == GPIO_Pin_3 || Pin == GPIO_Pin_4   || Pin == GPIO_Pin_5 ||
								Pin == GPIO_Pin_6 || Pin == GPIO_Pin_7   || Pin == GPIO_Pin_8 ||
								Pin == GPIO_Pin_9 || Pin == GPIO_Pin_10)))
		ADCX = ADC3;

	else
		ADCX = NULL;

	return ADCX;
}

uint32_t FIND_RCC_APB(ADC_TypeDef* ADCX)
{
	uint32_t RCC_APB;

	if 		(ADCX == ADC1) RCC_APB = RCC_APB2Periph_ADC1;
	else if (ADCX == ADC3) RCC_APB = RCC_APB2Periph_ADC3;
	else 				   RCC_APB = 0;

	return RCC_APB;
}

uint8_t FIND_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin)
{
	uint8_t Channel;

	if 		(Port == GPIOA && Pin == GPIO_Pin_0)  Channel = ADC_Channel_0;  else if (Port == GPIOA && Pin == GPIO_Pin_1)  Channel = ADC_Channel_1;
	else if (Port == GPIOA && Pin == GPIO_Pin_2)  Channel = ADC_Channel_2;  else if (Port == GPIOA && Pin == GPIO_Pin_3)  Channel = ADC_Channel_3;
	else if (Port == GPIOA && Pin == GPIO_Pin_4)  Channel = ADC_Channel_4;  else if (Port == GPIOA && Pin == GPIO_Pin_5)  Channel = ADC_Channel_5;
	else if (Port == GPIOA && Pin == GPIO_Pin_6)  Channel = ADC_Channel_6;  else if (Port == GPIOA && Pin == GPIO_Pin_7)  Channel = ADC_Channel_7;
	else if (Port == GPIOB && Pin == GPIO_Pin_0)  Channel = ADC_Channel_8;  else if (Port == GPIOB && Pin == GPIO_Pin_1)  Channel = ADC_Channel_9;
	else if (Port == GPIOC && Pin == GPIO_Pin_0)  Channel = ADC_Channel_10; else if (Port == GPIOC && Pin == GPIO_Pin_1)  Channel = ADC_Channel_11;
	else if (Port == GPIOC && Pin == GPIO_Pin_2)  Channel = ADC_Channel_12; else if (Port == GPIOC && Pin == GPIO_Pin_3)  Channel = ADC_Channel_13;
	else if (Port == GPIOC && Pin == GPIO_Pin_4)  Channel = ADC_Channel_14;	else if (Port == GPIOC && Pin == GPIO_Pin_5)  Channel = ADC_Channel_15;
	else if (Port == GPIOF && Pin == GPIO_Pin_3)  Channel = ADC_Channel_9;	else if (Port == GPIOF && Pin == GPIO_Pin_4)  Channel = ADC_Channel_14;
	else if (Port == GPIOF && Pin == GPIO_Pin_5)  Channel = ADC_Channel_15;	else if (Port == GPIOF && Pin == GPIO_Pin_6)  Channel = ADC_Channel_4;
	else if (Port == GPIOF && Pin == GPIO_Pin_7)  Channel = ADC_Channel_5;	else if (Port == GPIOF && Pin == GPIO_Pin_8)  Channel = ADC_Channel_6;
	else if (Port == GPIOF && Pin == GPIO_Pin_9)  Channel = ADC_Channel_7;	else if (Port == GPIOF && Pin == GPIO_Pin_10) Channel = ADC_Channel_8;
	else 										  Channel = 0;

	return Channel;
}

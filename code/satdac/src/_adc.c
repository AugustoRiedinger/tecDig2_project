#include "_adc.h"

/*Inicializacion del ADC:*/
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

/*Leer valor de ADC:*/
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

/*Control del ADC:*/
ADC_TypeDef* FIND_ADC_TYPE (GPIO_TypeDef* Port, uint32_t Pin)
{
    ADC_TypeDef* ADCX;

    if  ((Port == GPIOA && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   || Pin == GPIO_Pin_2 ||
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

    if      (ADCX == ADC1) RCC_APB = RCC_APB2Periph_ADC1;
    else if (ADCX == ADC3) RCC_APB = RCC_APB2Periph_ADC3;
    else                   RCC_APB = 0;

    return RCC_APB;
}

uint8_t FIND_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin)
{
  uint8_t Channel;

  if      (Port == GPIOA && Pin == GPIO_Pin_0)  Channel = ADC_Channel_0;  else if (Port == GPIOA && Pin == GPIO_Pin_1)  Channel = ADC_Channel_1;
  else if (Port == GPIOA && Pin == GPIO_Pin_2)  Channel = ADC_Channel_2;  else if (Port == GPIOA && Pin == GPIO_Pin_3)  Channel = ADC_Channel_3;
  else if (Port == GPIOA && Pin == GPIO_Pin_4)  Channel = ADC_Channel_4;  else if (Port == GPIOA && Pin == GPIO_Pin_5)  Channel = ADC_Channel_5;
  else if (Port == GPIOA && Pin == GPIO_Pin_6)  Channel = ADC_Channel_6;  else if (Port == GPIOA && Pin == GPIO_Pin_7)  Channel = ADC_Channel_7;
  else if (Port == GPIOB && Pin == GPIO_Pin_0)  Channel = ADC_Channel_8;  else if (Port == GPIOB && Pin == GPIO_Pin_1)  Channel = ADC_Channel_9;
  else if (Port == GPIOC && Pin == GPIO_Pin_0)  Channel = ADC_Channel_10; else if (Port == GPIOC && Pin == GPIO_Pin_1)  Channel = ADC_Channel_11;
  else if (Port == GPIOC && Pin == GPIO_Pin_2)  Channel = ADC_Channel_12; else if (Port == GPIOC && Pin == GPIO_Pin_3)  Channel = ADC_Channel_13;
  else if (Port == GPIOC && Pin == GPIO_Pin_4)  Channel = ADC_Channel_14; else if (Port == GPIOC && Pin == GPIO_Pin_5)  Channel = ADC_Channel_15;
  else if (Port == GPIOF && Pin == GPIO_Pin_3)  Channel = ADC_Channel_9;  else if (Port == GPIOF && Pin == GPIO_Pin_4)  Channel = ADC_Channel_14;
  else if (Port == GPIOF && Pin == GPIO_Pin_5)  Channel = ADC_Channel_15; else if (Port == GPIOF && Pin == GPIO_Pin_6)  Channel = ADC_Channel_4;
  else if (Port == GPIOF && Pin == GPIO_Pin_7)  Channel = ADC_Channel_5;  else if (Port == GPIOF && Pin == GPIO_Pin_8)  Channel = ADC_Channel_6;
  else if (Port == GPIOF && Pin == GPIO_Pin_9)  Channel = ADC_Channel_7;  else if (Port == GPIOF && Pin == GPIO_Pin_10) Channel = ADC_Channel_8;
  else                                          Channel = 0;

  return Channel;
}

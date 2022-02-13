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

#include "string.h"

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
#define _Servo1     GPIOC
#define _servo1     GPIO_Pin_0
#define __servo1    GPIO_PinSource0
#define _Servo2     GPIOC
#define _servo2     GPIO_Pin_3
#define __servo2    GPIO_PinSource3

/*Sensor temperatura:*/
#define _LM35   GPIOC
#define _lm35   GPIO_Pin_3

/*RX USART:*/
#define _RX GPIOA
#define _rx GPIO_Pin_3

/*TX USART:*/
#define _TX GPIOA
#define _tx GPIO_Pin_2

/* - - - - PARAMS. - - - -*/
/*Frecuencia PWM:*/
#define freqPWM 10e3

/*Ciclo de trabajo PWM:*/
#define dutyCyclePWM 50

/* - - - -   USART   - - - -*/
/*Codigo SERVO:*/
#define servoCode 22

/*Codigo TEMPERATURA:*/
#define tempCode  44

/*Baud Rate:*/
#define baudRate 9600

/* * * * * * * * * * * * * VAR. GLOBAL * * * * * * * * * * * * */
/* - - - -  GENERAL  - - - -*/
/*Codigo recibido:*/
char receivedCode[8];

/* - - - -   USART  - - - -*/
/*Almacenamiento valor temperatura:*/
uint32_t temp = 0;

/* - - - -  TIMERS  - - - -*/
/*Valor de frecuencia en Hz:*/
uint32_t freq      = 0;
uint32_t dutyCycle = 0;
uint8_t  DE = 6;
uint8_t  desiredElementsServo2 = 6;
uint8_t  elements = 100;
uint8_t  elements2 = 100;


/* * * * * * * * * * * * * FUNCIONES * * * * * * * * * * * * */
/*Inicializacion ADC:*/
void INIT_ADC(void);

/*Funciones configuracion ADC:*/
ADC_TypeDef* FIND_ADC_TYPE(GPIO_TypeDef* Port, uint32_t Pin);
uint32_t     FIND_RCC_APB (ADC_TypeDef*  ADCX);
uint8_t      FIND_CHANNEL (GPIO_TypeDef* Port, uint32_t Pin);
uint32_t     FIND_CLOCK   (GPIO_TypeDef* Port);
uint8_t      FIND_PINSOURCE(uint32_t Pin);

/*Leer ADC:*/
int32_t READ_ADC(GPIO_TypeDef* Port, uint16_t Pin);

/*Inicializacion de salida digital:*/
void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin);

/*Inicialización puertos USART:*/
void INIT_USART_RX_TX(GPIO_TypeDef* Port1, uint16_t Pin1, GPIO_TypeDef* Port2, uint16_t Pin2, uint32_t BaudRate);

/*Inicializacion TIM3:*/
void INIT_TIM3(uint32_t freq);

/*Inicializacion TIM4:*/
void INIT_TIMPWM(void);

/*Inicializacion TIM3:*/
void INIT_TIM3(uint32_t freq);

/*Inicializacion PWM:*/
void INIT_PWM(void);

/*Inicializacion servo:*/
void INIT_SERVO(void);

/*Mover servo:*/
void MOVE_SERVO(void);

/*Comenzar adquision temperatura:*/
void START_TEMP(void);

/*Enviar valor temperatura:*/
void SEND_TEMP(void);

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
    /*Inicio del sistema:*/
    SystemInit();

    /*Inicializacion puertos USART:*/
    INIT_USART_RX_TX(_RX, _rx, _TX, _tx, baudRate);

    /*Inicializacion salida digital:*/
    INIT_DO(_Servo1, _servo1);
    INIT_DO(_Servo2, _servo2);

    INIT_TIM3(5000);

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
          receivedCode[0] = USART_ReceiveData(USART2);

      if      (!strcmp(receivedCode, "a") || !strcmp(receivedCode, "c"))  DE = 3;
      else if (!strcmp(receivedCode, "b") || !strcmp(receivedCode, "d"))  DE = 10;

      /*Reseteo de elementos:*/
      if (elements == 0)
          elements = 100;

      if (elements2 == 0)
          elements2 = 100;
  }
}

/*----------------------------------------------------------------*/
/*INTERRUPCIONES:                                                 */
/*----------------------------------------------------------------*/

/*Interrupcion por vencimiento de cuenta de TIM3 cada 1/FS:*/
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {

        if (elements < DE)
        {
            GPIO_ResetBits(_Servo1, _servo1);
            GPIO_ResetBits(_Servo2, _servo2);
        }
        else
        {
            GPIO_SetBits(_Servo1, _servo1);
            GPIO_SetBits(_Servo2, _servo2);
        }

        elements--;

        /*Rehabilitacion del timer:*/
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
/*----------------------------------------------------------------*/
/*FUNCIONES LOCALES:                                              */
/*----------------------------------------------------------------*/

void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin)
{
    /*Estructura de configuracion:*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*Habilitacion de la senal de reloj para el periferico:*/
    RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port), ENABLE);

    /*Se configura el pin como entrada (GPI0_MODE_IN):*/
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

    /*Se aplica la configuracion definida anteriormente al puerto:*/
    GPIO_Init(Port, &GPIO_InitStructure);
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

/* * * * * * * * * * * * * TIMERS * * * * * * * * * * * * */

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

/*Adquisicion temperatura:*/
void START_TEMP(void)
{
    /*Lectura del valor digital:*/
    temp = READ_ADC(_LM35, _lm35);

    /*Enviar el dato de temperatura leido:*/
    SEND_TEMP();
}

/*Enviar valor de temperatura:*/
void SEND_TEMP()
{
    /*Clarear el flag de estado para transmitir:*/
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {}

    /*Iniciar la transmision:*/
    USART_SendData(USART2, temp);
}
/*Inicializacion de dos ADC:*/
void INIT_ADC(void) {

	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	/* Puerto C -------------------------------------------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* PC1 para entrada analógica */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Activar ADC1 ----------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* Activar ADC2 ----------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

	/* ADC Common Init -------------------------------------------------------*/
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // max 36 MHz segun datasheet
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC Init ---------------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Init(ADC2, &ADC_InitStructure);

	/* Establecer la configuración de conversión ------------------------------*/
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_1, 0);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 1,
			ADC_SampleTime_480Cycles);

	/* Establecer la configuración de conversión ------------------------------*/
	ADC_InjectedSequencerLengthConfig(ADC2, 1);
	ADC_SetInjectedOffset(ADC2, ADC_InjectedChannel_1, 0);
	ADC_InjectedChannelConfig(ADC2, ADC_Channel_13, 1,
			ADC_SampleTime_480Cycles);

	/* Poner en marcha ADC ----------------------------------------------------*/
	ADC_Cmd(ADC1, ENABLE);

	/* Poner en marcha ADC ----------------------------------------------------*/
    ADC_Cmd(ADC2, ENABLE);
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

ADC_TypeDef* FIND_ADC_TYPE (GPIO_TypeDef* Port, uint32_t Pin)
{
    ADC_TypeDef* ADCX;

    if ((Port == GPIOA && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   ||
                           Pin == GPIO_Pin_2 || Pin == GPIO_Pin_3   ||
                           Pin == GPIO_Pin_4 || Pin == GPIO_Pin_5   ||
                           Pin == GPIO_Pin_6 || Pin == GPIO_Pin_7)) ||

        (Port == GPIOB && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1)) ||
        (Port == GPIOC && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   ||
                           Pin == GPIO_Pin_2 || Pin == GPIO_Pin_3   ||
                           Pin == GPIO_Pin_4 || Pin == GPIO_Pin_5)))
        ADCX = ADC1;

    else if ((Port == GPIOF && (Pin == GPIO_Pin_3 ||
                                Pin == GPIO_Pin_4 ||
                                Pin == GPIO_Pin_5 ||
                                Pin == GPIO_Pin_6 ||
                                Pin == GPIO_Pin_7 ||
                                Pin == GPIO_Pin_8 ||
                                Pin == GPIO_Pin_9 ||
                                Pin == GPIO_Pin_10)))
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
    else     RCC_APB = 0;

    return RCC_APB;
}

uint8_t FIND_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin)
{
    uint8_t Channel;

    if      (Port == GPIOA && Pin == GPIO_Pin_0)
        Channel = ADC_Channel_0;
    else if (Port == GPIOA && Pin == GPIO_Pin_1)
        Channel = ADC_Channel_1;
    else if (Port == GPIOA && Pin == GPIO_Pin_2)
        Channel = ADC_Channel_2;
    else if (Port == GPIOA && Pin == GPIO_Pin_3)
        Channel = ADC_Channel_3;
    else if (Port == GPIOA && Pin == GPIO_Pin_4)
        Channel = ADC_Channel_4;
    else if (Port == GPIOA && Pin == GPIO_Pin_5)
        Channel = ADC_Channel_5;
    else if (Port == GPIOA && Pin == GPIO_Pin_6)
        Channel = ADC_Channel_6;
    else if (Port == GPIOA && Pin == GPIO_Pin_7)
        Channel = ADC_Channel_7;
    else if (Port == GPIOB && Pin == GPIO_Pin_0)
        Channel = ADC_Channel_8;
    else if (Port == GPIOB && Pin == GPIO_Pin_1)
        Channel = ADC_Channel_9;
    else if (Port == GPIOC && Pin == GPIO_Pin_0)
        Channel = ADC_Channel_10;
    else if (Port == GPIOC && Pin == GPIO_Pin_1)
        Channel = ADC_Channel_11;
    else if (Port == GPIOC && Pin == GPIO_Pin_2)
        Channel = ADC_Channel_12;
    else if (Port == GPIOC && Pin == GPIO_Pin_3)
        Channel = ADC_Channel_13;
    else if (Port == GPIOC && Pin == GPIO_Pin_4)
        Channel = ADC_Channel_14;
    else if (Port == GPIOC && Pin == GPIO_Pin_5)
        Channel = ADC_Channel_15;
    else if (Port == GPIOF && Pin == GPIO_Pin_3)
        Channel = ADC_Channel_9;
    else if (Port == GPIOF && Pin == GPIO_Pin_4)
        Channel = ADC_Channel_14;
    else if (Port == GPIOF && Pin == GPIO_Pin_5)
        Channel = ADC_Channel_15;
    else if (Port == GPIOF && Pin == GPIO_Pin_6)
        Channel = ADC_Channel_4;
    else if (Port == GPIOF && Pin == GPIO_Pin_7)
        Channel = ADC_Channel_5;
    else if (Port == GPIOF && Pin == GPIO_Pin_8)
        Channel = ADC_Channel_6;
    else if (Port == GPIOF && Pin == GPIO_Pin_9)
        Channel = ADC_Channel_7;
    else if (Port == GPIOF && Pin == GPIO_Pin_10)
        Channel = ADC_Channel_8;
    else
        Channel = 0;

    return Channel;
}

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

void MOVE_SERVO()
{

}

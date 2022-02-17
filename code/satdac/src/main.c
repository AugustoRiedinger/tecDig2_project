/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "functions.h"
/*Definicion pines del hardware:*/
#include "hardware.h"
/*Control de los pines USART:*/
#include "_usart.h"
/*Control pin ADC:*/
#include "_adc.h"
/*Control del servomotor:*/
#include "_servo.h"
/*Control del LCD:*/
#include "_lcd.h"
/*Control de pulsadores - interrupciones externas:*/
#include "_exti.h"

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
  /*Inicio del sistema:*/
  SystemInit();

  /*Inicializacion puertos USART:*/
  INIT_USART_RX_TX(_RX, _rx, _TX, _tx, baudRate);

  /*Inicializacion del servo como salida digital:*/
  INIT_DO(_Servo, _servo);

  /*Inicialización del TIM3 a 5 kHz:*/
  INIT_TIM3(freq);

  /*Inicialización del ADC por DMA:*/
  INIT_ADC1DMA(tempDigValues, maxSampling);

  /*Inicializacion TIM2:*/
  INIT_TIM2(freq);

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
          receivedCode[0] = USART_ReceiveData(USART2);

      if      (!strcmp(receivedCode, "a")) DE = 3;
      else if (!strcmp(receivedCode, "b")) DE = 10;
      else if (!strcmp(receivedCode, "c")) DMA_Cmd(DMA2_Stream0, ENABLE);

      /*Reseteo de elementos:*/
      if (elements == 0)
          elements = 100;
  }
}

/*----------------------------------------------------------------*/
/*                     INTERRUPCIONES:                            */
/*----------------------------------------------------------------*/

/*Interrupcion por vencimiento de cuenta de TIM3 cada 1/FS:*/
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {

      if (elements < DE)
        GPIO_ResetBits(_Servo, _servo);
      else
        GPIO_SetBits(_Servo, _servo);

      elements--;

      /*Rehabilitacion del timer:*/
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

/*Interrupcion por Transfer Request del DMA:*/
void DMA2_Stream0_IRQHandler(void)
{
  /* transmission complete interrupt */
  if (DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0))
  {
    /*Se para el timer:*/
    TIM_Cmd(TIM3, DISABLE);

    /*Resetear el flag del DMA:*/
    DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
  }
}

/*----------------------------------------------------------------*/
/*                    FUNCIONES LOCALES:                          */
/*----------------------------------------------------------------*/
/*Procesamiento de datos del ADC:*/
void PROCESS_ADC_DATA()
{
  for(uint8_t i = 0; i < maxSampling; i++)
    tempAnaValues[i] = tempDigValues[i] * 3.0 / 4095.0;

  for(uint8_t i = 0; i < maxSampling; i++)
    tempAvg = tempAvg + tempAnaValues[i];

  //tempAvg = tempAvg / maxSampling;
}

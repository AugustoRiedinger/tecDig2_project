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

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
          receivedCode[0] = USART_ReceiveData(USART2);

      if      (!strcmp(receivedCode, "a")) DE = 3;
      else if (!strcmp(receivedCode, "b")) DE = 10;
      else if (!strcmp(receivedCode, "c")) GET_TEMP();

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

///*Interrupcion por Transfer Request del DMA:*/
//void DMA2_Stream0_IRQHandler(void)
//{
//  /* transmission complete interrupt */
//  if (DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0))
//  {
//    /*Se para el timer:*/
//    TIM_Cmd(TIM3, DISABLE);
//    ADC_Cmd(ADC1, ENABLE);
//    /*Se habilita el flag para procesar los datos:*/
//    //adc = 1;
//
//    /*Resetear el flag del DMA:*/
//    DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
//  }
//}

/*----------------------------------------------------------------*/
/*                    FUNCIONES LOCALES:                          */
/*----------------------------------------------------------------*/
/*Adquisicion temperatura:*/
void GET_TEMP(void)
{
    /*Lectura del valor digital:*/
    //temp = READ_ADC(_LM35, _lm35);
    //temp = "d";

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
    //USART_SendData(USART2, "d");
}

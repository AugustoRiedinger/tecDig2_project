/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "libs.h"
#include "local.h"
#include "_init.h"

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/*Inicializacion de puertos y funciones del sistema:*/
  INIT_ALL();

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
      /*Se guarda lo recibido en la varibale Data:*/
      temp1[0] = USART_ReceiveData(USART2);
  }
}

/*----------------------------------------------------------------*/
/*INTERRUPCIONES:                                                 */
/*----------------------------------------------------------------*/

/*Interrupcion por vencimiento de cuenta de TIM3 cada 1/FS:*/
void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    /*Refresco del LCD:*/
    LCD();

    /*Rehabilitacion del timer:*/
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
}

/*Interrupcion al pulso por PC6-C1 o PC8-C2:*/
void EXTI9_5_IRQHandler(void)
{
  /*Si la interrupcion fue por linea 6 (PC6 - C1):*/
  if(EXTI_GetITStatus(EXTI_Line6) != RESET){
      /*Si ademas de estar C1 en 1 tambien esta F1 en 1, entonces el switch pulsado es S1:*/
      if(GPIO_ReadInputDataBit(_F1, _f1))       switchTemp = 1;
      /*Si ademas de estar C1 en 1 tambien esta F2 en 1, entonces el switch pulsado es S2:*/
      else if(GPIO_ReadInputDataBit(_F2, _f2))  switchServo1 = 1;

      /*Clear the EXTI line 6 pending bit:*/
      EXTI_ClearITPendingBit(EXTI_Line6);
  }

  /*Si la interrupcion fue por linea 8 (PC8 - C2):*/
  else if(EXTI_GetITStatus(EXTI_Line8) != RESET){
      /*Si ademas de estar C2 en 1 tambien esta F1 en 1, entonces el switch pulsado es S3:*/
      if (GPIO_ReadInputDataBit(_F1, _f1))      switchServo2 = 1;
      /*Si ademas de estar C2 en 1 tambien esta F2 en 1, entonces el switch pulsado es S4:*/
      else if (GPIO_ReadInputDataBit(_F2, _f2)) switchSD = 1;

      /*Clear the EXTI line 8 pending bit:*/
      EXTI_ClearITPendingBit(EXTI_Line8);
  }

  /*Evitar que se muestre la pantalla incial:*/
  initialScreen = 0;
}

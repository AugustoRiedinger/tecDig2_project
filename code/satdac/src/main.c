/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias generales a utilzar:*/
#include "_libs.h"
/*Inicializacion de pines y funciones:*/
#include "_init.h"
/*Funciones locales:*/
#include "_local.h"

/*----------------------------------------------------------------*/
/*                           MAIN:                                */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
  INIT_ALL();

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
        receivedCode[0] = USART_ReceiveData(USART2);

      if      (!strcmp(receivedCode, "a")) DE = 3;
      else if (!strcmp(receivedCode, "b")) DE = 10;
      else if (!strcmp(receivedCode, "c")) READ_TEMP();
      else if (!strcmp(receivedCode, "d")) WRITE_SD();

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

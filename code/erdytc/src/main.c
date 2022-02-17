/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "functions.h"
#include "_hardware.h"
#include "_lcd.h"
#include "_usart.h"
#include "_exti.h"

char receivedCode[8];

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
    /*Inicio del sistema:*/
    SystemInit();

    /*Inicializacion del TIM3:*/
    INIT_TIM3(freqTIM3);

    /*Inicializacion LCD:*/
    INIT_LCD_2x16(LCD_2X16);

    /*Inicializacion puertos USART:*/
    INIT_USART_RX_TX(_RX, _rx, _TX, _tx, baudRate);

    /*Inicializacion interrupciones por pulso externo:*/
    INIT_EXTINT(_C1,_c1);
    INIT_EXTINT(_C2,_c2);
    INIT_DO(_F1,_f1);
    INIT_DO(_F2,_f2);
    GPIO_SetBits(_F1, _f1);

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
          receivedCode[0] = USART_ReceiveData(USART2);
  }
}

/*----------------------------------------------------------------*/
/*INTERRUPCIONES:                                                 */
/*----------------------------------------------------------------*/

/*Interrupcion por vencimiento de cuenta de TIM3 cada 1/FS:*/
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {

        /*Buffer para mostrar el valor de temperatura:*/
        char BuffTemp[buffLen];
        char BuffTest[buffLen];

        /*Refresco LCD:*/
        CLEAR_LCD_2x16(LCD_2X16);

        /*Pantalla incial:*/
        if(initialScreen == 1){
            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 2, 0, "TD II-ERDYTC");
            sprintf(BuffTemp, "%.1f", tempDeg);
            PRINT_LCD_2x16(LCD_2X16, 2, 1, "Temp=");
            PRINT_LCD_2x16(LCD_2X16, 7, 1, BuffTemp);

            sprintf(BuffTest, "%c", receivedCode[0]);
            PRINT_LCD_2x16(LCD_2X16, 0, 0, BuffTest);
        }

        /*Pantalla actualizar temperatura - pulsador S1:*/
        else if(switchTemp == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 2, 0, "TEMPERATURA");
            PRINT_LCD_2x16(LCD_2X16, 2, 1, "ACTUALIZADA");

            /*Pedir el valor de temperatura si pasaron 5 segundos:*/
            if (fiveSecDelay == 20)
                SEND_TEMP();
        }

        /*Pantalla SD - pulsador S2:*/
        else if(switchSD == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 1, 0, "DATOS GUARDADOS");
            PRINT_LCD_2x16(LCD_2X16, 2, 1, "CORRECTAMENTE");

            /*Guardar datos en SD si pasaron 5 segundos:*/
            if (fiveSecDelay == 20)
                SD();
        }

        /*Pantalla mover servo - pulsador S3:*/
        else if(switchServo == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 0, 0, "ANTENA DESPLEGA");
            PRINT_LCD_2x16(LCD_2X16, 2, 1, "CORRECTAMENTE");

            /*Enviar comando servo si pasaron 5 segundos:*/
            if (fiveSecDelay == 20)
                SERVO();
        }

        /*Pantalla menu - pulsador S4:*/
        else if(switchMenu == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 0, 0, "1_Temp.");
            PRINT_LCD_2x16(LCD_2X16, 9, 0, "3_Servo");
            PRINT_LCD_2x16(LCD_2X16, 0, 1, "2_SD");
            PRINT_LCD_2x16(LCD_2X16, 9, 1, "4_Volver");
        }

        /*Reseteo variables:*/
        else {
            initialScreen   = 1;
            fiveSecDelay    = 0;
        }

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
      else if(GPIO_ReadInputDataBit(_F2, _f2))  switchSD = 1;

      /*Clear the EXTI line 6 pending bit:*/
      EXTI_ClearITPendingBit(EXTI_Line6);
  }

  /*Si la interrupcion fue por linea 8 (PC8 - C2):*/
  else if(EXTI_GetITStatus(EXTI_Line8) != RESET){
      /*Si ademas de estar C2 en 1 tambien esta F1 en 1, entonces el switch pulsado es S3:*/
      if (GPIO_ReadInputDataBit(_F1, _f1))      switchServo = 1;
      /*Si ademas de estar C2 en 1 tambien esta F2 en 1, entonces el switch pulsado es S4:*/
      else if (GPIO_ReadInputDataBit(_F2, _f2)) switchMenu = 1;

      /*Clear the EXTI line 8 pending bit:*/
      EXTI_ClearITPendingBit(EXTI_Line8);
  }

  /*Evitar que se muestre la pantalla incial:*/
  initialScreen = 0;
}

/*----------------------------------------------------------------*/
/*FUNCIONES LOCALES:                                              */
/*----------------------------------------------------------------*/
void SEND_TEMP(void){

    /*Resetear flag del switch:*/
    switchTemp = 0;

//    /*Clarear el flag de estado para transmitir:*/
//    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//    {}

    char tempCode[8] = "t";

    /*Iniciar la transmision:*/
    USART_SendData(USART2, tempCode[0]);

}


void SERVO(void){

    /*Resetear flag switch:*/
    switchServo = 0;

    /*Clarear el flag de estado para transmitir:*/
//    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//    {}

    char servoCode[8] = "a";

    /*Iniciar la transmision:*/
    USART_SendData(USART2, servoCode[0]);
}

/* * * * * * * * * * * * * SD * * * * * * * * * * * * */

void SD(void){

  /*Resetear flag switch:*/
  switchSD = 0;

  char servoCode2[8] = "b";

  /*Iniciar la transmision:*/
  USART_SendData(USART2, servoCode2[0]);

}

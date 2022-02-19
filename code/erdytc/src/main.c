/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "functions.h"
#include "_hardware.h"
#include "_lcd.h"
#include "_usart.h"
#include "_exti.h"

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
                TEMP_CODE();
        }

        /*Pantalla SD - pulsador S2:*/
        else if(switchServo1 == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 1, 0, "SERVO POSICION");
            PRINT_LCD_2x16(LCD_2X16, 8, 1, "2");

            /*Guardar datos en SD si pasaron 5 segundos:*/
            if (fiveSecDelay == 20)
                SERVO_2();
        }

        /*Pantalla mover servo - pulsador S3:*/
        else if(switchServo2 == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 1, 0, "SERVO POSICION");
            PRINT_LCD_2x16(LCD_2X16, 8, 1, "1");

            /*Enviar comando servo si pasaron 5 segundos:*/
            if (fiveSecDelay == 20)
                SERVO_1();
        }

        /*Pantalla menu - pulsador S4:*/
        else if(switchSD == 1 && fiveSecDelay <= 20){
            /*Aumentar el contador de los 5 seg:*/
            fiveSecDelay++;

            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 1, 0, "DATOS GUARDADOS");
            PRINT_LCD_2x16(LCD_2X16, 5, 1, "EN SD");


            /*Enviar comando servo si pasaron 5 segundos:*/
            if (fiveSecDelay == 20)
                SD();
        }

        /*Reseteo variables:*/
        else {
            initialScreen   = 1;
            fiveSecDelay    = 0;
        }

        /*Se apaga F1 para medir a continuacion S3 y S4 mediante F2:*/
        if (activeSwitch == 0)
        {
          GPIO_ResetBits(_F1, _f1);
          GPIO_SetBits(_F2, _f2);
          activeSwitch = 1;
        }
        else {
          GPIO_ResetBits(_F2, _f2);
          GPIO_SetBits(_F1, _f1);
          activeSwitch = 0;
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

/*----------------------------------------------------------------*/
/*FUNCIONES LOCALES:                                              */
/*----------------------------------------------------------------*/
void TEMP_CODE(void){
    /*Resetear flag del switch:*/
    switchTemp = 0;

//    /*Clarear el flag de estado para transmitir:*/
//    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//    {}

    /*Iniciar la transmision:*/
    USART_SendData(USART2, tempCode[0]);

    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
      /*Se guarda lo recibido en la varibale Data:*/
      receiveTemp = USART_ReceiveData(USART2);
}

void SERVO_1(void){

    /*Resetear flag switch:*/
    switchServo1 = 0;

    /*Clarear el flag de estado para transmitir:*/
//    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//    {}

    /*Iniciar la transmision:*/
    USART_SendData(USART2, servoCode1[0]);

}

/* * * * * * * * * * * * * SD * * * * * * * * * * * * */

void SERVO_2(void){

  /*Resetear flag switch:*/
  switchServo2 = 0;

  /*Iniciar la transmision:*/
  USART_SendData(USART2, servoCode2[0]);
}

void SD(void)
{
  /*Resetear flag switch:*/
  switchSD = 0;

  /*Iniciar la transmision:*/
  USART_SendData(USART2, sdCode[0]);
}

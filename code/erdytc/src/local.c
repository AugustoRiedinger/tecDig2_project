#include "local.h"

void TEMP_CODE(void){
    /*Resetear flag del switch:*/
    switchTemp = 0;

    /*Iniciar la transmision:*/
    USART_SendData(USART2, tempCode[0]);

    flagTemp2 = 1;
}

void SERVO_1(void){
    /*Resetear flag switch:*/
    switchServo1 = 0;

    /*Iniciar la transmision:*/
    USART_SendData(USART2, servoCode1[0]);

}

void SERVO_2(void){
  /*Resetear flag switch:*/
  switchServo2 = 0;

  /*Iniciar la transmision:*/
  USART_SendData(USART2, servoCode2[0]);
}

void SD(void){
  /*Resetear flag switch:*/
  switchSD = 0;

  /*Iniciar la transmision:*/
  USART_SendData(USART2, sdCode[0]);
}

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

float DECODE_TEMP(char* tempChar)
{
    float tempFloat = 0.0f;

    if     (!strcmp(tempChar, "a")) tempFloat = 22.1;
    else if(!strcmp(tempChar, "b")) tempFloat = 22.3;
    else if(!strcmp(tempChar, "c")) tempFloat = 22.5;
    else if(!strcmp(tempChar, "d")) tempFloat = 22.7;
    else if(!strcmp(tempChar, "e")) tempFloat = 22.9;
    else if(!strcmp(tempChar, "f")) tempFloat = 23.1;
    else if(!strcmp(tempChar, "g")) tempFloat = 23.3;
    else if(!strcmp(tempChar, "h")) tempFloat = 23.5;
    else if(!strcmp(tempChar, "i")) tempFloat = 23.7;
    else if(!strcmp(tempChar, "j")) tempFloat = 23.9;
    else if(!strcmp(tempChar, "k")) tempFloat = 24.1;
    else if(!strcmp(tempChar, "l")) tempFloat = 24.3;
    else if(!strcmp(tempChar, "m")) tempFloat = 24.5;
    else if(!strcmp(tempChar, "n")) tempFloat = 24.7;
    else if(!strcmp(tempChar, "o")) tempFloat = 24.9;
    else if(!strcmp(tempChar, "p")) tempFloat = 25.1;
    else if(!strcmp(tempChar, "q")) tempFloat = 25.3;
    else if(!strcmp(tempChar, "r")) tempFloat = 25.5;
    else if(!strcmp(tempChar, "s")) tempFloat = 25.7;
    else if(!strcmp(tempChar, "t")) tempFloat = 25.9;
    else if(!strcmp(tempChar, "u")) tempFloat = 26.1;
    else if(!strcmp(tempChar, "v")) tempFloat = 26.3;
    else if(!strcmp(tempChar, "w")) tempFloat = 26.5;
    else if(!strcmp(tempChar, "x")) tempFloat = 26.7;
    else if(!strcmp(tempChar, "y")) tempFloat = 26.9;
    else if(!strcmp(tempChar, "z")) tempFloat = 27.1;
    else if(!strcmp(tempChar, "A")) tempFloat = 27.3;
    else if(!strcmp(tempChar, "B")) tempFloat = 27.5;
    else if(!strcmp(tempChar, "C")) tempFloat = 27.7;
    else if(!strcmp(tempChar, "D")) tempFloat = 27.9;
    else if(!strcmp(tempChar, "E")) tempFloat = 28.1;
    else if(!strcmp(tempChar, "F")) tempFloat = 28.3;
    else if(!strcmp(tempChar, "G")) tempFloat = 28.5;
    else if(!strcmp(tempChar, "H")) tempFloat = 28.7;
    else if(!strcmp(tempChar, "I")) tempFloat = 28.9;
    else if(!strcmp(tempChar, "J")) tempFloat = 29.1;
    else if(!strcmp(tempChar, "K")) tempFloat = 29.3;
    else if(!strcmp(tempChar, "L")) tempFloat = 29.5;
    else if(!strcmp(tempChar, "M")) tempFloat = 29.7;
    else if(!strcmp(tempChar, "N")) tempFloat = 29.9;
    else if(!strcmp(tempChar, "O")) tempFloat = 30.1;
    else if(!strcmp(tempChar, "P")) tempFloat = 30.3;
    else if(!strcmp(tempChar, "Q")) tempFloat = 30.5;
    else if(!strcmp(tempChar, "R")) tempFloat = 30.7;
    else if(!strcmp(tempChar, "S")) tempFloat = 30.9;
    else if(!strcmp(tempChar, "T")) tempFloat = 31.1;
    else if(!strcmp(tempChar, "U")) tempFloat = 31.3;
    else if(!strcmp(tempChar, "V")) tempFloat = 31.5;
    else if(!strcmp(tempChar, "W")) tempFloat = 31.7;
    else if(!strcmp(tempChar, "X")) tempFloat = 31.9;
    else if(!strcmp(tempChar, "Y")) tempFloat = 32.0;
    else if(!strcmp(tempChar, "Z")) tempFloat = 22.0;
    else if(!strcmp(tempChar, "0")) tempFloat = 0.0 ;
    else                            tempFloat = -1.0;

    return tempFloat;
}

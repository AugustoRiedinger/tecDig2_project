#ifndef _USART_H_
#define _USART_H_

/* * * * * * * * * * *         LIBRERIAS       * * * * * * * * * * * * * * */
#include "standar.h"
#include "string.h"
#include "stm32f4xx_syscfg.h"

/* * * * * * * * * * *          DEFINES        * * * * * * * * * * * * * * */
/*Baud Rate USART:*/
#define baudRate    50

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
void INIT_USART_RX_TX(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, uint32_t);

/* * * * * * * * * * *         VARIABLES       * * * * * * * * * * * * * * */
/*Temperatura en grados:*/
float tempDeg       = 0;
/*Temperatura en valor digital:*/
uint32_t tempDig    = 0;
/*Flag recibir temperatura:*/

/*Variable para recibir el codigo transmitido por el SATDAC:*/
char receivedTemp[8];

/*Codigos para acceder al SATDAC:*/
char servoCode1[8] = "a";
char servoCode2[8] = "b";
char tempCode  [8] = "c";
char sdCode    [8] = "d";
char tempCode2 [8] = "e";
char tempCode3 [8] = "f";

#endif // _USART_H_

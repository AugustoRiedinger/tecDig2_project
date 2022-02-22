#ifndef _USART_H_
#define _USART_H_

/* * * * * * * * * * *         LIBRERIAS       * * * * * * * * * * * * * * */
#include "string.h"
#include "stm32f4xx_syscfg.h"
#include "standar.h"

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
void INIT_USART_RX_TX(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, uint32_t);

/* * * * * * * * * * *          DEFINES        * * * * * * * * * * * * * * */
/*Baud Rate:*/
#define baudRate 50

/* * * * * * * * * * *         VARIABLES       * * * * * * * * * * * * * * */
/*Recepción de caracteres:*/
char receivedCode[8];

#endif // _USART_H_

#ifndef _USART_H_
#define _USART_H_

/*Baud Rate:*/
#define baudRate 9600

/*Recepción de caracteres:*/
char receivedCode[8];

/*Valor de frecuencia en Hz:*/
uint32_t freq = 5000;
uint8_t  DE = 6;
uint8_t  elements = 100;

#endif // _USART_H_

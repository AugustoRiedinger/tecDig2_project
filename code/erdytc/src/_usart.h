#ifndef _USART_H_
#define _USART_H_
/* - - - -   USART   - - - -*/
/*Baud Rate USART:*/
#define baudRate    9600

/*Temperatura en grados:*/
float tempDeg = 0;

/*Temperatura en valor digital:*/
uint32_t tempDig = 0;

/*Flag recibir temperatura:*/
uint8_t receiveTemp = 0;

#endif // _USART_H_

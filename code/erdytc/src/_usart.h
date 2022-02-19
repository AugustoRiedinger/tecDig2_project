#ifndef _USART_H_
#define _USART_H_
/* - - - -   USART   - - - -*/
/*Baud Rate USART:*/
#define baudRate    9600

/*Temperatura en grados:*/
float tempDeg       = 0;

/*Temperatura en valor digital:*/
uint32_t tempDig    = 0;

/*Flag recibir temperatura:*/
uint32_t receiveTemp = 0;

/*Variable para recibir el codigo transmitido por el SATDAC:*/
char receivedCode[8];

/*Funciones para enviar los codigos de mocion del servo:*/
void SERVO_1(void);
void SERVO_2(void);

/*Funcion para enviar el codigo de pedido de temperatura:*/
void TEMP_CODE(void);

/*Codigos para acceder al SATDAC:*/
char servoCode1[8] = "a";
char servoCode2[8] = "b";
char tempCode  [8] = "c";

#endif // _USART_H_

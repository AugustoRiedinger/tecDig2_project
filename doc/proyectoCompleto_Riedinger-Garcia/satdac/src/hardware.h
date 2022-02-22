#ifndef HARDWARE_H
#define HARDWARE_H

/* * * * * * * * * * * * * CONSTANTES * * * * * * * * * * * * */
/* - - - - HARDWARE - - - -*/
/*Servomotor:*/
#define _Servo     GPIOF
#define _servo     GPIO_Pin_9
#define __servo    GPIO_PinSource9

/*Sensor temperatura:*/
#define _LM35   GPIOC
#define _lm35   GPIO_Pin_0

/*RX USART:*/
#define _RX GPIOA
#define _rx GPIO_Pin_3

/*TX USART:*/
#define _TX GPIOA
#define _tx GPIO_Pin_2

#endif // HARDWARE_H

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "standar.h"
#include "_lcd.h"
#include "libs.h"

/*Pulsadores:*/
#define _F1 GPIOC
#define _f1 GPIO_Pin_9
#define _F2 GPIOB
#define _f2 GPIO_Pin_8
#define _C1 GPIOC
#define _c1 GPIO_Pin_6
#define _C2 GPIOC
#define _c2 GPIO_Pin_8

/*RX USART:*/
#define _RX GPIOA
#define _rx GPIO_Pin_3

/*TX USART:*/
#define _TX GPIOA
#define _tx GPIO_Pin_2

#endif // _HARDWARE_H_

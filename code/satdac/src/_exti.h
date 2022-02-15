#ifndef _EXTI_H_
#define _EXTI_H_

/*Control de interrupcion por pulso externo:*/
#include "stm32f4xx_exti.h"

EXTI_InitTypeDef        EXTI_InitStructure;

/* - - - -   EXTI   - - - -*/
/*Switch temperatura:*/
uint8_t switchTemp  = 0;

/*Switch SD:*/
uint8_t switchSD    = 0;

/*Switch servo:*/
uint8_t switchServo = 0;

/*Switch extra:*/
uint8_t switchMenu = 0;

#endif // _EXTI_H_

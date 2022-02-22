#ifndef _TIMERS_H_
#define _TIMERS_H_

/* * * * * * * * * * *         LIBRERIAS       * * * * * * * * * * * * * * */
#include "stm32f4xx_tim.h"
#include "standar.h"

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
void INIT_TIM3(uint32_t Freq);
void INIT_TIM2(uint32_t Freq);

/* * * * * * * * * * *         VARIABLES       * * * * * * * * * * * * * * */
/*Cada cuanto se interrumpe el timer en frecuencia en Hz:*/
uint32_t freq = 5000;

#endif // _TIMERS_H_

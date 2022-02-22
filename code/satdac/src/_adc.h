#ifndef _ADC_H_
#define _ADC_H_
/* * * * * * * * * * *         LIBRERIAS       * * * * * * * * * * * * * * */
#include "standar.h"
#include "stm32f4xx_adc.h"

/* * * * * * * * * * *         VARIABLES       * * * * * * * * * * * * * * */
/*Variable para almacenar el valor digital de temperatura:*/
uint32_t tempDig = 0;
/*Valor promedio de temperatura:*/
float    tempAna = 0.0;
/*Flag de que hay nuevos datos para procesar:*/
uint8_t  f_newTempData = 0;

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
/*Funcion para procesar los datos del ADC:*/
void READ_TEMP(void);
/*Inicializacion ADC:*/
void INIT_ADC(GPIO_TypeDef* Port, uint16_t Pin);
/*Leer valor ADC:*/
int32_t READ_ADC(GPIO_TypeDef* Port, uint16_t Pin);
/*Control del ADC:*/
ADC_TypeDef *FIND_ADC_TYPE(GPIO_TypeDef *Port, uint32_t Pin);
uint32_t FIND_RCC_APB(ADC_TypeDef *ADCX);
uint8_t FIND_CHANNEL(GPIO_TypeDef *Port, uint32_t Pin);

#endif // _ADC_H_

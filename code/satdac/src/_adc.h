#ifndef _ADC_H_
#define _ADC_H_

#define maxSampling 1000

/*Almacenamiento valor temperatura:*/
uint16_t tempDigValues[maxSampling];
float    tempAnaValues[maxSampling];

/*Valor promedio de temperatura:*/
float    tempAvg = 0;

/*Flag de que hay nuevos datos para procesar:*/
uint8_t  f_newTempData = 0;

/*Funcion para procesar los datos del ADC:*/
void PROCESS_ADC_DATA(void);

#endif // _ADC_H_

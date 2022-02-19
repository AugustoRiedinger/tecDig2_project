#ifndef _ADC_H_
#define _ADC_H_

#define maxSampling 1000

/*Almacenamiento valor temperatura:*/
uint16_t tempDigValues[maxSampling];
float    tempAnaValues[maxSampling];

/*Variable para almacenar el valor digital de temperatura:*/
uint32_t tempDig = 0;
/*Valor promedio de temperatura:*/
float    tempAvg = 0;

/*Flag de que hay nuevos datos para procesar:*/
uint8_t  f_newTempData = 0;

/*Funcion para procesar los datos del ADC:*/
void READ_TEMP(void);

#endif // _ADC_H_

#ifndef _ADC_H_
#define _ADC_H_

#define maxSampling 1000

/*Almacenamiento valor temperatura:*/
uint16_t tempDigValues[maxSampling];
float    tempAnaValues[maxSampling];

/*Comenzar adquision temperatura:*/
void GET_TEMP(void);

/*Enviar valor temperatura:*/
void SEND_TEMP(void);

#endif // _ADC_H_

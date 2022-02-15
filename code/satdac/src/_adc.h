#ifndef _ADC_H_
#define _ADC_H_

/*Almacenamiento valor temperatura:*/
uint32_t temp = 0;

/*Comenzar adquision temperatura:*/
void GET_TEMP(void);

/*Enviar valor temperatura:*/
void SEND_TEMP(void);

#endif // _ADC_H_

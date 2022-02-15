#ifndef _ADC_H_
#define _ADC_H_

/*Almacenamiento valor temperatura:*/
char temp[8];

/*Comenzar adquision temperatura:*/
void GET_TEMP(void);

/*Enviar valor temperatura:*/
void SEND_TEMP(void);

#endif // _ADC_H_

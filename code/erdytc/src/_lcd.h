#ifndef _LCD_H_
#define _LCD_H_

/*Longitud general de buffers:*/
#define buffLen 20

/*Agotamiento de cuenta del TIM3 para refrescar LCD:*/
#define freqTIM3 4

/*Pantalla inicial:*/
uint8_t initialScreen = 1;

/*Contador de 5 segundos:*/
uint8_t fiveSecDelay = 0;

#endif // _LCD_H_

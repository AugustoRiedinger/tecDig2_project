#ifndef _LCD_H_
#define _LCD_H_

/*Tiempo de interrupcion cada 250 mseg = 0.250 seg:*/
#define systickIntTime 0.001

/*Flag para actualizar valores del LCD:*/
uint32_t f_lcd = 0;

/*Funcion para actualizar valores en el LCD:*/
void LCD(void);

/*Longitud de los buffer para mostrar palabras:*/
#define buffLen 20

/*Pantalla inicial:*/
uint8_t initialScreen = 1;

/*Contador de 5 segundos:*/
uint8_t fiveSecDelay = 0;

#endif // _LCD_H_

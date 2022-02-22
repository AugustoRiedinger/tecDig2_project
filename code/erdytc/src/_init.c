#include "local.h"

/*Inicializacion de puertos y funciones del sistema:*/
void INIT_ALL(void)
{
    /*Inicio del sistema:*/
    SystemInit();

    /*Inicializacion del TIM3:*/
    INIT_TIM3(freqTIM3);

    /*Inicializacion LCD:*/
    INIT_LCD_2x16(LCD_2X16);

    /*Inicializacion puertos USART:*/
    INIT_USART_RX_TX(_RX, _rx, _TX, _tx, baudRate);

    /*Inicializacion interrupciones por pulso externo:*/
    INIT_EXTINT(_C1,_c1);
    INIT_EXTINT(_C2,_c2);
    INIT_DO(_F1,_f1);
    INIT_DO(_F2,_f2);
    GPIO_SetBits(_F1, _f1);
}

#ifndef HARDWARE_H
#define HARDWARE_H

/* * * * * * * * * * * * * CONSTANTES * * * * * * * * * * * * */
/* - - - - HARDWARE - - - -*/
/*Servomotor:*/
#define _Servo     GPIOC
#define _servo     GPIO_Pin_0
#define __servo    GPIO_PinSource0

/*Sensor temperatura:*/
#define _LM35   GPIOC
#define _lm35   GPIO_Pin_3

/*RX USART:*/
#define _RX GPIOA
#define _rx GPIO_Pin_3

/*TX USART:*/
#define _TX GPIOA
#define _tx GPIO_Pin_2

/*Pulsadores:*/
#define _F1 GPIOC
#define _f1 GPIO_Pin_9
#define _F2 GPIOB
#define _f2 GPIO_Pin_8
#define _C1 GPIOC
#define _c1 GPIO_Pin_6
#define _C2 GPIOC
#define _c2 GPIO_Pin_8

/*Definicion de pines del LCD:*/
LCD_2X16_t LCD_2X16[] = {
    /* Name  , PORT ,     PIN    ,         CLOCK       ,   Init    */
    { TLCD_RS, GPIOE, GPIO_Pin_13, RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_E,  GPIOF, GPIO_Pin_15, RCC_AHB1Periph_GPIOF, Bit_RESET },
    { TLCD_D4, GPIOF, GPIO_Pin_13, RCC_AHB1Periph_GPIOF, Bit_RESET },
    { TLCD_D5, GPIOE, GPIO_Pin_9,  RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_D6, GPIOE, GPIO_Pin_11, RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_D7, GPIOF, GPIO_Pin_14, RCC_AHB1Periph_GPIOF, Bit_RESET },};

#endif // HARDWARE_H

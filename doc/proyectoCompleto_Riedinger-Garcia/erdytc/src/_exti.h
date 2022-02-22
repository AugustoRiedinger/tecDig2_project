#ifndef _EXTI_H_
#define _EXTI_H_

#include "libs.h"

/* * * * * * * * * * *         VARIABLES       * * * * * * * * * * * * * * */
/*Switch temperatura:*/
uint8_t switchTemp   = 0;

/*Switch SD:*/
uint8_t switchServo1 = 0;

/*Switch servo:*/
uint8_t switchServo2 = 0;

/*Switch extra:*/
uint8_t switchSD     = 0;

/*Control de switch activo:*/
uint8_t activeSwitch = 0;

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
void UPDATE_SWITCH(void);

/*Configuracion de nterrupciones externas:*/
void INIT_EXTINT(GPIO_TypeDef* Port, uint16_t Pin);
uint8_t FIND_EXTI_PORT_SOURCE(GPIO_TypeDef* Port);
uint8_t FIND_EXTI_PIN_SOURCE(uint32_t Pin);
uint32_t FIND_EXTI_LINE(uint32_t Pin);
uint32_t FIND_EXTI_HANDLER(uint32_t Pin);

#endif // _EXTI_H_

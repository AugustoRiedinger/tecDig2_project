#ifndef _EXTI_H_
#define _EXTI_H_

/* - - - -   EXTI   - - - -*/
/*Switch temperatura:*/
uint8_t switchTemp  = 0;

/*Switch SD:*/
uint8_t switchSD    = 0;

/*Switch servo:*/
uint8_t switchServo = 0;

/*Switch extra:*/
uint8_t switchMenu = 0;

/*Funciones de accion de los pulsadores:*/
void RECEIVE_TEMP();
void SEND_TEMP();
void SERVO(void);
void SD(void);

#endif // _EXTI_H_

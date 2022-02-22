#include "_init.h"

void INIT_ALL(void)
{
  /*Inicio del sistema:*/
  SystemInit();
  /*Inicializacion puertos USART:*/
  INIT_USART_RX_TX(_RX, _rx, _TX, _tx, baudRate);
  /*Inicializacion del servo como salida digital:*/
  INIT_DO(_Servo, _servo);
  /*Inicialización del TIM3 a 5 kHz:*/
  INIT_TIM3(freq);
  /*Inicializacion PC3 como ADC (LM35)*/
  INIT_ADC(_LM35, _lm35);
  /*Inicializacion tarjeta SD:*/
  UB_Fatfs_Init();
}

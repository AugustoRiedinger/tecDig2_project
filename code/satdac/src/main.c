/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "functions.h"
/*Definicion pines del hardware:*/
#include "hardware.h"
/*Control de los pines USART:*/
#include "_usart.h"
/*Control pin ADC:*/
#include "_adc.h"
/*Control del servomotor:*/
#include "_servo.h"
/*Control del LCD:*/
#include "_lcd.h"
/*Control de pulsadores - interrupciones externas:*/
#include "_exti.h"

char hola[8] = "hola";
/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
  /*Inicio del sistema:*/
  SystemInit();

  /*Inicializacion puertos USART:*/
  INIT_USART_RX_TX(_RX, _rx, _TX, _tx, baudRate);

  /*Inicializacion del servo como salida digital:*/
  INIT_DO(_Servo, _servo);

  /*Inicialización del TIM3 a 5 kHz:*/
  INIT_TIM3(freq);

  /*Inicialización del ADC por DMA:*/
  INIT_ADC1DMA(tempDigValues, maxSampling);

  /*Inicializacion LCD:*/
  INIT_LCD_2x16(LCD_2X16);

  /*Inicializacion interrupciones por pulso externo:*/
  INIT_EXTINT(_C1,_c1);
  INIT_EXTINT(_C2,_c2);
  INIT_DO(_F1,_f1);
  INIT_DO(_F2,_f2);
  GPIO_SetBits(_F1, _f1);

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
          receivedCode[0] = USART_ReceiveData(USART2);

      USART_SendData(USART2, hola[0]);

      if      (!strcmp(receivedCode, "a")) DE = 3;
      else if (!strcmp(receivedCode, "b")) DE = 10;
      else if (!strcmp(receivedCode, "c")) DMA_Cmd(DMA2_Stream0, ENABLE);

      if (f_lcd == 1000) LCD();

      /*Reseteo de elementos:*/
      if (elements == 0)
          elements = 100;
  }
}

/*----------------------------------------------------------------*/
/*                     INTERRUPCIONES:                            */
/*----------------------------------------------------------------*/

/*Interrupcion por vencimiento de cuenta de TIM3 cada 1/FS:*/
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {

    	f_lcd++;

        if (elements < DE)
            GPIO_ResetBits(_Servo, _servo);
        else
            GPIO_SetBits(_Servo, _servo);

        elements--;

        /*Rehabilitacion del timer:*/
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

/*Interrupcion por Transfer Request del DMA:*/
//void DMA2_Stream0_IRQHandler(void)
//{
//  /*Transmission complete interrupt:*/
//  if (DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0))
//  {
//    /*Setear flag:*/
//    f_newTempData = 1;
//
//    DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
//  }
//}

/*Interrupcion al pulso por PC6-C1 o PC8-C2:*/
void EXTI9_5_IRQHandler(void)
{
  /*Si la interrupcion fue por linea 6 (PC6 - C1):*/
  if(EXTI_GetITStatus(EXTI_Line6) != RESET){
      /*Si ademas de estar C1 en 1 tambien esta F1 en 1, entonces el switch pulsado es S1:*/
      if(GPIO_ReadInputDataBit(_F1, _f1))       switchTemp = 1;
      /*Si ademas de estar C1 en 1 tambien esta F2 en 1, entonces el switch pulsado es S2:*/
      else if(GPIO_ReadInputDataBit(_F2, _f2))  switchSD = 1;

      /*Clear the EXTI line 6 pending bit:*/
      EXTI_ClearITPendingBit(EXTI_Line6);
  }

  /*Si la interrupcion fue por linea 8 (PC8 - C2):*/
  else if(EXTI_GetITStatus(EXTI_Line8) != RESET){
      /*Si ademas de estar C2 en 1 tambien esta F1 en 1, entonces el switch pulsado es S3:*/
      if (GPIO_ReadInputDataBit(_F1, _f1))      switchServo = 1;
      /*Si ademas de estar C2 en 1 tambien esta F2 en 1, entonces el switch pulsado es S4:*/
      else if (GPIO_ReadInputDataBit(_F2, _f2)) switchMenu = 1;

      /*Clear the EXTI line 8 pending bit:*/
      EXTI_ClearITPendingBit(EXTI_Line8);
  }

  /*Evitar que se muestre la pantalla incial:*/
  initialScreen = 0;
}

/*Interrupcion por tiempo - Systick cada 250mseg:*/
void SysTick_Handler()
{
  //f_lcd++;
}

/*----------------------------------------------------------------*/
/*                    FUNCIONES LOCALES:                          */
/*----------------------------------------------------------------*/
/*Refresco del LCD:*/
void LCD()
{
  /*Reset variable TS:*/
  f_lcd = 0;

  /*Buffer para mostrar el valor de temperatura:*/
  char BuffTemp[buffLen];

  /*Refresco LCD:*/
  CLEAR_LCD_2x16(LCD_2X16);

  /*Pantalla incial:*/
  if(initialScreen == 1){
    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 2, 0, "TD II-ERDYTC");

    /*Comprobar si hay nuevos datos de temperatura:*/
    //if (f_newTempData == 1)
      //PROCESS_ADC_DATA();

    /*Mostrar el ultimo valor de temperatura en la pantalla:*/
    sprintf(BuffTemp, "%c", receivedCode[0]);
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "Temp=");
    PRINT_LCD_2x16(LCD_2X16, 7, 1, BuffTemp);
  }

  /*Pantalla actualizar temperatura - pulsador S1:*/
  else if(switchTemp == 1 && fiveSecDelay <= 20){
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 2, 0, "TEMPERATURA");
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "ACTUALIZADA");
  }

  /*Pantalla SD - pulsador S2:*/
  else if(switchSD == 1 && fiveSecDelay <= 20){
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 1, 0, "DATOS GUARDADOS");
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "CORRECTAMENTE");
  }

  /*Pantalla mover servo - pulsador S3:*/
  else if(switchServo == 1 && fiveSecDelay <= 20){
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 0, 0, "ANTENA DESPLEGA");
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "CORRECTAMENTE");
  }

  /*Pantalla menu - pulsador S4:*/
  else if(switchMenu == 1 && fiveSecDelay <= 20){
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 0, 0, "1_Temp.");
    PRINT_LCD_2x16(LCD_2X16, 9, 0, "3_Servo");
    PRINT_LCD_2x16(LCD_2X16, 0, 1, "2_SD");
    PRINT_LCD_2x16(LCD_2X16, 9, 1, "4_Volver");
  }

  /*Reseteo variables:*/
  else {
    initialScreen   = 1;
    fiveSecDelay    = 0;
  }
}

/*Procesamiento de datos del ADC:*/
void PROCESS_ADC_DATA()
{
  for(uint8_t i = 0; i < maxSampling; i++)
    tempAnaValues[i] = tempDigValues[i] * 3.0 / 4095.0;

  for(uint8_t i = 0; i < maxSampling; i++)
	tempAvg = tempAvg + tempAnaValues[i];

  //tempAvg = tempAvg / maxSampling;
}

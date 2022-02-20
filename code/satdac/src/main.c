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
/*Control de la tarjeta SD:*/
#include "_sd.h"
char buffTemp[8];
char buffTemp1[1] = "H";
char buffTemp2[1] = "O";
char buffTemp3[1] = "L";
char test1[1] = "1";
char test2[1] = "2";
char test3[1] = "3";


uint8_t i = 0;

/*----------------------------------------------------------------*/
/*                           MAIN:                                */
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
  /*Inicializacion PC3 como ADC (LM35)*/
  INIT_ADC(_LM35, _lm35);
  /*Inicializacion tarjeta SD:*/
  UB_Fatfs_Init();

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
      if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
          /*Se guarda lo recibido en la varibale Data:*/
        receivedCode[0] = USART_ReceiveData(USART2);

      if      (!strcmp(receivedCode, "a")) DE = 3;
      else if (!strcmp(receivedCode, "b")) DE = 10;
      else if (!strcmp(receivedCode, "c")) READ_TEMP();
      else if (!strcmp(receivedCode, "d")) WRITE_SD();
      else if (!strcmp(receivedCode, "e")) USART_SendData(USART2, buffTemp[1]);
      else if (!strcmp(receivedCode, "f")) USART_SendData(USART2, buffTemp[2]);

      //WRITE_SD();

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
void DMA2_Stream0_IRQHandler(void)
{
  /* transmission complete interrupt */
  if (DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0))
  {
    /*Se para el timer:*/
    TIM_Cmd(TIM3, DISABLE);

    /*Resetear el flag del DMA:*/
    DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
  }
}

/*----------------------------------------------------------------*/
/*                    FUNCIONES LOCALES:                          */
/*----------------------------------------------------------------*/
void READ_TEMP()
{
  /*Almacenar el valor digital de temperatura:*/
  tempDig = READ_ADC(_LM35, _lm35);

  tempAna = (float) tempDig * 3.0 / 5.0;

  sprintf(buffTemp, "%.1f", tempAna);
  USART_SendData(USART2, buffTemp[0]);

//  USART_SendData(USART2, buffTemp2[0]);

//  if(i == 0)
//	  USART_SendData(USART2, buffTemp1[0]);
//  else if(i == 1)
//	  USART_SendData(USART2, buffTemp2[0]);
//  else if(i == 2)
//	  USART_SendData(USART2, buffTemp3[0]);
//  else
//	  i = 0;
//
//  i++;


//  USART_SendData(USART2, test1[0]);
//  USART_SendData(USART2, buffTemp1[0]);
//  USART_SendData(USART2, test2[0]);
//  USART_SendData(USART2, buffTemp2[0]);
//  USART_SendData(USART2, test3[0]);
//  USART_SendData(USART2, buffTemp3[0]);


//  if (i < 3)
//  {
//	  USART_SendData(USART2, buffTemp[i]);
//	  i++;
//  }
//  else
//	  i = 0;

//  USART_SendData(USART2, buffTemp[1]);
//  USART_SendData(USART2, buffTemp[2]);
//  USART_SendData(USART2, buffTemp[0]);

  /*Enviar temperatura:*/
//  for(uint8_t i = 0; i <= 2; i++)
//    USART_SendData(USART2, buffTemp[i]);
}

void WRITE_SD()
{
  /*Verifica si hay una tarjeta insertada:*/
  if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK)
  {
    /*Verifica si se pueden almacenar datos en la tarjeta ingresada:*/
    if(UB_Fatfs_Mount(MMC_0) == FATFS_OK)
    {
      /*Abre el archivo para escritura:*/
      if(UB_Fatfs_OpenFile(&myFile, "sd.txt", F_WR_NEW) == FATFS_OK)
      {
        /*Escribe el archivo:*/
        UB_Fatfs_WriteString(&myFile, fila1);
        /*Cierra el archivo:*/
        UB_Fatfs_CloseFile(&myFile);
      }
      /*Media unmount:*/
      UB_Fatfs_UnMount(MMC_0);
    }
  }
}

void DELAY(volatile uint32_t n)
{
  while(n--) {};
}

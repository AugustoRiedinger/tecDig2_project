#include "_local.h"

/*Variable para almacenar temperatura:*/
char buffTemp[2];

/*Leer y enviar el valor de temperatura:*/
void READ_TEMP()
{
  /*Almacenar el valor digital de temperatura:*/
  tempDig = READ_ADC(_LM35, _lm35);

  tempAna = (float) tempDig * 3.0 / 5.0;

  //sprintf(buffTemp, "%.1f", tempAna);
  USART_SendData(USART2, DECODE_TEMP(tempAna));
}

/*Escribir en la SD:*/
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

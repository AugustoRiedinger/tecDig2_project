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

char DECODE_TEMP(float tempFloat)
{
  char tempChar[2];

  if     (tempFloat >= 22.0 && tempFloat <= 22.2) strcpy(tempChar, "a");
  else if(tempFloat >  22.2 && tempFloat <= 22.4) strcpy(tempChar, "b");
  else if(tempFloat >  22.4 && tempFloat <= 22.6) strcpy(tempChar, "c");
  else if(tempFloat >  22.6 && tempFloat <= 22.8) strcpy(tempChar, "d");
  else if(tempFloat >  22.8 && tempFloat <= 23.0) strcpy(tempChar, "e");
  else if(tempFloat >  23.0 && tempFloat <= 23.2) strcpy(tempChar, "f");
  else if(tempFloat >  23.2 && tempFloat <= 23.4) strcpy(tempChar, "g");
  else if(tempFloat >  23.4 && tempFloat <= 23.6) strcpy(tempChar, "h");
  else if(tempFloat >  23.6 && tempFloat <= 23.8) strcpy(tempChar, "i");
  else if(tempFloat >  23.8 && tempFloat <= 24.0) strcpy(tempChar, "j");
  else if(tempFloat >  24.0 && tempFloat <= 24.2) strcpy(tempChar, "k");
  else if(tempFloat >  24.2 && tempFloat <= 24.4) strcpy(tempChar, "l");
  else if(tempFloat >  24.4 && tempFloat <= 24.6) strcpy(tempChar, "m");
  else if(tempFloat >  24.6 && tempFloat <= 24.8) strcpy(tempChar, "n");
  else if(tempFloat >  24.8 && tempFloat <= 25.0) strcpy(tempChar, "o");
  else if(tempFloat >  25.0 && tempFloat <= 25.2) strcpy(tempChar, "p");
  else if(tempFloat >  25.2 && tempFloat <= 25.4) strcpy(tempChar, "q");
  else if(tempFloat >  25.4 && tempFloat <= 25.6) strcpy(tempChar, "r");
  else if(tempFloat >  25.6 && tempFloat <= 25.8) strcpy(tempChar, "s");
  else if(tempFloat >  25.8 && tempFloat <= 26.0) strcpy(tempChar, "t");
  else if(tempFloat >  26.0 && tempFloat <= 26.2) strcpy(tempChar, "u");
  else if(tempFloat >  26.2 && tempFloat <= 26.4) strcpy(tempChar, "v");
  else if(tempFloat >  26.4 && tempFloat <= 26.6) strcpy(tempChar, "w");
  else if(tempFloat >  26.6 && tempFloat <= 26.8) strcpy(tempChar, "x");
  else if(tempFloat >  26.8 && tempFloat <= 27.0) strcpy(tempChar, "y");
  else if(tempFloat >  27.0 && tempFloat <= 27.2) strcpy(tempChar, "z");
  else if(tempFloat >  27.2 && tempFloat <= 27.4) strcpy(tempChar, "A");
  else if(tempFloat >  27.4 && tempFloat <= 27.6) strcpy(tempChar, "B");
  else if(tempFloat >  27.6 && tempFloat <= 27.8) strcpy(tempChar, "C");
  else if(tempFloat >  27.8 && tempFloat <= 28.0) strcpy(tempChar, "D");
  else if(tempFloat >  28.0 && tempFloat <= 28.2) strcpy(tempChar, "E");
  else if(tempFloat >  28.2 && tempFloat <= 28.4) strcpy(tempChar, "F");
  else if(tempFloat >  28.4 && tempFloat <= 28.6) strcpy(tempChar, "G");
  else if(tempFloat >  28.6 && tempFloat <= 28.8) strcpy(tempChar, "H");
  else if(tempFloat >  28.8 && tempFloat <= 29.0) strcpy(tempChar, "I");
  else if(tempFloat >  29.0 && tempFloat <= 29.2) strcpy(tempChar, "J");
  else if(tempFloat >  29.2 && tempFloat <= 29.4) strcpy(tempChar, "K");
  else if(tempFloat >  29.4 && tempFloat <= 29.6) strcpy(tempChar, "L");
  else if(tempFloat >  29.6 && tempFloat <= 29.8) strcpy(tempChar, "M");
  else if(tempFloat >  29.8 && tempFloat <= 30.0) strcpy(tempChar, "N");
  else if(tempFloat >  30.0 && tempFloat <= 30.2) strcpy(tempChar, "O");
  else if(tempFloat >  30.2 && tempFloat <= 30.4) strcpy(tempChar, "P");
  else if(tempFloat >  30.4 && tempFloat <= 30.6) strcpy(tempChar, "Q");
  else if(tempFloat >  30.6 && tempFloat <= 30.8) strcpy(tempChar, "R");
  else if(tempFloat >  30.8 && tempFloat <= 31.0) strcpy(tempChar, "S");
  else if(tempFloat >  31.0 && tempFloat <= 31.2) strcpy(tempChar, "T");
  else if(tempFloat >  31.2 && tempFloat <= 31.4) strcpy(tempChar, "U");
  else if(tempFloat >  31.4 && tempFloat <= 31.6) strcpy(tempChar, "V");
  else if(tempFloat >  31.6 && tempFloat <= 31.8) strcpy(tempChar, "W");
  else if(tempFloat >  31.8 && tempFloat <= 32.0) strcpy(tempChar, "X");
  else if(tempFloat >  32.0 )                     strcpy(tempChar, "Y");
  else if(tempFloat <  22.0 )                     strcpy(tempChar, "Z");
  else                                            strcpy(tempChar, "0");

  return tempChar[0];
}

// ------------------------------------------------ --------------
// File: main.c
// Date: 06.04.2013 - 04-11-21
// Version: 1.1
// Author: UB - German Feres
// Email: mc-4u (@) t-online.de
// Web: www.mikrocontroller-4u.de
// CPU: STM32F4
// IDE: CooCox CoIDE 1.7.0
// Modules: CMSIS_BOOT, M4_CMSIS_CORE
// Function: demonstration of FATFS SDIO Library
// Probado con tarjeta SD (formato viejo) de 256 MB

// Note: These two files must be on 8MHz
// "Cmsis_boot / stm32f4xx.h"
// "Cmsis_boot / system_stm32f4xx.c"

// PINES UTILIZADOS SEGUN MODO
//
//  1bit-Mode :
//
//   PC8  -> SDIO_D0  = SD-Karte DAT0
//   PC12 -> SDIO_CK  = SD-Karte Clock
//   PD2  -> SDIO_CMD = SD-Karte CMD
//
// 4bit-Mode :
//
//   PC8  -> SDIO_D0  = SD-Karte DAT0  (pin 8 tarjeta)
//   PC9  -> SDIO_D1  = SD-Karte DAT1  (pin 9 tarjeta)
//   PC10 -> SDIO_D2  = SD-Karte DAT2  (pin 1 tarjeta)
//   PC11 -> SDIO_D3  = SD-Karte DAT3/CD  (pin 2 tarjeta)
//   PC12 -> SDIO_CK  = SD-Karte Clock  (pin 6 tarjeta)
//   PD2  -> SDIO_CMD = SD-Karte CMD    (pin 3 tarjeta)
//   VDD pin 5
//   GND pin 4 y 7
// mit Detect-Pin :
//
//   PC0  -> SD_Detect-Pin (Hi=ohne SD-Karte)
// ------------------------------------------------ --------------

// Configurar en File     : stm32_ub_sdcard.h el modo y si usamos pin deteccción tarjeta

#include "main.h"
#include "stm32_ub_fatfs.h"
#include "stm32_ub_lcd_2x16.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

FIL myFile;   // File Handler
FRESULT archivo;

char fila1[16] = "Hello World";

void Escribir_SD();

int main(void)
{
  SystemInit(); // inicialización clock

  UB_LCD_2x16_Init();

  UB_Fatfs_Init(); // Se inicializa la SD

  Escribir_SD();

  while(1)
  {
  }
}

void Escribir_SD()
{
  if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK)  // Verifica si hay una tarjeta insertada
  {
    if(UB_Fatfs_Mount(MMC_0) == FATFS_OK) // Verifica si se pueden almacenar datos en la tarjeta ingresada
    {
      if(UB_Fatfs_OpenFile(&myFile, "texto.txt", F_WR_NEW) == FATFS_OK) // Abre el archivo para escritura
      {
        UB_Fatfs_WriteString(&myFile, fila1); // Escribe el archivo

        UB_Fatfs_CloseFile(&myFile); // Cierra el archivo
      }
      // Media unmount
      UB_Fatfs_UnMount(MMC_0);
    }
  }

  else // No se detecto ninguna tarjeta SD
  {
    UB_LCD_2x16_String(0,0, "INSERTE SD CARD");
  }
}

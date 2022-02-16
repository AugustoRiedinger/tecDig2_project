//--------------------------------------------------------------
// File     : main.c
// Datum    : 01.01.2014
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.4
// GCC      : 4.7 2012q4
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Funktion : Hauptprogramm
// Hinweis  : Diese zwei Files muessen auf 8MHz stehen
//              "cmsis_boot/stm32f4xx.h"
//              "cmsis_boot/system_stm32f4xx.c"
//--------------------------------------------------------------


/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_ds1307.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32_ub_lcd_2x16.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32_ub_led.h"
#include "stm32_ub_fatfs.h"

int main(void) {
	char str[100];
	TM_DS1307_Time_t time;
	//uint8_t last;
	FIL myFile;
	int A=645;
	/* Initialize system */
	SystemInit();
	UB_Led_Init();
	UB_Fatfs_Init();

	UB_LCD_2x16_Init(); 	//Inicializa el display
	UB_LCD_2x16_Clear();
	/* Initialize DS1307 */
	if (TM_DS1307_Init() != TM_DS1307_Result_Ok)
	{
		/* Show on LCD */
		UB_LCD_2x16_String(5,0,"ERROR");
		while (1);
	}

	/* Set date and time */
	/* Day 7, 26th May 2014, 02:05:00 */

	time.hours = 23;
	time.minutes = 59;
	time.seconds = 30;
	time.date = 30;
	time.day = 3;
	time.month = 9;
	time.year = 20;
	TM_DS1307_SetDateTime(&time);

	/* Set output pin to 4096 Hz */
	TM_DS1307_EnableOutputPin(TM_DS1307_OutputFrequency_4096Hz);

	 // Check ob Medium eingelegt ist
	if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK)
	{
	    // Media mounten
	    if(UB_Fatfs_Mount(MMC_0)==FATFS_OK)
	    {
	      UB_Led_On(LED_BLUE);
	      // File zum schreiben im root neu anlegen
	      if(UB_Fatfs_OpenFile(&myFile, "0:/UB_File.txt", F_WR_CLEAR)==FATFS_OK)
	      {
	        UB_Led_On(LED_GREEN);
	        //UB_Fatfs_WriteString(&myFile,"paso 1");
	while (A!=0)
	{
		/* Get date and time */
		TM_DS1307_GetDateTime(&time);

		/* Display on LCD  */
		sprintf(str, "%02d/%02d/%02d Estado: ", time.date, time.month, time.year );
		UB_LCD_2x16_String(0,0,str);
		//UB_Fatfs_WriteString(&myFile,str);
		sprintf(str, "%02d:%02d:%02d   %02d", time.hours, time.minutes, time.seconds,A/43);
		UB_LCD_2x16_String(0,1,str);
		if(A%43==0)
		{
		UB_Fatfs_WriteString(&myFile,str);
		}
		A--;
	}
	       UB_Fatfs_CloseFile(&myFile);
	      }
	    // Media unmounten
	    UB_Fatfs_UnMount(MMC_0);
	    }
	    else {
	      UB_Led_On(LED_RED);
	    }
	 }
	while(1)
	{
		TM_DS1307_GetDateTime(&time);
		sprintf(str, "%02d/%02d/%02d Estado:", time.date, time.month, time.year);
		UB_LCD_2x16_String(0,0,str);
		sprintf(str, "%02d:%02d:%02d   OK", time.hours, time.minutes, time.seconds);
		UB_LCD_2x16_String(0,1,str);
	}
}

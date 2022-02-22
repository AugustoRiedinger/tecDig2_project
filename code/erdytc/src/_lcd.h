#ifndef _LCD_H_
#define _LCD_H_

/* * * * * * * * * * *         LIBRERIAS       * * * * * * * * * * * * * * */
#include "standar.h"
#include "_menus.h"

/* * * * * * * * * * *          DEFINES        * * * * * * * * * * * * * * */
#define  TLCD_INIT_PAUSE  100000  // pause beim init (>=70000)
#define  TLCD_PAUSE        50000  // kleine Pause (>=20000)
#define  TLCD_CLK_PAUSE     1000  // pause for Clock-Impuls (>=500)
#define  TLCD_MAXX            16  // max x-Position (0...15)
#define  TLCD_MAXY             2  // max y-Position (0...1)
#define  DAC_DHR12R2_ADDRESS   0x40007414
#define  TLCD_CMD_INIT_DISPLAY  0x28   // 2 Zeilen Display, 5x7 Punkte
#define  TLCD_CMD_ENTRY_MODE    0x06   // Cursor increase, Display fix
#define  TLCD_CMD_DISP_M0       0x08   // Display=AUS, Cursor=Aus, Blinken=Aus
#define  TLCD_CMD_DISP_M1       0x0C   // Display=EIN, Cursor=AUS, Blinken=Aus
#define  TLCD_CMD_DISP_M2       0x0E   // Display=EIN, Cursor=EIN, Blinken=Aus
#define  TLCD_CMD_DISP_M3       0x0F   // Display=EIN, Cursor=EIN, Blinken=EIN
#define  TLCD_CMD_CLEAR         0x01   // loescht das Display

typedef enum
{
  TLCD_RS = 0,  // RS-Pin
  TLCD_E  = 1,  // E-Pin
  TLCD_D4 = 2,  // DB4-Pin
  TLCD_D5 = 3,  // DB5-Pin
  TLCD_D6 = 4,  // DB6-Pin
  TLCD_D7 = 5   // DB7-Pin
}TLCD_NAME_t;

#define  TLCD_ANZ   6 // Anzahl von TLCD_NAME_t

typedef enum {
  TLCD_OFF = 0,    // Display=AUS, Cursor=Aus, Blinken=Aus
  TLCD_ON,         // Display=EIN, Cursor=Aus, Blinken=Aus
  TLCD_CURSOR,     // Display=EIN, Cursor=EIN, Blinken=Aus
  TLCD_BLINK       // Display=EIN, Cursor=EIN, Blinken=EIN
}TLCD_MODE_t;

typedef struct {
  TLCD_NAME_t TLCD_NAME;   // Name
  GPIO_TypeDef* TLCD_PORT; // Port
  const uint16_t TLCD_PIN; // Pin
  const uint32_t TLCD_CLK; // Clock
  BitAction TLCD_INIT;     // Init
}LCD_2X16_t;

/* * * * * * * * * * *          DEFINES        * * * * * * * * * * * * * * */
/*Longitud general de buffers:*/
#define buffLen 20

/*Agotamiento de cuenta del TIM3 para refrescar LCD:*/
#define freqTIM3 4

/* * * * * * * * * * *         VARIABLES       * * * * * * * * * * * * * * */
/*Pantalla inicial:*/
uint8_t initialScreen = 1;

/*Contador de 5 segundos:*/
uint8_t fiveSecDelay = 0;

/*Buffers para mostrar las variables:*/
char buffTemp1[8];
char buffTemp2[8];
char buffTemp [8];
char temp1[8];
char temp2[8];
char temp3[8];
uint8_t d = 0;
uint8_t delay = 0;
uint8_t flagTemp2 = 0;
uint8_t flagTemp3 = 0;

/*Definicion de pines del LCD:*/
LCD_2X16_t LCD_2X16[] = {
    /* Name  , PORT ,     PIN    ,         CLOCK       ,   Init    */
    { TLCD_RS, GPIOE, GPIO_Pin_13, RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_E,  GPIOF, GPIO_Pin_15, RCC_AHB1Periph_GPIOF, Bit_RESET },
    { TLCD_D4, GPIOF, GPIO_Pin_13, RCC_AHB1Periph_GPIOF, Bit_RESET },
    { TLCD_D5, GPIOE, GPIO_Pin_9,  RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_D6, GPIOE, GPIO_Pin_11, RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_D7, GPIOF, GPIO_Pin_14, RCC_AHB1Periph_GPIOF, Bit_RESET },};

/* * * * * * * * * * *         FUNCIONES       * * * * * * * * * * * * * * */
/*Refrescar datos LCD:*/
void LCD(void);

/*Funciones generales LCD:*/
void INIT_LCD_2x16(LCD_2X16_t*);
void CLEAR_LCD_2x16(LCD_2X16_t*);
void PRINT_LCD_2x16(LCD_2X16_t*, uint8_t, uint8_t, char*);
void P_LCD_2x16_InitIO(LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Delay(volatile uint32_t nCount);
void P_LCD_2x16_InitSequenz(LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Clk(LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Cursor(LCD_2X16_t *LCD_2X16, uint8_t x, uint8_t y);
void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t *LCD_2X16);

#endif // _LCD_H_

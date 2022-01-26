/*----------------------------------------------------------------*/
/*LIBRERIAS:                                                      */
/*----------------------------------------------------------------*/
/*Librerias estandar:*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stdio.h"
#include "misc.h"

/*Control del clock:*/
#include "stm32f4xx_rcc.h"

/*Control de timers:*/
#include "stm32f4xx_tim.h"

/*Matematicas:*/
#include "math.h"

/*----------------------------------------------------------------*/
/*DEFINICIONES:                                                   */
/*----------------------------------------------------------------*/

/* * * * * * * * * * * * * ESTRUCTURAS * * * * * * * * * * * * */
/*General:*/
GPIO_InitTypeDef        GPIO_InitStructure;
NVIC_InitTypeDef        NVIC_InitStructure;
EXTI_InitTypeDef        EXTI_InitStructure;

/*TIMERS*/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

/*DMA*/
DMA_InitTypeDef         DMA_InitStructure;

/* * * * * * * * * * * * * CONSTANTES * * * * * * * * * * * * */
/* - - - -   LCD   - - - -*/
/*Constantes varias:*/
#define  TLCD_INIT_PAUSE        100000
#define  TLCD_PAUSE             50000
#define  TLCD_CLK_PAUSE         1000
#define  TLCD_MAXX              16
#define  TLCD_MAXY              2
#define  MaxDigCount            4095
#define  MaxMiliVoltRef         3320
#define  Res32Bit               32
#define  DAC_DHR12R2_ADDRESS    0x40007414
#define  TLCD_CMD_INIT_DISPLAY  0x28
#define  TLCD_CMD_ENTRY_MODE    0x06
#define  TLCD_CMD_DISP_M0       0x08
#define  TLCD_CMD_DISP_M1       0x0C
#define  TLCD_CMD_DISP_M2       0x0E
#define  TLCD_CMD_DISP_M3       0x0F
#define  TLCD_CMD_CLEAR         0x01

/*Identificacion pines:*/
typedef enum
{
  TLCD_RS = 0,  // RS-Pin
  TLCD_E  = 1,  // E-Pin
  TLCD_D4 = 2,  // DB4-Pin
  TLCD_D5 = 3,  // DB5-Pin
  TLCD_D6 = 4,  // DB6-Pin
  TLCD_D7 = 5   // DB7-Pin
}TLCD_NAME_t;

/*Nombre LCD:*/
#define  TLCD_ANZ   6

/*Estados LCD:*/
typedef enum {
  TLCD_OFF = 0,
  TLCD_ON,
  TLCD_CURSOR,
  TLCD_BLINK
}TLCD_MODE_t;

/*Crear LCD:*/
typedef struct {
  TLCD_NAME_t TLCD_NAME;
  GPIO_TypeDef* TLCD_PORT;
  const uint16_t TLCD_PIN;
  const uint32_t TLCD_CLK;
  BitAction TLCD_INIT;
}LCD_2X16_t;

/* - - - - HARDWARE - - - -*/

/* - - - - PARAMS. - - - -*/

/* * * * * * * * * * * * * FUNCIONES * * * * * * * * * * * * */
/*Parametros LCD:*/
void P_LCD_2x16_InitIO(LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Delay(volatile uint32_t nCount);
void P_LCD_2x16_InitSequenz(LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Clk(LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Cursor(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y);
void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t* LCD_2X16);

/*Inicializacion LCD:*/
void INIT_LCD_2x16(LCD_2X16_t* LCD_2X16);

/*Refrescar LCD:*/
void CLEAR_LCD_2x16(LCD_2X16_t* LCD_2X16);

/*Imprimir LCD:*/
void PRINT_LCD_2x16(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y, char *ptr);

/*Inicializacion TIM3:*/
void INIT_TIM3(uint32_t freq);

/*Encontrar CLOCK:*/
uint32_t FIND_CLOCK(GPIO_TypeDef* Port);

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
    /*Inicio del sistema:*/
    SystemInit();

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
  }
}

/*----------------------------------------------------------------*/
/*FUNCIONES LOCALES:                                              */
/*----------------------------------------------------------------*/

/* * * * * * * * * * * * *     LCD    * * * * * * * * * * * * */
/*Inicializacion pines LCD:*/
void P_LCD_2x16_InitIO(LCD_2X16_t* LCD_2X16)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TLCD_NAME_t lcd_pin;

    for (lcd_pin = 0; lcd_pin < TLCD_ANZ; lcd_pin++)
    {
        //Habilitacion del Clock para cada PIN:
        RCC_AHB1PeriphClockCmd(LCD_2X16[lcd_pin].TLCD_CLK, ENABLE);

        //Configuracion como salidas digitales:
        GPIO_InitStructure.GPIO_Pin = LCD_2X16[lcd_pin].TLCD_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(LCD_2X16[lcd_pin].TLCD_PORT, &GPIO_InitStructure);

        //Default Wert einstellen
        if(LCD_2X16[lcd_pin].TLCD_INIT == Bit_RESET)
            P_LCD_2x16_PinLo(lcd_pin, LCD_2X16);
        else
            P_LCD_2x16_PinHi(lcd_pin, LCD_2X16);
    }
}

/*Cambio de estados del LCD:*/
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16)
{
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRH = LCD_2X16[lcd_pin].TLCD_PIN;
}

void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16)
{
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRL = LCD_2X16[lcd_pin].TLCD_PIN;
}

/*Retardo LCD:*/
void P_LCD_2x16_Delay(volatile uint32_t nCount)
{
  while(nCount--)
  {
  }
}

/*Inicializacion secuencia LCD:*/
void P_LCD_2x16_InitSequenz(LCD_2X16_t* LCD_2X16)
{
  //Inicializacion de la secuencia:
  P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16);
  P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  // Erster Init Impuls
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // Zweiter Init Impuls
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // Dritter Init Impuls
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // LCD-Modus einstellen (4Bit-Mode)
  P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
}

/*Configuracion CLOCK LCD:*/
void P_LCD_2x16_Clk(LCD_2X16_t* LCD_2X16)
{
  // Pin-E auf Hi
  P_LCD_2x16_PinHi(TLCD_E, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
  // Pin-E auf Lo
  P_LCD_2x16_PinLo(TLCD_E, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
}

/*Comandos LCD:*/
void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t* LCD_2X16)
{
  // RS=Lo (Command)
  P_LCD_2x16_PinLo(TLCD_RS, LCD_2X16);
  // Hi-Nibble ausgeben
  if((wert&0x80)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x40)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x20)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x10)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  // Lo-Nibble ausgeben
  if((wert&0x08)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x04)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x02)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x01)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
}

void P_LCD_2x16_Cursor(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y)
{
  uint8_t wert;

  if(x>=TLCD_MAXX) x=0;
  if(y>=TLCD_MAXY) y=0;

  wert=(y<<6);
  wert|=x;
  wert|=0x80;
  P_LCD_2x16_Cmd(wert,LCD_2X16);
}

/*Analisis de datos LCD:*/
void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t* LCD_2X16)
{
  // RS=Hi (Data)
  P_LCD_2x16_PinHi(TLCD_RS, LCD_2X16);
  // Hi-Nibble ausgeben
  if((wert&0x80)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x40)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x20)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x10)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  // Lo-Nibble ausgeben
  if((wert&0x08)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x04)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x02)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x01)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
}

/*Inicializacion de los pines del LCD:*/
void INIT_LCD_2x16(LCD_2X16_t* LCD_2X16)
{
    //Inicialización de los pines del LCD:
    P_LCD_2x16_InitIO(LCD_2X16);
    // kleine Pause
    P_LCD_2x16_Delay(TLCD_INIT_PAUSE);
    // Init Sequenz starten
    P_LCD_2x16_InitSequenz(LCD_2X16);
    // LCD-Settings einstellen
    P_LCD_2x16_Cmd(TLCD_CMD_INIT_DISPLAY, LCD_2X16);
    P_LCD_2x16_Cmd(TLCD_CMD_ENTRY_MODE, LCD_2X16);
    // Display einschalten
    P_LCD_2x16_Cmd(TLCD_CMD_DISP_M1, LCD_2X16);
    // Display l�schen
    P_LCD_2x16_Cmd(TLCD_CMD_CLEAR, LCD_2X16);
    // kleine Pause
    P_LCD_2x16_Delay(TLCD_PAUSE);
}

/*Refresco de la pantalla del LCD:*/
void CLEAR_LCD_2x16(LCD_2X16_t* LCD_2X16)
{
  // Display l�schen
  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_PAUSE);
}

/*Impresion en la pantalla del LCD:*/
void PRINT_LCD_2x16(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y, char *ptr)
{
  // Cursor setzen
  P_LCD_2x16_Cursor(LCD_2X16,x,y);
  // kompletten String ausgeben
  while (*ptr != 0) {
    P_LCD_2x16_Data(*ptr, LCD_2X16);
    ptr++;
  }
}

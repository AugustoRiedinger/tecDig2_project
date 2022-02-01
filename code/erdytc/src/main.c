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
/*Definicion de pines del LCD:*/
LCD_2X16_t LCD_2X16[] = {
    /* Name  , PORT ,     PIN    ,         CLOCK       ,   Init    */
    { TLCD_RS, GPIOE, GPIO_Pin_13, RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_E,  GPIOF, GPIO_Pin_15, RCC_AHB1Periph_GPIOF, Bit_RESET },
    { TLCD_D4, GPIOF, GPIO_Pin_13, RCC_AHB1Periph_GPIOF, Bit_RESET },
    { TLCD_D5, GPIOE, GPIO_Pin_9,  RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_D6, GPIOE, GPIO_Pin_11, RCC_AHB1Periph_GPIOE, Bit_RESET },
    { TLCD_D7, GPIOF, GPIO_Pin_14, RCC_AHB1Periph_GPIOF, Bit_RESET },};

/*Pulsadores:*/
#define _F1 GPIOC
#define _f1 GPIO_Pin_9
#define _F2 GPIOB
#define _f2 GPIO_Pin_8
#define _C1 GPIOC
#define _c1 GPIO_Pin_6
#define _C2 GPIOC
#define _c2 GPIO_Pin_8

/* - - - - PARAMS. - - - -*/
/*Longitud general de buffers:*/
#define buffLen 20

/*Agotamiento de cuenta del TIM3:*/
#define freqTIM3 4

/* * * * * * * * * * * * * VAR. GLOBAL * * * * * * * * * * * * */
/* - - - -   EXTI   - - - -*/
/*Switch temperatura:*/
uint8_t switchTemp  = 0;

/*Switch SD:*/
uint8_t switchSD    = 0;

/*Switch servo:*/
uint8_t switchServo = 0;

/*Switch extra:*/
uint8_t switchMenu = 0;

/* - - - -   RF    - - - -*/
/*Temperatura en grados:*/
float tempDeg = 0;

/*Temperatura en valor digital:*/
uint32_t tempDig = 0;

/* - - - -   LCD   - - - -*/
/*Pantalla inicial:*/
uint8_t initialScreen = 1;

/*Contador de 5 segundos:*/
uint8_t fiveSecDelay = 0;

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

/*Inicializacion de salida digital:*/
void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin);

/*Inicializacion TIM3:*/
void INIT_TIM3(uint32_t freq);

/*Configuracion de nterrupciones externas:*/
void INIT_EXTINT(GPIO_TypeDef* Port, uint16_t Pin);
uint8_t FIND_EXTI_PORT_SOURCE(GPIO_TypeDef* Port);
uint8_t FIND_EXTI_PIN_SOURCE(uint32_t Pin);
uint32_t FIND_EXTI_LINE(uint32_t Pin);
uint32_t FIND_EXTI_HANDLER(uint32_t Pin);

/*Encontrar CLOCK:*/
uint32_t FIND_CLOCK(GPIO_TypeDef* Port);

/*----------------------------------------------------------------*/
/*MAIN:                                                           */
/*----------------------------------------------------------------*/
int main(void){

/* * * * * * * * * * * * * INICIALIZ. * * * * * * * * * * * * */
    /*Inicio del sistema:*/
    SystemInit();

    /*Inicializacion del TIM3:*/
    INIT_TIM3(freqTIM3);

    /*Inicializacion interrupciones por pulso externo:*/
    INIT_EXTINT(_C1,_c1);
    INIT_EXTINT(_C2,_c2);
    INIT_DO(_F1,_f1);
    INIT_DO(_F2,_f2);
    GPIO_SetBits(_F1, _f1);

/* * * * * * * * * * * * * BUCLE PPAL. * * * * * * * * * * * * */
  while (1)
  {
  }
}

/*----------------------------------------------------------------*/
/*INTERRUPCIONES:                                                 */
/*----------------------------------------------------------------*/

/*Interrupcion por vencimiento de cuenta de TIM3 cada 1/FS:*/
void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {

        /*Buffer para mostrar el valor de temperatura:*/
        char BuffTemp[buffLen];

        /*Refresco LCD:*/
        CLEAR_LCD_2x16(LCD_2X16);

        /*Pantalla incial:*/
        if(initialScreen == 1){
            /*Algoritmo para mostrar el mensaje:*/
            PRINT_LCD_2x16(LCD_2X16, 2, 0, "TD II-ERDYTC");
            sprintf(BuffTemp, "%.1f", tempDeg);
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
        else{
            initialScreen   = 1;
            switchTemp      = 0;
            switchSD        = 0;
            switchServo     = 0;
            switchMenu      = 0;
        }

        /*Rehabilitacion del timer:*/
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

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

/*----------------------------------------------------------------*/
/*FUNCIONES LOCALES:                                              */
/*----------------------------------------------------------------*/

void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin)
{
    /*Estructura de configuracion:*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*Habilitacion de la senal de reloj para el periferico:*/
    RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port), ENABLE);

    /*Se configura el pin como entrada (GPI0_MODE_IN):*/
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

    /*Se aplica la configuracion definida anteriormente al puerto:*/
    GPIO_Init(Port, &GPIO_InitStructure);
}

uint32_t FIND_CLOCK(GPIO_TypeDef* Port)
{
    uint32_t Clock;

    if      (Port == GPIOA) Clock = RCC_AHB1Periph_GPIOA;
    else if (Port == GPIOB) Clock = RCC_AHB1Periph_GPIOB;
    else if (Port == GPIOC) Clock = RCC_AHB1Periph_GPIOC;
    else if (Port == GPIOD) Clock = RCC_AHB1Periph_GPIOD;
    else if (Port == GPIOE) Clock = RCC_AHB1Periph_GPIOE;
    else if (Port == GPIOF) Clock = RCC_AHB1Periph_GPIOF;
    else if (Port == GPIOG) Clock = RCC_AHB1Periph_GPIOG;
    return Clock;
}

/*Inicializacion del TIM3:*/
void INIT_TIM3(uint32_t Freq)
{

    /*Habilitacion del clock para el TIM3:*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /*Habilitacion de la interrupcion por agotamiento de cuenta del TIM3:*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*Actualización de los valores del TIM3:*/
    SystemCoreClockUpdate();
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM3, DISABLE);

    /*Definicion de la base de tiempo:*/
    uint32_t TimeBase = 200e3;

    /*Computar el valor del preescaler en base a la base de tiempo:*/
    uint16_t PrescalerValue = 0;
    PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TimeBase) - 1;

    /*Configuracion del tiempo de trabajo en base a la frecuencia:*/
    TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /*Habilitacion de la interrupcion:*/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /*Habilitacion del contador:*/
    TIM_Cmd(TIM3, ENABLE);
}

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

void INIT_EXTINT(GPIO_TypeDef* Port, uint16_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /*Enable GPIO clock:*/
    uint32_t Clock;
    Clock = FIND_CLOCK(Port);
    RCC_AHB1PeriphClockCmd(Clock, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_Init(Port, &GPIO_InitStructure);

    /* Connect EXTI Line to pin */
    SYSCFG_EXTILineConfig(FIND_EXTI_PORT_SOURCE(Port), FIND_EXTI_PIN_SOURCE(Pin));

    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line = FIND_EXTI_LINE(Pin);
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = FIND_EXTI_HANDLER(Pin);
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

uint8_t FIND_EXTI_PORT_SOURCE(GPIO_TypeDef* Port)
{
    if (Port == GPIOA)      return EXTI_PortSourceGPIOA;
    else if (Port == GPIOB) return EXTI_PortSourceGPIOB;
    else if (Port == GPIOC) return EXTI_PortSourceGPIOC;
    else if (Port == GPIOD) return EXTI_PortSourceGPIOD;
    else if (Port == GPIOE) return EXTI_PortSourceGPIOE;
    else if (Port == GPIOF) return EXTI_PortSourceGPIOF;
    else                    return 0;
}

uint8_t FIND_EXTI_PIN_SOURCE(uint32_t Pin)
{
    if (Pin == GPIO_Pin_0)          return EXTI_PinSource0;
    else if (Pin == GPIO_Pin_1)     return EXTI_PinSource1;
    else if (Pin == GPIO_Pin_1)     return EXTI_PinSource1;
    else if (Pin == GPIO_Pin_2)     return EXTI_PinSource2;
    else if (Pin == GPIO_Pin_3)     return EXTI_PinSource3;
    else if (Pin == GPIO_Pin_4)     return EXTI_PinSource4;
    else if (Pin == GPIO_Pin_5)     return EXTI_PinSource5;
    else if (Pin == GPIO_Pin_6)     return EXTI_PinSource6;
    else if (Pin == GPIO_Pin_7)     return EXTI_PinSource7;
    else if (Pin == GPIO_Pin_8)     return EXTI_PinSource8;
    else if (Pin == GPIO_Pin_9)     return EXTI_PinSource9;
    else if (Pin == GPIO_Pin_10)    return EXTI_PinSource10;
    else if (Pin == GPIO_Pin_11)    return EXTI_PinSource11;
    else if (Pin == GPIO_Pin_12)    return EXTI_PinSource12;
    else if (Pin == GPIO_Pin_13)    return EXTI_PinSource13;
    else if (Pin == GPIO_Pin_14)    return EXTI_PinSource14;
    else                            return 0;
}

uint32_t FIND_EXTI_LINE(uint32_t Pin)
{
    if (Pin == GPIO_Pin_0)          return EXTI_Line0;
    else if (Pin == GPIO_Pin_1)     return EXTI_Line1;
    else if (Pin == GPIO_Pin_2)     return EXTI_Line2;
    else if (Pin == GPIO_Pin_3)     return EXTI_Line3;
    else if (Pin == GPIO_Pin_4)     return EXTI_Line4;
    else if (Pin == GPIO_Pin_5)     return EXTI_Line5;
    else if (Pin == GPIO_Pin_6)     return EXTI_Line6;
    else if (Pin == GPIO_Pin_7)     return EXTI_Line7;
    else if (Pin == GPIO_Pin_8)     return EXTI_Line8;
    else if (Pin == GPIO_Pin_9)     return EXTI_Line9;
    else if (Pin == GPIO_Pin_10)    return EXTI_Line10;
    else if (Pin == GPIO_Pin_11)    return EXTI_Line11;
    else if (Pin == GPIO_Pin_12)    return EXTI_Line12;
    else if (Pin == GPIO_Pin_13)    return EXTI_Line13;
    else if (Pin == GPIO_Pin_14)    return EXTI_Line14;
    else if (Pin == GPIO_Pin_15)    return EXTI_Line15;
    else                            return 0;
}

uint32_t FIND_EXTI_HANDLER(uint32_t Pin)
{
    if (Pin == GPIO_Pin_0)          return EXTI0_IRQn;
    else if (Pin == GPIO_Pin_1)     return EXTI1_IRQn;
    else if (Pin == GPIO_Pin_2)     return EXTI2_IRQn;
    else if (Pin == GPIO_Pin_3)     return EXTI3_IRQn;
    else if (Pin == GPIO_Pin_4)     return EXTI4_IRQn;
    else if (Pin == GPIO_Pin_5  ||
             Pin == GPIO_Pin_5  ||
             Pin == GPIO_Pin_7  ||
             Pin == GPIO_Pin_8  ||
             Pin == GPIO_Pin_9)     return EXTI9_5_IRQn;
    else if (Pin == GPIO_Pin_10 ||
             Pin == GPIO_Pin_11 ||
             Pin == GPIO_Pin_12 ||
             Pin == GPIO_Pin_13 ||
             Pin == GPIO_Pin_14 ||
             Pin == GPIO_Pin_15)    return EXTI15_10_IRQn;
    else                            return 0;
}

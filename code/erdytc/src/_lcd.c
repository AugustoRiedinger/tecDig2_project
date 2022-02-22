#include "_lcd.h"

void LCD(void)
{
    /*Refresco LCD:*/
    CLEAR_LCD_2x16(LCD_2X16);

    /*Pantalla incial:*/
    if(initialScreen == 1){
        INITIAL_SCREEN();
    }

    /*Pantalla actualizar temperatura - pulsador S1:*/
    else if(switchTemp == 1 && fiveSecDelay <= 20){
        UPDATE_TEMP();
    }

    /*Pantalla SERVO posicion 2- pulsador S2:*/
    else if(switchServo1 == 1 && fiveSecDelay <= 20){
        MOVE_SERVO_2();
    }

    /*Pantalla mover servo posicion 1 - pulsador S3:*/
    else if(switchServo2 == 1 && fiveSecDelay <= 20){
        MOVE_SERVO_1();
    }

    /*Pantalla datos guardados SD - pulsador S4:*/
    else if(switchSD == 1 && fiveSecDelay <= 20){
        SD();
    }

    /*Reseteo variables:*/
    else {
        initialScreen   = 1;
        fiveSecDelay    = 0;
    }

    /*Actualizar estado teclado matricial:*/
    UPDATE_SWITCH();

    /*Rehabilitacion del timer:*/
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

/*Inicializacion LCD:*/
void INIT_LCD_2x16(LCD_2X16_t *LCD_2X16) {
  // Inicialización de los pines del LCD:
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

/*Refresco LCD:*/
void CLEAR_LCD_2x16(LCD_2X16_t *LCD_2X16) {
  // Display l�schen
  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_PAUSE);
}

/*Imprimir en pantalla LCD:*/
void PRINT_LCD_2x16(LCD_2X16_t *LCD_2X16, uint8_t x, uint8_t y, char *ptr) {
  // Cursor setzen
  P_LCD_2x16_Cursor(LCD_2X16, x, y);
  // kompletten String ausgeben
  while (*ptr != 0) {
    P_LCD_2x16_Data(*ptr, LCD_2X16);
    ptr++;
  }
}

// LCD:
void P_LCD_2x16_InitIO(LCD_2X16_t *LCD_2X16) {
  GPIO_InitTypeDef GPIO_InitStructure;
  TLCD_NAME_t lcd_pin;

  for (lcd_pin = 0; lcd_pin < TLCD_ANZ; lcd_pin++) {
    // Habilitacion del Clock para cada PIN:
    RCC_AHB1PeriphClockCmd(LCD_2X16[lcd_pin].TLCD_CLK, ENABLE);

    // Configuracion como salidas digitales:
    GPIO_InitStructure.GPIO_Pin = LCD_2X16[lcd_pin].TLCD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_2X16[lcd_pin].TLCD_PORT, &GPIO_InitStructure);

    // Default Wert einstellen
    if (LCD_2X16[lcd_pin].TLCD_INIT == Bit_RESET)
      P_LCD_2x16_PinLo(lcd_pin, LCD_2X16);
    else
      P_LCD_2x16_PinHi(lcd_pin, LCD_2X16);
  }
}

void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t *LCD_2X16) {
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRH = LCD_2X16[lcd_pin].TLCD_PIN;
}

void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t *LCD_2X16) {
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRL = LCD_2X16[lcd_pin].TLCD_PIN;
}

void P_LCD_2x16_Delay(volatile uint32_t nCount) {
  while (nCount--) {
  }
}

void P_LCD_2x16_InitSequenz(LCD_2X16_t *LCD_2X16) {
  // Inicializacion de la secuencia:
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

void P_LCD_2x16_Clk(LCD_2X16_t *LCD_2X16) {
  // Pin-E auf Hi
  P_LCD_2x16_PinHi(TLCD_E, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
  // Pin-E auf Lo
  P_LCD_2x16_PinLo(TLCD_E, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
}

void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t *LCD_2X16) {
  // RS=Lo (Command)
  P_LCD_2x16_PinLo(TLCD_RS, LCD_2X16);
  // Hi-Nibble ausgeben
  if ((wert & 0x80) != 0)
    P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if ((wert & 0x40) != 0)
    P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if ((wert & 0x20) != 0)
    P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if ((wert & 0x10) != 0)
    P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  // Lo-Nibble ausgeben
  if ((wert & 0x08) != 0)
    P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if ((wert & 0x04) != 0)
    P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if ((wert & 0x02) != 0)
    P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if ((wert & 0x01) != 0)
    P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
}

void P_LCD_2x16_Cursor(LCD_2X16_t *LCD_2X16, uint8_t x, uint8_t y) {
  uint8_t wert;

  if (x >= TLCD_MAXX)
    x = 0;
  if (y >= TLCD_MAXY)
    y = 0;

  wert = (y << 6);
  wert |= x;
  wert |= 0x80;
  P_LCD_2x16_Cmd(wert, LCD_2X16);
}

void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t *LCD_2X16) {
  // RS=Hi (Data)
  P_LCD_2x16_PinHi(TLCD_RS, LCD_2X16);
  // Hi-Nibble ausgeben
  if ((wert & 0x80) != 0)
    P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if ((wert & 0x40) != 0)
    P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if ((wert & 0x20) != 0)
    P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if ((wert & 0x10) != 0)
    P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  // Lo-Nibble ausgeben
  if ((wert & 0x08) != 0)
    P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if ((wert & 0x04) != 0)
    P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if ((wert & 0x02) != 0)
    P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if ((wert & 0x01) != 0)
    P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16);
  else
    P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
}

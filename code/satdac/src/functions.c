#include "functions.h"

/*------------------------------------------------------------------------------
DECLARACION DE FUNCIONES INTERNAS:
------------------------------------------------------------------------------*/
/*Encontrar el CLOCK de un determinado pin:*/
uint32_t FIND_CLOCK(GPIO_TypeDef *Port);

/*Control del ADC:*/
ADC_TypeDef *FIND_ADC_TYPE(GPIO_TypeDef *Port, uint32_t Pin);
uint32_t FIND_RCC_APB(ADC_TypeDef *ADCX);
uint8_t FIND_CHANNEL(GPIO_TypeDef *Port, uint32_t Pin);

// LCD:
void P_LCD_2x16_InitIO(LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Delay(volatile uint32_t nCount);
void P_LCD_2x16_InitSequenz(LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Clk(LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t *LCD_2X16);
void P_LCD_2x16_Cursor(LCD_2X16_t *LCD_2X16, uint8_t x, uint8_t y);
void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t *LCD_2X16);

/*****************************************************************************
INIT_DO

        * @author	A. Riedinger.
        * @brief	Inicialiiza salidas digitales.
        * @returns	void
        * @param
                - Port	Puerto del pin a inicializar. Ej: GPIOX.
                - Clock Clock del pin a inicializar. Ej: RCC_AHB1Periph_GPIOX.
                - Pin	Pin a inicializar. Ej: GPIO_Pin_X
        * @ej
                - INIT_DO(GPIOX, RCC_AHB1Periph_GPIOX, GPIO_Pin_X);
******************************************************************************/
void INIT_DO(GPIO_TypeDef *Port, uint32_t Pin) {
  // Estructura de configuracion
  GPIO_InitTypeDef GPIO_InitStructure;

  // Habilitacion de la senal de reloj para el periferico:
  uint32_t Clock;
  Clock = FIND_CLOCK(Port);
  RCC_AHB1PeriphClockCmd(Clock, ENABLE);

  // Se configura el pin como entrada (GPI0_MODE_IN):
  GPIO_InitStructure.GPIO_Pin = Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  // Se aplica la configuracion definida anteriormente al puerto:
  GPIO_Init(Port, &GPIO_InitStructure);
}

/*****************************************************************************
INIT_SYSTICK

        * @author	A. Riedinger.
        * @brief	Inicializa la Interrupcion por tiempo definido por el
usuario.
        * @returns	void
        * @param
                - seg	Tiempo en segundos a los que se realizara la
interrupcion.

        * @ej
                - INIT_SYSTICK(1/1000); //Interrupcion cada 1 mseg.
******************************************************************************/
void INIT_SYSTICK(float div) {
  SysTick_Config(SystemCoreClock * div);
  RCC_ClocksTypeDef Clocks_Values;
  RCC_GetClocksFreq(&Clocks_Values);
}

/*****************************************************************************
INIT_ADC

        * @author	Catedra UTN-BHI TDII / A. Riedinger.
        * @brief	Inicializa una entrada analogica como ADC.
        * @returns	void
        * @param
                - Port		Puerto del ADC a inicializar. Ej: GPIOX.
                - Pin		Pin del ADC a inicializar. Ej: GPIO_Pin_X
        * @ej
                - INIT_ADC(GPIOX, GPIO_Pin_X);
******************************************************************************/
void INIT_ADC1DMA(uint16_t *adcArray, uint32_t bufferSize) {
  /*Habilitacion del clock de los perficos del DMA y el ADC1::*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /*Inicio configuracion DMA2:*/
  DMA_DeInit(DMA2_Stream0);
  /*Se elige DMA canal 0 para trabajar con ADC1:*/
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  /*Registro donde se guardan los valores convertidos:*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  /*Direccion del arreglo donde se guardan los valores leidos:*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)adcArray;
  /*Establecer que el DMA transmitira a memoria:*/
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  /*Establecer la cantidad de valores a convertir:*/
  DMA_InitStructure.DMA_BufferSize = bufferSize;
  /*Se deshabilita el incremento de memoria por periferico:*/
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /*Se habilita el incremento de memoria para crear un arreglo dinamico:*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /*Ancho de bit del dato:*/
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  /*Ancho de cada elemento del arreglo:*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  /*Modo circuilar, cuando llega al final del arreglo vuelve a arrancar desde el
   * primer valor:*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  /*Maxima prioridad al DMA:*/
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  /*Configuracion del handler para la interrupcion:*/
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

  /*Habilitacion del DMA:*/
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /*Inicio configuracion del ADC1:*/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  /*Se habilita el acceso del DMA:*/
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

  /*Cargar la configuracion:*/
  ADC_CommonInit(&ADC_CommonInitStructure);

  /*Resolucion del ADC en 12 bits:*/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  /*Habilita la conversion de varios canales simultaneamente:*/
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  /*Se deshabilita el modo continuo, ya que el ADC se va a activar por el
   * TIM3:*/
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  /*Disparo por flanco ascendente:*/
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  /*Define el TMRGO del TIM3 como disparo del ADC:*/
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  /*Cantidad de datos que se van a convertir por disparo del ADC (tension y
   * corriente):*/
  ADC_InitStructure.ADC_NbrOfConversion = 2;

  /*Inicializacion del ADC con los parametros establecidos:*/
  ADC_Init(ADC1, &ADC_InitStructure);

  /*Configuracion de los canales del ADC:*/
  /*Canal 10 ADC1 (PC0) con orden 1 para conversion de tension:*/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);
  /*Canal 13 ADC1 (PC13) con orden 2 para conversion de corriente:*/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 2, ADC_SampleTime_480Cycles);
  /*Habilitacion del pedido del DMA en el ADC1:*/
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /* PC1 para entrada analógica */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /*Habilitacion del ADC1:*/
  ADC_Cmd(ADC1, ENABLE);
}

/*****************************************************************************
INIT_TIM3

        * @author	A. Riedinger.
        * @brief	Inicializa las interrupciones del TIM3.
        * @returns	void
        * @param
        * @ej
                - INIT_TIM3();
******************************************************************************/
void INIT_TIM3(uint32_t Freq) {
  /*Habilitacion del clock del TIM3:*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /*Habilitacion de la interrupcion por vencimiento de cuenta del TIM3:*/
  //	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  //	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  //	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //	NVIC_Init(&NVIC_InitStructure);

  /*Computacion del valor del preescaler:*/
  uint32_t TimeBase = 200e3;
  uint16_t PrescalerValue = (uint16_t)((SystemCoreClock / 2) / TimeBase) - 1;

  /*Configuracion del tiempo de interrupcion:*/
  TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Selección de TIM3 TRGO */
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
  TIM_Cmd(TIM3, ENABLE);
}

/*****************************************************************************
INIT_LCD_2x16

        * @author	A. Riedinger.
        * @brief	Inicializa los pines del display.
        * @returns	void
        * @param
                - LCD_2x16	Arreglo tipo LCD_2X16_t con los pines del LCD.
Ej: LCD_2X16_t LCD_2X16[] = {
                                        // Name  , PORT,    PIN     , CLOCK ,
Init {TLCD_RS ,GPIOC,GPIO_Pin_10 ,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_E
,GPIOC,GPIO_Pin_11 ,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_D4 ,GPIOC,GPIO_Pin_12
,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_D5 ,GPIOD,GPIO_Pin_2
,RCC_AHB1Periph_GPIOD,Bit_RESET}, {TLCD_D6 ,GPIOF,GPIO_Pin_9
,RCC_AHB1Periph_GPIOF,Bit_RESET}, {TLCD_D7 ,GPIOF,GPIO_Pin_7
,RCC_AHB1Periph_GPIOF,Bit_RESET},};
        * @ej
                - INIT_LCD_2x16(LCD_2X16);
******************************************************************************/
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

/*****************************************************************************
CLEAR_LCD_2x16

        * @author	A. Riedinger.
        * @brief	Refresca la pantalla del LCD.
        * @returns	void
        * @param
                - LCD_2x16	Arreglo tipo LCD_2X16_t con los pines del LCD.
Ej: LCD_2X16_t LCD_2X16[] = {
                                        // Name  , PORT,    PIN     , CLOCK ,
Init {TLCD_RS ,GPIOC,GPIO_Pin_10 ,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_E
,GPIOC,GPIO_Pin_11 ,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_D4 ,GPIOC,GPIO_Pin_12
,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_D5 ,GPIOD,GPIO_Pin_2
,RCC_AHB1Periph_GPIOD,Bit_RESET}, {TLCD_D6 ,GPIOF,GPIO_Pin_9
,RCC_AHB1Periph_GPIOF,Bit_RESET}, {TLCD_D7 ,GPIOF,GPIO_Pin_7
,RCC_AHB1Periph_GPIOF,Bit_RESET},};
        * @ej
                - UB_LCD_2x16_Clear(LCD_2X16);
******************************************************************************/
void CLEAR_LCD_2x16(LCD_2X16_t *LCD_2X16) {
  // Display l�schen
  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_PAUSE);
}

/*****************************************************************************
PRINT_LCD_2x16

        * @author	A. Riedinger.
        * @brief	Imprime una string en el LCD.
        * @returns	void
        * @param
                - LCD_2x16	Arreglo tipo LCD_2X16_t con los pines del LCD.
Ej: LCD_2X16_t LCD_2X16[] = {
                                        // Name  , PORT,    PIN     , CLOCK ,
Init {TLCD_RS ,GPIOC,GPIO_Pin_10 ,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_E
,GPIOC,GPIO_Pin_11 ,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_D4 ,GPIOC,GPIO_Pin_12
,RCC_AHB1Periph_GPIOC,Bit_RESET}, {TLCD_D5 ,GPIOD,GPIO_Pin_2
,RCC_AHB1Periph_GPIOD,Bit_RESET}, {TLCD_D6 ,GPIOF,GPIO_Pin_9
,RCC_AHB1Periph_GPIOF,Bit_RESET}, {TLCD_D7 ,GPIOF,GPIO_Pin_7
,RCC_AHB1Periph_GPIOF,Bit_RESET},};
                - x			Indicador de fila. Ej: 0 ... 1.
                - y 		Indicador de columna. Ej: 0 .. 16.
                - ptr		Puntero a la string a imprimir.

        * @ej
                - PRINT_LCD_2x16(LCD_2X16, 0, 0, STR);
******************************************************************************/
void PRINT_LCD_2x16(LCD_2X16_t *LCD_2X16, uint8_t x, uint8_t y, char *ptr) {
  // Cursor setzen
  P_LCD_2x16_Cursor(LCD_2X16, x, y);
  // kompletten String ausgeben
  while (*ptr != 0) {
    P_LCD_2x16_Data(*ptr, LCD_2X16);
    ptr++;
  }
}

void INIT_USART_RX_TX(GPIO_TypeDef* Port1, uint16_t Pin1, GPIO_TypeDef* Port2, uint16_t Pin2, uint32_t BaudRate)
{
    /*USART clock enable:*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /*GPIO clock enable:*/
    RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port1), ENABLE);
    RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port2), ENABLE);

    /*GPIO Configuration:*/
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = Pin1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Port1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Pin2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Port2, &GPIO_InitStructure);

    /*Connect USART pins to AF:*/
    GPIO_PinAFConfig(Port1, FIND_PINSOURCE(Pin1), GPIO_AF_USART2);
    GPIO_PinAFConfig(Port2, FIND_PINSOURCE(Pin2), GPIO_AF_USART2);

    /*USARTx configuration:*/
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
}


/*------------------------------------------------------------------------------
 FUNCIONES INTERNAS:
------------------------------------------------------------------------------*/
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

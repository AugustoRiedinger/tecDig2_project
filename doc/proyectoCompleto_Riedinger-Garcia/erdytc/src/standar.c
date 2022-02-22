#include "standar.h"

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

/*Encontrar clock de determinado pin:*/
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

/*Encontrar PinSource de determinado pin:*/
uint8_t FIND_PINSOURCE(uint32_t Pin)
{
    if     (Pin == GPIO_Pin_0)  return GPIO_PinSource0;
    else if(Pin == GPIO_Pin_1)  return GPIO_PinSource1;
    else if(Pin == GPIO_Pin_2)  return GPIO_PinSource2;
    else if(Pin == GPIO_Pin_3)  return GPIO_PinSource3;
    else if(Pin == GPIO_Pin_4)  return GPIO_PinSource4;
    else if(Pin == GPIO_Pin_5)  return GPIO_PinSource5;
    else if(Pin == GPIO_Pin_6)  return GPIO_PinSource6;
    else if(Pin == GPIO_Pin_7)  return GPIO_PinSource7;
    else if(Pin == GPIO_Pin_8)  return GPIO_PinSource8;
    else if(Pin == GPIO_Pin_9)  return GPIO_PinSource9;
    else if(Pin == GPIO_Pin_10) return GPIO_PinSource10;
    else if(Pin == GPIO_Pin_11) return GPIO_PinSource11;
    else if(Pin == GPIO_Pin_12) return GPIO_PinSource12;
    else if(Pin == GPIO_Pin_13) return GPIO_PinSource13;
    else if(Pin == GPIO_Pin_14) return GPIO_PinSource14;
    else if(Pin == GPIO_Pin_15) return GPIO_PinSource15;
    else
         return 0;
}

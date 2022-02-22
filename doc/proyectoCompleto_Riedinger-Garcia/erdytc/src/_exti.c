#include "_exti.h"

/*Actualizar estado teclado matricial:*/
void UPDATE_SWITCH(void)
{
    /*Se apaga F1 para medir a continuacion S3 y S4 mediante F2:*/
    if (activeSwitch == 0)
    {
        GPIO_ResetBits(_F1, _f1);
        GPIO_SetBits(_F2, _f2);
        activeSwitch = 1;
    }
    else {
        GPIO_ResetBits(_F2, _f2);
        GPIO_SetBits(_F1, _f1);
        activeSwitch = 0;
    }
}

/*Inicializacion de puertos EXTI:*/
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

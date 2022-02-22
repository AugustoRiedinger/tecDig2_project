#include "_menus.h"

/*Pantalla inicial:*/
void INITIAL_SCREEN(void)
{
    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 2, 0, "TD II-ERDYTC");
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "Temp=");

    sprintf(buffTemp, "%c", DECODE_TEMP(receivedTemp));
}

/*Pantalla para mostrar mensaje de actualizacion de temperatura:*/
void UPDATE_TEMP(void)
{
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 2, 0, "ACTUALIZANDO");
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "TEMPERATURA");

    /*Pedir el valor de temperatura si pasaron 5 segundos:*/
    if (fiveSecDelay == 20)
        TEMP_CODE();
}

/*Pantalla para indicar que el servo se mueve a posicion 2:*/
void MOVE_SERVO_2(void)
{
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 1, 0, "SERVO POSICION");
    PRINT_LCD_2x16(LCD_2X16, 8, 1, "2");

    /*Guardar datos en SD si pasaron 5 segundos:*/
    if (fiveSecDelay == 20)
        SERVO_2();
}

/*Pantalla para indicar que el servo se mueve a posicion 1:*/
void MOVE_SERVO_1(void)
{
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 1, 0, "SERVO POSICION");
    PRINT_LCD_2x16(LCD_2X16, 8, 1, "1");

    /*Enviar comando servo si pasaron 5 segundos:*/
    if (fiveSecDelay == 20)
        SERVO_1();
}

/*Pantalla datos guardados SD:*/
void SD(void)
{
    /*Aumentar el contador de los 5 seg:*/
    fiveSecDelay++;

    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 1, 0, "DATOS GUARDADOS");
    PRINT_LCD_2x16(LCD_2X16, 5, 1, "EN SD");

    /*Enviar comando servo si pasaron 5 segundos:*/
    if (fiveSecDelay == 20)
        SD();
}

#include "_menus.h"

/*Pantalla inicial:*/
void INITIAL_SCREEN(void)
{
    /*Algoritmo para mostrar el mensaje:*/
    PRINT_LCD_2x16(LCD_2X16, 2, 0, "TD II-ERDYTC");
    PRINT_LCD_2x16(LCD_2X16, 2, 1, "Temp=");

    sprintf(buffTemp1, "%c", temp1[0]);
    PRINT_LCD_2x16(LCD_2X16, 8, 1, buffTemp1);
    sprintf(buffTemp2, "%c", temp2[0]);
    PRINT_LCD_2x16(LCD_2X16, 9, 1, buffTemp2);
    sprintf(buffTemp3, "%c", temp3[0]);
    PRINT_LCD_2x16(LCD_2X16, 10, 1, buffTemp3);
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

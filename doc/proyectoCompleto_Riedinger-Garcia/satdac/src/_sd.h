#ifndef _SD_H_
#define _SD_H_

#include "stm32_ub_fatfs.h"

FIL myFile;   // File Handler
FRESULT archivo;

char fila1[16] = "Hello World";

void WRITE_SD();

#endif // _SD_H_

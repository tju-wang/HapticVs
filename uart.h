#ifndef _UART_H_
#define _UART_H_


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <windows.h>
#include "seriallib.h"
#include "kinematic.h"
#include "solveCoord.h"
#include "solveFbi.h"


BOOL SerialInit(HANDLE *hC);
BOOL Update_q(unsigned char *data,var_q *Lq,char *);
void Double2Char(double doub,unsigned char* val_1,unsigned char* val_2,unsigned char* val_3);
unsigned char dataCheck(unsigned char* buff,char len);
BOOL SendData(HANDLE hComm,double F1,double F2,double F3,unsigned int sort,unsigned char* wData);
void ErrorCheck(unsigned char err);
#endif




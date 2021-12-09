#ifndef UAPP_SENSER_H
#define UAPP_SENSER_H
#include "sys.h"



typedef struct
{
	short  Current;
	short  Voltage;
  short  Temp1;
	short  Temp2;
	short  TempA;
	short  AngleX;
	short  AngleY;
	short  AngleZ;
}SENSER_VAL;

typedef struct
{
	short* voltin;
	short* currin;
	long  voltacc;
	long  curracc;
	u16   cntnum;
	short* voltout;
  short* currout;	
	
}AVG_FILTER;

#define Filternum            50

#define Coeff_Adccurr        250*6/4096
#define Coeff_Adcvolt        250*6/4096

#define Currbias             10
#define Voltbias             10

void _SENSER_TASK1MS(void);
void _READ_SenserData(SENSER_VAL* Senval);
void _WRITE_SenserData(SENSER_VAL* Senval);

#endif






#ifndef __SHOWINTERFACE_H
#define __SHOWINTERFACE_H			  	 
#include "sys.h"
#include "stdlib.h"	

typedef struct
{
	int page;
	int PageChangeFlag;
	
}INTERFACE;
#define MaxPage			5
#define PageMain 		1
#define PageEquipment	2
#define Page4G		3
#define PageAbout		4



#define NoAlarm	0
#define AlarmSts 1

extern INTERFACE Interface;
int WhichInterface(void);
int IsInterfaceChange(void);
int InterfaceChangeFlag(void);

int ShowWarning(void);

#endif  
	 




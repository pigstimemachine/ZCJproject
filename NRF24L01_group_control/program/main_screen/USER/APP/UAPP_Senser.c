#include "lib.h"

SENSER_VAL   SenserVal,SenserOld;
AVG_FILTER   ADC_Filter;
extern unsigned short ADC_Buf[16];
short filter_1(short NEW_DATA,short OLD_DATA,short k)   
{
    long result;
    if(NEW_DATA<OLD_DATA)
    {
        result=OLD_DATA-NEW_DATA;
        result=result*k;
        result=result+128;
        result=result>>8;
        result=OLD_DATA-result;
    }
    else if(NEW_DATA>OLD_DATA)
    {
        result=NEW_DATA-OLD_DATA;
        result=result*k;
        result=result+128;
        result=result>>8;
        result=OLD_DATA+result;
    }
    else result=OLD_DATA;
    return((short)result);
}

void Averagefilter_task(AVG_FILTER *fliter,Uint16 Fperiod)    //1<<Fperiod
{
	fliter->voltacc+=*(fliter->voltin);
	fliter->curracc+=*(fliter->currin);
	fliter->cntnum++;
	if(fliter->cntnum>=(1<<Fperiod))
	{
		*(fliter->voltout)=fliter->voltacc>>Fperiod;
		*(fliter->currout)=fliter->curracc>>Fperiod;
		memset(fliter,0,sizeof(AVG_FILTER));
	}
}

void _SenserFilter_Task(void)    //1HZ
{
	  short Currdata,Voltdata;
    Currdata=Get_Adc_Average(ADC_CURR,1);
	  Voltdata=Get_Adc_Average(ADC_VOLT,1);
	  Currdata=(long)Currdata*Coeff_Adccurr;
	  Voltdata=(long)Voltdata*Coeff_Adcvolt;
	  Currdata-=Currbias;
	  Voltdata-=Voltbias;
	  if(Currdata<0) Currdata=0;
	  if(Voltdata<0) Voltdata=0;
	  ADC_Filter.currout=&SenserVal.Current;
	  ADC_Filter.voltout=&SenserVal.Voltage;
	  ADC_Filter.currin=&Currdata;
	  ADC_Filter.voltin=&Voltdata;
	  Averagefilter_task(&ADC_Filter,8);

//    SenserVal.Current= filter_1(Currdata,SenserOld.Current,Filternum);
//	  SenserOld.Current=Currdata;
//	  SenserVal.Voltage= filter_1(Voltdata,SenserOld.Voltage,Filternum);
//	  SenserOld.Voltage=Voltdata;
}	

void _READ_SenserData(SENSER_VAL* Senval)
{
    memcpy(Senval,&SenserVal,sizeof(SenserVal));
}	

void _WRITE_SenserData(SENSER_VAL* Senval)
{
    memcpy(&SenserVal,Senval,sizeof(SenserVal));
}	

void _SENSER_TASK1MS(void)
{
    _SenserFilter_Task();
}	

#include "oled.h"
#include "stdlib.h"
//#include "lcdfont.h"
#include "delay.h"
#include "ShowInterface.h"
#include "UAPP_CAN.h"
#include "key.h"
#include "pic.h"
#include "UAPP_Senser.h"
//#include "lib.h"
INTERFACE Interface;
int alarmsts;
enum  alarm
{
    noalarm=0,
    sts_cur,//最低位 最右位
    sts_vol,
    sts_SSspeed,
    sts_HJspeed,
    sts_LayerTemp,
    sts_BoxTemp,
    sts_Humidity,
} ALARM_PARA;


void ShowSignalStrength(void)
{
    LCD_ShowString(5,220,"4G:",RED,WHITE,16,0);
    //LCD_ShowChinese(23,220,"信号",RED,WHITE,16,0);
    //LCD_ShowString(23,220,":",RED,WHITE,16,0);
    LCDShowStringUpdate(30,220,CAN_DataSingalStrength.CodeData,His_AS4Data.DeviceData.SignalSrth,5,RED,WHITE,16,0);
    LCD_ShowString(57,220,"dBm",RED,WHITE,16,0);
    AS4Data.DeviceData.SignalStrength=atoi(CAN_DataSingalStrength.CodeData);
    if((AS4Data.DeviceData.SignalStrength >= (-85)) && (AS4Data.DeviceData.SignalStrength < 0))
        LCD_ShowChinese(85,220,"强",RED,WHITE,16,0);
    else if( (AS4Data.DeviceData.SignalStrength >=(-100)) && (AS4Data.DeviceData.SignalStrength <(-85)) )
        LCD_ShowChinese(85,220,"中",RED,WHITE,16,0);
    else if( (AS4Data.DeviceData.SignalStrength <(-100)) )
        LCD_ShowChinese(85,220,"弱",RED,WHITE,16,0);
    else
        LCD_ShowChinese(85,220,"无",GBLUE,WHITE,16,0);

    LCD_ShowString(105,220,"WiFi:",RED,WHITE,16,0);
    if( AS4Data.is_WiFi_connect == 1 )
    {
        LCD_ShowChinese(145,220,"强",RED,WHITE,16,0);
    }
    else
    {
        LCD_ShowChinese(145,220,"无",GBLUE,WHITE,16,0);
    }


}
void ShowTime(void)
{
    LCD_ShowString(165,220,CAN_DataTime.CodeData,RED,WHITE,16,0);
}


int ShowInterfaceMain(void)//1
{
    LCD_ShowChinese(5,20,"焊口",RED,WHITE,16,0);
    LCD_ShowString(35,20,":",RED,WHITE,16,0);
    //LCDShowStringUpdate(45,20,CAN_DataPipe.CodeData,His_AS4Data.WELD_CODE,60,RED,WHITE,16,0);//34 275 8
    LCDStrLineFeed(45,20,CAN_DataPipe.CodeData,His_AS4Data.WELD_CODE,His_AS4Data.WELD_CODE_Front,60,MAGENTA);

    LCD_ShowChinese(5,60,"焊工",RED,WHITE,16,0);
    LCD_ShowString(35,60,":",RED,WHITE,16,0);
    LCDStrLineFeed(45,60,CAN_DataWelder.CodeData,His_AS4Data.PERSON_CODE,His_AS4Data.PERSON_CODE_Front,60,MAGENTA);
    //LCDShowStringUpdate(45,60,CAN_DataWelder.CodeData,His_AS4Data.PERSON_CODE,60,RED,WHITE,16,0);






    LCD_ShowChinese(5,80,"规程",RED,WHITE,16,0);
    LCD_ShowString(35,80,":",RED,WHITE,16,0);
    LCDShowStringUpdate(45,80,CAN_DataProcess.CodeData,His_AS4Data.PROCESS,20,RED,WHITE,16,0);

    LCD_ShowChinese(215,80,"工艺",RED,WHITE,16,0);//200
    LCD_ShowString(245,80,":",RED,WHITE,16,0);//230
    LCDShowStringUpdate(255,80,CAN_DataHjProcess.CodeData,His_AS4Data.HJ_PROCESS,20,RED,WHITE,16,0);//245




    LCD_ShowChinese(5,100,"电压",RED,WHITE,16,0);
    LCD_ShowString(35,100,":",RED,WHITE,16,0);
    if(alarmsts==sts_vol)
    {
        LCD_ShowString(45,100,AS4Data.VOLTAGE,GBLUE,WHITE,16,0);
    }
    else
    {
        LCD_ShowString(45,100,AS4Data.VOLTAGE,RED,WHITE,16,0);
    }


    LCD_ShowChinese(5,120,"电流",RED,WHITE,16,0);
    LCD_ShowString(35,120,":",RED,WHITE,16,0);
    if(alarmsts==sts_cur)
    {
        LCD_ShowString(45,120,AS4Data.CURRENT,GBLUE,WHITE,16,0);
    }
    else
    {
        LCD_ShowString(45,120,AS4Data.CURRENT,RED,WHITE,16,0);
    }


    LCD_ShowChinese(5,170,"焊层",RED,WHITE,16,0);//235
    LCD_ShowString(35,170,":",RED,WHITE,16,0);//265
    LCDShowStringUpdate( 45,160,can_layer.layer_str,His_AS4Data.LAYER,5,BRED,WHITE,32,0); //275



    LCD_ShowChinese(110,100,"丝速",RED,WHITE,16,0);
    LCD_ShowString(140,100,":",RED,WHITE,16,0);
    LCD_ShowString(155,100,AS4Data.SS_SPEED,RED,WHITE,16,0);

    LCD_ShowChinese(110,120,"焊速",RED,WHITE,16,0);
    LCD_ShowString(140,120,":",RED,WHITE,16,0);
    LCD_ShowString(155,120,AS4Data.HJ_SPEED,RED,WHITE,16,0);

    LCD_ShowChinese(215,120,"层温",RED,WHITE,16,0);
    LCD_ShowString(245,120,":",RED,WHITE,16,0);
    if(alarmsts==sts_LayerTemp)
    {
        LCD_ShowIntNum(250,120,AS4Data.LayerTemp,3,GBLUE,WHITE,16);
    }
    else
    {
        LCD_ShowIntNum(250,120,AS4Data.LayerTemp,3,RED,WHITE,16);
    }




    LCD_ShowChinese(215,160,"湿度",RED,WHITE,16,0);
    LCD_ShowString(245,160,":",RED,WHITE,16,0);
    if(alarmsts==sts_Humidity)
    {
        LCD_ShowString(255,160,AS4Data.HUMIDITY,GBLUE,WHITE,16,0);
    }
    else
    {
        LCD_ShowString(255,160,AS4Data.HUMIDITY,RED,WHITE,16,0);
    }



    LCD_ShowChinese(215,140,"板温",RED,WHITE,16,0);
    LCD_ShowString(245,140,":",RED,WHITE,16,0);
    if(alarmsts == sts_BoxTemp)
    {
        LCD_ShowString(255,140,AS4Data.TEMP,GBLUE,WHITE,16,0);
    }
    else
    {
        LCD_ShowString(255,140,AS4Data.TEMP,RED,WHITE,16,0);
    }




    LCD_ShowChinese(215,100,"位置",RED,WHITE,16,0);
    LCD_ShowString(245,100,":",RED,WHITE,16,0);
    LCDShowStringUpdate( 255,100, AS4Data.POSITION, His_AS4Data.POSITION,10,RED, WHITE, 16, 0);



    LCD_ShowChinese(5,140,"机组",RED,WHITE,16,0);
    LCD_ShowString(35,140,":",RED,WHITE,16,0);
    LCDShowStringUpdate( 45,140, CAN_DataTeamCode.CodeData, His_AS4Data.TEAM_CODE,20,RED, WHITE, 16, 0);



    LCD_ShowChinese(5,200,"报警",RED,WHITE,16,0);
    LCD_ShowString(35,200,":",RED,WHITE,16,0);
    //LCDShowStringUpdate(45,200,CAN_DataWarning.CodeData,His_AS4Data.workExcep,40,GBLUE,WHITE,16,0);


    ShowTime();
    ShowSignalStrength();
    return 0;
}



/*该函数存在缺陷，当函数进入一个if语句后，就直接返回，导致报警始终不完整，存在漏报警的情况*/
int ShowWarning(void)
{
    LCD_ShowString(45,200,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",WHITE,WHITE,16,0);


    if( 0 == memcmp(CAN_DataWarning.His_CodeData, "BoxTempTooHigh!",sizeof("BoxTempTooHigh!")) )
    {
        LCD_ShowChinese(45,200,"板载温度过高！！！！！！！！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts = sts_BoxTemp ;
        return 5;
    }
    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "VoltageTooHigh!",sizeof("VoltageTooHigh!")) )
    {
        LCD_ShowChinese(45,200,"电压过高！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_vol ;
        return 2;
    }
    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "CurrentTooHigh!",sizeof("CurrentTooHigh!")) )
    {
        LCD_ShowChinese(45,200,"电流过高！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_cur ;
        return 1;
    }

    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "SSspeedTooLow!",sizeof("SSspeedTooLow!")) )
    {
        LCD_ShowChinese(45,200,"送丝速度过低！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_SSspeed  ;
        return 3;
    }
    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "HJspeedTooHigh!",sizeof("HJspeedTooHigh!")) )
    {
        //LCD_ShowString(45,200,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",WHITE,WHITE,16,0);
        LCD_ShowChinese(45,200,"焊接速度过高！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_HJspeed  ;
        return 4;
    }

    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "HumidityTooHigh!",sizeof("HumidityTooHigh!")) )
    {
        LCD_ShowChinese(45,200,"板载湿度过高！       ",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_Humidity ;
        return 6;
    }
    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "LayerTempErr!",sizeof("LayerTempErr!")) )
    {
        LCD_ShowChinese(45,200,"焊层温度错误！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_LayerTemp ;
        return 7;
    }
    else if( 0 == memcmp(CAN_DataWarning.His_CodeData, "LayerTempTooLow!",sizeof("LayerTempTooLow!")) )
    {
        LCD_ShowChinese(45,200,"焊层温度过低！",GBLUE,WHITE,16,0);
        memcpy( His_AS4Data.workExcep, CAN_DataWarning.His_CodeData, sizeof(His_AS4Data.workExcep));
        alarmsts=sts_LayerTemp ;
        return 8;
    }
    else
    {
        LCDShowStringUpdate(45,200,CAN_DataWarning.His_CodeData,His_AS4Data.workExcep,40,GBLUE,WHITE,16,0);
        alarmsts=noalarm;
        return 9;
    }
    return 0;
}


int ShowInterfaceEquipment(void)//2
{
    int AngleSts;
    AngleSts = weld_angle() ;
    LCD_ShowChinese(5,40,"设备",RED,WHITE,16,0);
    LCD_ShowString(35,40,":",RED,WHITE,16,0);
    LCDShowStringUpdate(45,40,CAN_DataBoxID.CodeData,His_AS4Data.DATASBOX_CODE,20,RED,WHITE,16,0);

    LCD_ShowChinese(5,20,"项目",RED,WHITE,16,0);
    LCD_ShowString(35,20,":",RED,WHITE,16,0);
    LCDShowStringUpdate(45,20,CAN_DataProjectID.CodeData,His_AS4Data.PROJECT_ID,30,RED,WHITE,16,0);


    LCD_ShowChinese(5,60,"焊机",RED,WHITE,16,0);
    LCD_ShowString(35,60,":",RED,WHITE,16,0);
    LCDShowStringUpdate(45,60,CAN_DataEquipCode.CodeData,His_AS4Data.EQUIP_CODE,20,RED,WHITE,16,0);

    LCD_ShowString(5,80,"AngleX",RED,WHITE,16,0);
    LCD_ShowString(55,80,":",RED,WHITE,16,0);
    LCDShowStringUpdate(65,80,SenserVal.AngleStrX,HisSenserVal.AngleStrX,5,RED,WHITE,16,0);

    LCD_ShowString(105,80,"AngleY",RED,WHITE,16,0);
    LCD_ShowString(155,80,":",RED,WHITE,16,0);
    LCDShowStringUpdate(165,80,SenserVal.AngleStrY,HisSenserVal.AngleStrY,5,RED,WHITE,16,0);

    LCD_ShowString(205,80,"AngleZ",RED,WHITE,16,0);
    LCD_ShowString(255,80,":",RED,WHITE,16,0);
    LCDShowStringUpdate(265,80,SenserVal.AngleStrZ,HisSenserVal.AngleStrZ,5,RED,WHITE,16,0);


    LCD_ShowChinese(5,100,"当前焊接坐标",RED,WHITE,16,0);
    LCD_ShowString(105,100,":",RED,WHITE,16,0);
    switch (AngleSts)
    {
    case _0clock_position:
        LCD_ShowChinese(115,100,"大概在零点钟方向",RED,WHITE,16,0);//大概在一点钟方向
        break;
    case _3clock_position:
        LCD_ShowChinese(115,100,"大概在三点钟方向",RED,WHITE,16,0);//大概在三点钟方向
        break;
    case _6clock_position:
        LCD_ShowChinese(115,100,"大概在六点钟方向",RED,WHITE,16,0);//大概在六点钟方向
        break;
    default:
        LCD_ShowChinese(115,100,"非正常焊接坐标点",RED,WHITE,16,0);//大概在六点钟方向
        break;
    }

    LCD_ShowString(5,120,"WiFi:",RED,WHITE,16,0);
    LCDShowStringUpdate(45,120,CAN_Datawlan0IP.CodeData, His_AS4Data.wlan0_IP,20,RED,WHITE,16,0);
	//LCDStrLineFeed(45,120,CAN_Datawlan0IP.CodeData,His_AS4Data.wlan0_IP,His_AS4Data.wlan0_IP_Front,20,MAGENTA);

    LCD_ShowString(5,140,"ssid:",RED,WHITE,16,0);
    //LCDShowStringUpdate(5,140,CAN_Data_WiFi_ssid.CodeData , His_AS4Data.WiFi_ssid,5,RED,WHITE,16,0);
    LCDStrLineFeed(45,140,CAN_Data_WiFi_ssid.CodeData,His_AS4Data.WiFi_ssid,His_AS4Data.WiFi_ssid_Front,100,MAGENTA);

    LCD_ShowString(5,180,"psk:",RED,WHITE,16,0);
    //LCDShowStringUpdate(5,180,CAN_Data_WiFi_psk.CodeData , His_AS4Data.WiFi_psk,5,RED,WHITE,16,0);
    LCDStrLineFeed(45,180,CAN_Data_WiFi_psk.CodeData,His_AS4Data.WiFi_psk,His_AS4Data.WiFi_psk_Front,100,MAGENTA);


    //LCD_ShowChinese(5,120"当前焊接",RED,WHITE,16,0);
    ShowSignalStrength();
    ShowTime();
    return 0;
}
int ShowInterface4G(void)//3
{
    //LCDStrLine3Feed(5, 120, CAN_DataCPSI.CodeData, His_AS4Data.DeviceData.CPSI, His_AS4Data.DeviceData.FrontCPSI,His_AS4Data.DeviceData.MiddleCPSI, 90);

    //LCDStrLineFeed(5, 20, CAN_DataCPSI.CodeData, His_AS4Data.DeviceData.CPSI, His_AS4Data.DeviceData.FrontCPSI, 90);
    /*
    LCDShowStringUpdate(5,20,CAN_DataCPSI.CodeData,His_AS4Data.DeviceData.CPSI,30,RED,WHITE,16,0);
    LCDShowStringUpdate(5,40,CAN_DataCPSI.CodeData+30,His_AS4Data.DeviceData.FrontCPSI,30,RED,WHITE,16,0);
    LCDShowStringUpdate(5,60,CAN_DataCPSI.CodeData+60,His_AS4Data.DeviceData.MiddleCPSI,30,RED,WHITE,16,0);
    */


    Show_Str(5,100,30,20,CAN_DataCPSI.CodeData,16,0);

    LCD_ShowString(5,20,CAN_DataICCID.CodeData,RED,WHITE,16,0);

    LCD_ShowString(5,40,"IMEI:",RED,WHITE,16,0);
    LCD_ShowString(50,40,CAN_DataIMEI.CodeData,RED,WHITE,16,0);

    LCD_ShowString(5,60,"LBSx:",RED,WHITE,16,0);
    LCD_ShowString(50,60,CAN_DataLBSx.CodeData,RED,WHITE,16,0);

    LCD_ShowString(5,80,"LBSy:",RED,WHITE,16,0);
    LCD_ShowString(50,80,CAN_DataLBSy.CodeData,RED,WHITE,16,0);

    LCD_ShowString(5,160,CAN_DataVersionSM.CodeData,RED,WHITE,16,0);
    LCD_ShowString(5,180,CAN_DataVersionAS.CodeData,RED,WHITE,16,0);


    ShowSignalStrength();
    ShowTime();
}
int ShowInterfaceAbout(void)//4
{

    LCD_ShowPicture(40,20,240,46,gImage_black);
    LCD_ShowChinese(70,100,"成都熊谷加世电器有限公司",RED,WHITE,16,0);
    //LCD_ShowString(150,220,CAN_DataTime.CodeData,RED,WHITE,16,0);

    LCD_ShowChinese(70,130,"成都市高新区天彩路",RED,WHITE,16,0);
    LCD_ShowString(217,130,"100",RED,WHITE,16,0);
    LCD_ShowChinese(247,130,"号",RED,WHITE,16,0);

    LCD_ShowString(75,160,"http://www.xionggu.com",RED,WHITE,16,0);
    LCD_ShowString(68,190,"E-mail:welder@xionggu.com",RED,WHITE,16,0);

    LCD_ShowString(110,220,"+86 400 8080 663",RED,WHITE,16,0);



    //LCD_ShowPicture(40,90,130,130,gImage_scansucc);
    return 0;
}
int IsInterfaceChange(void)
{
    if(Interface.PageChangeFlag == 1)
    {
       // LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
        Interface.PageChangeFlag = 0;
    }
}
int InterfaceChangeFlag(void)
{
    Interface.PageChangeFlag = 1;
}
int ChooseInterface(void)
{
    switch(Interface.page)
    {
    case PageMain:
        ShowInterfaceMain();
        break;
    case PageEquipment:
        ShowInterfaceEquipment();
        break;
    case Page4G:
        ShowInterface4G();
        break;
    case PageAbout:
        ShowInterfaceAbout();
        break;
    default:
        break;
    }
    return 0 ;
}
int WhichInterface(void)
{
    if(Interface.PageChangeFlag == 0)
    {
        ChooseInterface();
    }
}


#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	
//////////////////////////////////////////////////////////////////////////////////	 

//Mini STM32开发板
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
////////////////////////////////////////////////////////////////////////////////// 	  


#define WHITE 0xFFFF//黑色
#define BLACK 0x0000//白色
#define BLUE 0x001F//yellow
#define RED 0xF800//BLUE
#define MAGENTA 0xF81F//GREEN
#define GREEN 0x07E0//purple
#define CYAN 0x7FFF//朱红
#define YELLOW 0xFFE0////深蓝色
#define GRAY 0X8430//灰色
#define BRED 0XF81F//绿色
#define GRED 0XFFE0//深蓝色
#define GBLUE 0X07FF//红色
#define BROWN 0XBC40//半深蓝
#define BRRED 0XFC07//半深蓝
#define DARKBLUE 0X01CF//泥巴色
#define LIGHTBLUE 0X7D7C//深泥巴色
#define GRAYBLUE 0X5458//深泥巴色

#define LIGHTGREEN 0X841F//泥巴色
#define LGRAY 0XC618//蓝黑色
#define LGRAYBLUE 0XA651//紫色
#define LBBLUE 0X2B12//泥巴色



//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE 0
		    						  
//-----------------OLED端口定义----------------  					   
#define OLED_CS  PBout(11) //PCout(9)  //PB11
//#define OLED_RST  PBout(14)//在MINISTM32上直接接到了STM32的复位脚！	
#define OLED_RS  PBout(10)//PCout(8) //PB10 DC
//使用4线串行接口时使用 
#define OLED_SCLK PBout(13)//PBout(0) //PB13
#define OLED_SDIN PBout(15)//PBout(1) //PB15
	//IM3 0 IM2 1 IM1 1 IM0 0
//

#define IM0 PAout(2)
#define IM1 PAout(3)
#define IM2 PAout(4)
#define IM3 PAout(5)

#define USE_HORIZONTAL 2 //2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#define LCD_W 320
#define LCD_H 240


#define OLED_WR  PBout(15)//PCout(7)  //	 	  
#define OLED_RD PCout(6)

//PB0~7,作为数据线
#define DATAOUT(x) GPIOB->ODR=(GPIOB->ODR&0xff00)|(x&0x00FF); //输出
  
	     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//-----------------LCD端口定义---------------- 

//#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_13)//SCL=SCLK
//#define LCD_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_13)

//#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_15)//SDA=MOSI
//#define LCD_MOSI_Set() GPIO_SetBits(GPIOB,GPIO_Pin_15)

//#define LCD_RES_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_11)//RES
//#define LCD_RES_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_11)

//#define LCD_DC_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_10)//DC
//#define LCD_DC_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_10)

//#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)//BLK
//#define LCD_BLK_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_4)

						   		    
void OLED_Init(void);

void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数


void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//指定区域填充颜色
void LCD_DrawPoint(u16 x,u16 y,u16 color);//在指定位置画一个点
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//在指定位置画一条线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//在指定位置画一个矩形
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//在指定位置画一个圆

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示汉字串
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个12x12汉字
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个16x16汉字
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个24x24汉字
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个32x32汉字

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//显示一个字符
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//显示字符串
u32 mypow(u8 m,u8 n);//求幂
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//显示整数变量
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//显示两位小数变量

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片

void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

int LCDShowStringUpdate(u16 x,u16 y,const char *NewString, 
    char *HisString,unsigned int len,u16 fc,u16 bc,u8 sizey,u8 mode);


	int LCDStrLineFeed(u16 x,u16 y,const char *NewString, char *HisString,char *FrontString,unsigned int len,int color);

	int LCDStrLine3Feed(u16 x,u16 y,const char *NewString, char *HisString,char *FrontString,char *MiddleString,unsigned int len);
	void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);

#endif  
	 




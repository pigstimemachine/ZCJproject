#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	
//////////////////////////////////////////////////////////////////////////////////	 

//Mini STM32������
//SSD1306 OLED ����IC��������
//������ʽ:8080����/4�ߴ���
////////////////////////////////////////////////////////////////////////////////// 	  


#define WHITE 0xFFFF//��ɫ
#define BLACK 0x0000//��ɫ
#define BLUE 0x001F//yellow
#define RED 0xF800//BLUE
#define MAGENTA 0xF81F//GREEN
#define GREEN 0x07E0//purple
#define CYAN 0x7FFF//���
#define YELLOW 0xFFE0////����ɫ
#define GRAY 0X8430//��ɫ
#define BRED 0XF81F//��ɫ
#define GRED 0XFFE0//����ɫ
#define GBLUE 0X07FF//��ɫ
#define BROWN 0XBC40//������
#define BRRED 0XFC07//������
#define DARKBLUE 0X01CF//���ɫ
#define LIGHTBLUE 0X7D7C//�����ɫ
#define GRAYBLUE 0X5458//�����ɫ

#define LIGHTGREEN 0X841F//���ɫ
#define LGRAY 0XC618//����ɫ
#define LGRAYBLUE 0XA651//��ɫ
#define LBBLUE 0X2B12//���ɫ



//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ
#define OLED_MODE 0
		    						  
//-----------------OLED�˿ڶ���----------------  					   
#define OLED_CS  PBout(11) //PCout(9)  //PB11
//#define OLED_RST  PBout(14)//��MINISTM32��ֱ�ӽӵ���STM32�ĸ�λ�ţ�	
#define OLED_RS  PBout(10)//PCout(8) //PB10 DC
//ʹ��4�ߴ��нӿ�ʱʹ�� 
#define OLED_SCLK PBout(13)//PBout(0) //PB13
#define OLED_SDIN PBout(15)//PBout(1) //PB15
	//IM3 0 IM2 1 IM1 1 IM0 0
//

#define IM0 PAout(2)
#define IM1 PAout(3)
#define IM2 PAout(4)
#define IM3 PAout(5)

#define USE_HORIZONTAL 2 //2  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#define LCD_W 320
#define LCD_H 240


#define OLED_WR  PBout(15)//PCout(7)  //	 	  
#define OLED_RD PCout(6)

//PB0~7,��Ϊ������
#define DATAOUT(x) GPIOB->ODR=(GPIOB->ODR&0xff00)|(x&0x00FF); //���
  
	     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

//-----------------LCD�˿ڶ���---------------- 

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

void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��


void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//ָ�����������ɫ
void LCD_DrawPoint(u16 x,u16 y,u16 color);//��ָ��λ�û�һ����
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//��ָ��λ�û�һ����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//��ָ��λ�û�һ������
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//��ָ��λ�û�һ��Բ

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ���ִ�
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����12x12����
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����16x16����
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����24x24����
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����32x32����

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾһ���ַ�
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ�ַ���
u32 mypow(u8 m,u8 n);//����
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��������
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��λС������

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//��ʾͼƬ

void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

int LCDShowStringUpdate(u16 x,u16 y,const char *NewString, 
    char *HisString,unsigned int len,u16 fc,u16 bc,u8 sizey,u8 mode);


	int LCDStrLineFeed(u16 x,u16 y,const char *NewString, char *HisString,char *FrontString,unsigned int len,int color);

	int LCDStrLine3Feed(u16 x,u16 y,const char *NewString, char *HisString,char *FrontString,char *MiddleString,unsigned int len);
	void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);

#endif  
	 




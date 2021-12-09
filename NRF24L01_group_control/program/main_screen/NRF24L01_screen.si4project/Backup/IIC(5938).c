#include "IIC.h"
#include "delay.h" 
//初始化IIC
void IIC_Init(void)
{		
/*	
 	RCC->APB2ENR|=1<<3;//先使能外设IO PORTB时钟 							 
	GPIOB->CRH&=0XFFFF00FF;//PB10/11 推挽输出
	GPIOB->CRH|=0X00003300;	   
	GPIOB->ODR|=3<<10;     //PB10,11 输出高
*/	
	RCC->APB2ENR|=1<<5;//先使能外设IO PORTD时钟 							 
	GPIOD->CRL&=0XFFF00FFF;//PD3/4 推挽输出
	GPIOD->CRL|=0X00033000;	   
	GPIOD->ODR|=3<<3;     //PD3,4 输出高
}  
//产生IIC起始信号
u8  IIC_Start(void)
{	
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);	
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
	return 1;
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);	 	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入 
	IIC_SDA=1;  delay_us(1);	   
	IIC_SCL=1;  delay_us(1);	 		 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			IIC_SDA=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(2);   //对TEA5767这三个延时都是必须的
			IIC_SCL=1;
			delay_us(2); 
			IIC_SCL=0;	
			delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	  unsigned char i,receive=0;
	  SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	  {
			IIC_SCL=0; 
			delay_us(2);
			IIC_SCL=1;
			receive<<=1;
			if(READ_SDA)  receive++;   
			delay_us(1); 
    }					 
    if (!ack)
			IIC_NAck();//发送nACK
    else
			IIC_Ack(); //发送ACK   
    return receive;
}
////////////////////////////////////////////硬件IIC
/*void IIC_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   I2C_InitTypeDef I2C_InitStructure;
 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);	*/ 

  //PB6-I2C1_SCL PB7-I2C1_SDA PB10-I2C2_SCL PB11-I2C2_SDA
   /* Configure IO connected to IIC*********************/
/*   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
 
   I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1 = 0X0A;
   I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed = 200000;
 
   I2C_Cmd(I2C2, ENABLE);   
 
   I2C_Init(I2C2, &I2C_InitStructure);	*/

  /*允许1字节1应答模式*/
/*   I2C_AcknowledgeConfig(I2C2, ENABLE);    
}*/	   
 
/*void I2C_WriteOneByte(u8 id, u16 write_address,u8 byte)
{
   while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
   I2C_GenerateSTART(I2C2,ENABLE);
   //产生起始条件
   while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)));
   //等待ACK  
   I2C_Send7bitAddress(I2C2,id,I2C_Direction_Transmitter);
   //向设备发送设备地址
   while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
   //等待ACK
   I2C_SendData(I2C2, write_address>>8);  	 //发送高地址
   //寄存器地址
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //等待ACK
   I2C_SendData(I2C2, write_address&0xff); 	 //发送低地址
   //寄存器地址
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //等待ACK
   I2C_SendData(I2C2, byte);  
   //发送数据
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //发送完成
   I2C_GenerateSTOP(I2C2, ENABLE);
   //产生结束信号     
}

u8 I2C_ReadOneByte(u8 id, u16 read_address)
{  
   u8 temp=0;        
   while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
   //等待I2C
   I2C_GenerateSTART(I2C2,ENABLE);
   //产生起始条件
   while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)));
   //EV5
   I2C_Send7bitAddress(I2C2, id, I2C_Direction_Transmitter); 
   //发送地址
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
   //EV6
   I2C_Cmd(I2C2, ENABLE); 
   //重新设置可以清除EV6
   I2C_SendData(I2C2, read_address>>8);  
   //发送读得地址
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //EV8
   I2C_SendData(I2C2, read_address&0xff);  
   //发送读得地址
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
   //EV8 
   I2C_GenerateSTART(I2C2, ENABLE);	
   //重新发送
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
   //EV5
   I2C_Send7bitAddress(I2C2, id, I2C_Direction_Receiver); 
   //发送读地址
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
   //EV6  
   I2C_AcknowledgeConfig(I2C2, DISABLE);
   I2C_GenerateSTOP(I2C2, ENABLE); 
   //关闭应答和停止条件产生
   while(!(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)));     
   temp = I2C_ReceiveData(I2C2);
   I2C_AcknowledgeConfig(I2C2, ENABLE);
   return temp;
}*/

  



























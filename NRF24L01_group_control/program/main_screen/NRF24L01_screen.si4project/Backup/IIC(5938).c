#include "IIC.h"
#include "delay.h" 
//��ʼ��IIC
void IIC_Init(void)
{		
/*	
 	RCC->APB2ENR|=1<<3;//��ʹ������IO PORTBʱ�� 							 
	GPIOB->CRH&=0XFFFF00FF;//PB10/11 �������
	GPIOB->CRH|=0X00003300;	   
	GPIOB->ODR|=3<<10;     //PB10,11 �����
*/	
	RCC->APB2ENR|=1<<5;//��ʹ������IO PORTDʱ�� 							 
	GPIOD->CRL&=0XFFF00FFF;//PD3/4 �������
	GPIOD->CRL|=0X00033000;	   
	GPIOD->ODR|=3<<3;     //PD3,4 �����
}  
//����IIC��ʼ�ź�
u8  IIC_Start(void)
{	
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);	
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	return 1;
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);	 	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ���� 
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
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
			IIC_SDA=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(2);   //��TEA5767��������ʱ���Ǳ����
			IIC_SCL=1;
			delay_us(2); 
			IIC_SCL=0;	
			delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	  unsigned char i,receive=0;
	  SDA_IN();//SDA����Ϊ����
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
			IIC_NAck();//����nACK
    else
			IIC_Ack(); //����ACK   
    return receive;
}
////////////////////////////////////////////Ӳ��IIC
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

  /*����1�ֽ�1Ӧ��ģʽ*/
/*   I2C_AcknowledgeConfig(I2C2, ENABLE);    
}*/	   
 
/*void I2C_WriteOneByte(u8 id, u16 write_address,u8 byte)
{
   while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
   I2C_GenerateSTART(I2C2,ENABLE);
   //������ʼ����
   while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)));
   //�ȴ�ACK  
   I2C_Send7bitAddress(I2C2,id,I2C_Direction_Transmitter);
   //���豸�����豸��ַ
   while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
   //�ȴ�ACK
   I2C_SendData(I2C2, write_address>>8);  	 //���͸ߵ�ַ
   //�Ĵ�����ַ
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //�ȴ�ACK
   I2C_SendData(I2C2, write_address&0xff); 	 //���͵͵�ַ
   //�Ĵ�����ַ
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //�ȴ�ACK
   I2C_SendData(I2C2, byte);  
   //��������
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //�������
   I2C_GenerateSTOP(I2C2, ENABLE);
   //���������ź�     
}

u8 I2C_ReadOneByte(u8 id, u16 read_address)
{  
   u8 temp=0;        
   while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
   //�ȴ�I2C
   I2C_GenerateSTART(I2C2,ENABLE);
   //������ʼ����
   while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)));
   //EV5
   I2C_Send7bitAddress(I2C2, id, I2C_Direction_Transmitter); 
   //���͵�ַ
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
   //EV6
   I2C_Cmd(I2C2, ENABLE); 
   //�������ÿ������EV6
   I2C_SendData(I2C2, read_address>>8);  
   //���Ͷ��õ�ַ
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   //EV8
   I2C_SendData(I2C2, read_address&0xff);  
   //���Ͷ��õ�ַ
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
   //EV8 
   I2C_GenerateSTART(I2C2, ENABLE);	
   //���·���
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
   //EV5
   I2C_Send7bitAddress(I2C2, id, I2C_Direction_Receiver); 
   //���Ͷ���ַ
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
   //EV6  
   I2C_AcknowledgeConfig(I2C2, DISABLE);
   I2C_GenerateSTOP(I2C2, ENABLE); 
   //�ر�Ӧ���ֹͣ��������
   while(!(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)));     
   temp = I2C_ReceiveData(I2C2);
   I2C_AcknowledgeConfig(I2C2, ENABLE);
   return temp;
}*/

  



























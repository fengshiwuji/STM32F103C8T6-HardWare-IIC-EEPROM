/******************************************************************************
*оƬ��STM32F103RC
*ʵ�ֹ��ܣ�ʹ�����ģ��IIC��дEEPROM
*		1��ʵ���˵����ֽڵĶ�д
*		2��ʵ����PageWrite��ReadBytes����
*		3��ʵ�������迼�ǵ�ַ���룬д��С�ڣ�EEPROM_SIZE-addr�����ֽ�����
*		4������˶Ը��������������ݼ��ַ����Ĵ洢�Ͷ�ȡ
*���ڣ�2022-09-28
********************************************************************************/

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "delay.h"
#include "bsp_e2prom.h"
#include <string.h>

long double DoubleNums[10]= {0};

int IntNums[10] = {0};

uint8_t StringNums[50] = "���ӱ�2101����ƽ";

uint8_t ReadBuff[100]={0};
uint8_t WriteBuff[100]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
					  21,22,23,18,19,20,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
					  11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,0,1,2,3,4,5,6,7,8,9,10,29,30,31,
					   100,200,150,250};

int main(void)
{
	uint8_t i = 0;
	USART_Config();
	LED_GPIO_Config();
	delay_init();
	IIC_EEPROM_GPIO_Config();
	EEPROM_INFO("\r\n����һ��IIC EEPROM �Ĳ��Գ���\n");
	EEPROM_INFO("\r\n����Ѱ��EEPROM,���Ե�......\n");
	
	/*ѰַEEPROM*/
	I2C_START();
	IIC_WriteByte(EEPROM_ADDR);
	if(I2C_Waite_Slaver_ACK(6)==0)
	{
		for(i=0;i<3;i++)
		{
			LED(ON);
			delay_ms(200);
			LED(OFF);
			delay_ms(200);
		}
		EEPROM_INFO("\r\nEEPROM�ѽ������ӣ�\n");
	}
	
	I2C_STOP();

	/*��ҳ���ֽ��Ҳ��ù��ĵ�ַ�Ƿ�����д�����*/
	
	I2C_EEPROM_BuffWrite(0,WriteBuff,60);
	
	I2C_EEPROM_ReadBytes(0,ReadBuff,60);
	
	for(i=0;i<60;i++)
	{
		ReadBuff[i] = I2C_EEPROM_ReadByte(i+1);
		printf("ReadBuff[%d] = %d\n",i,ReadBuff[i]);
	}
	
	/*��ʼ����д�������*/
	for(i=0;i<10;i++)
	{
		DoubleNums[i] = i*10+1.0;
		IntNums[i] = i*10+1;
	}
	
	/*������д�����*/
	I2C_EEPROM_BuffWrite(0,(void*)DoubleNums,10);

	I2C_EEPROM_ReadBytes(0,(void*)DoubleNums,10);
	
	EEPROM_INFO("\r\nEEPROM �������ɹ�д�룡\n");
	for(i=0;i<10;i++)
	{
		printf("DoubleNums[%d] = %LF\n",i,DoubleNums[i]);
	}
	
	/*��������д�����*/
	I2C_EEPROM_BuffWrite(0,(void*)IntNums,10);

	I2C_EEPROM_ReadBytes(0,(void*)IntNums,10);
	
	EEPROM_INFO("\r\nEEPROM �������ݳɹ�д�룡\n");
	for(i=0;i<10;i++)
	{
		printf("IntNums[%d] = %d\n",i,IntNums[i]);
	}
	
	/*�ַ���д�����*/
	I2C_EEPROM_BuffWrite(0,(void*)StringNums,sizeof(StringNums));

	I2C_EEPROM_ReadBytes(0,(void*)StringNums,sizeof(StringNums));
	
	EEPROM_INFO("\r\nEEPROM �ַ������ݳɹ�д�룡\n");

	printf("%s\n",StringNums);
	
	while(1)
	{
		
	}
}

/******************************************************************************
*芯片：STM32F103RC
*实现功能：使用软件模拟IIC读写EEPROM
*		1、实现了单个字节的读写
*		2、实现了PageWrite和ReadBytes函数
*		3、实现了无需考虑地址对齐，写入小于（EEPROM_SIZE-addr）个字节数据
*		4、完成了对浮点数、整型数据及字符串的存储和读取
*日期：2022-09-28
********************************************************************************/

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "delay.h"
#include "bsp_e2prom.h"
#include <string.h>

long double DoubleNums[10]= {0};

int IntNums[10] = {0};

uint8_t StringNums[50] = "电子本2101张永平";

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
	EEPROM_INFO("\r\n这是一个IIC EEPROM 的测试程序！\n");
	EEPROM_INFO("\r\n正在寻找EEPROM,请稍等......\n");
	
	/*寻址EEPROM*/
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
		EEPROM_INFO("\r\nEEPROM已建立连接！\n");
	}
	
	I2C_STOP();

	/*多页多字节且不用关心地址是否对齐的写入测试*/
	
	I2C_EEPROM_BuffWrite(0,WriteBuff,60);
	
	I2C_EEPROM_ReadBytes(0,ReadBuff,60);
	
	for(i=0;i<60;i++)
	{
		ReadBuff[i] = I2C_EEPROM_ReadByte(i+1);
		printf("ReadBuff[%d] = %d\n",i,ReadBuff[i]);
	}
	
	/*初始化待写入的数据*/
	for(i=0;i<10;i++)
	{
		DoubleNums[i] = i*10+1.0;
		IntNums[i] = i*10+1;
	}
	
	/*浮点数写入测试*/
	I2C_EEPROM_BuffWrite(0,(void*)DoubleNums,10);

	I2C_EEPROM_ReadBytes(0,(void*)DoubleNums,10);
	
	EEPROM_INFO("\r\nEEPROM 浮点数成功写入！\n");
	for(i=0;i<10;i++)
	{
		printf("DoubleNums[%d] = %LF\n",i,DoubleNums[i]);
	}
	
	/*整形数据写入测试*/
	I2C_EEPROM_BuffWrite(0,(void*)IntNums,10);

	I2C_EEPROM_ReadBytes(0,(void*)IntNums,10);
	
	EEPROM_INFO("\r\nEEPROM 整型数据成功写入！\n");
	for(i=0;i<10;i++)
	{
		printf("IntNums[%d] = %d\n",i,IntNums[i]);
	}
	
	/*字符串写入测试*/
	I2C_EEPROM_BuffWrite(0,(void*)StringNums,sizeof(StringNums));

	I2C_EEPROM_ReadBytes(0,(void*)StringNums,sizeof(StringNums));
	
	EEPROM_INFO("\r\nEEPROM 字符串数据成功写入！\n");

	printf("%s\n",StringNums);
	
	while(1)
	{
		
	}
}

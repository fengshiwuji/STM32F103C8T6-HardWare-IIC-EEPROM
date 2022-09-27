#ifndef __BSP_E2PROM_H__
#define __BSP_E2PROM_H__

#include "stm32f10x.h"
#include "bsp_usart.h"

/*���Ժ�*/
#define EEPROM_DEBUG_ON				1

#define EEPROM_INFO(fmt,arg...)		printf("<<-EEPROM-INFO->>"fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)	printf("<<-EEPROM-ERROR->>"fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)	do{\
										if(EEPROM_DEBUG_ON)\
										printf("<<-EEPROM-DEBUG->> [%s][%d]"fmt"\n",__FILE__,__LINE__,##arg);\
									}while(0)
/*�궨��I2C��ʱ��Ƶ��*/
#define IIC_CLK_SPEED				400000
/*�궨��I2C��ʱ��Ƶ��*/	
#define STM32_OWN_ADDR				0x5f
/*EEPROM�ĵ�ַ*/
#define EEPROM_DEVICE_ADDR			0xA0
/*EEPROM��PageSize*/
#define EEPROM_PAGESIZE				32

/*����IIC�ĺ궨��*/
#define EEPROM_I2Cx					I2C1
/*IIC�����ʱ��*/
#define EEPROM_I2C_CLK_FUN			RCC_APB1PeriphClockCmd
#define EEPROM_I2C_CLK				RCC_APB1Periph_I2C1
/*GPIO�˿�ʱ��*/
#define EEPROM_I2C_GPIO_CLK_FUN		RCC_APB2PeriphClockCmd
#define EEPROM_I2C_GPIO_SCK_CLK	    RCC_APB2Periph_GPIOB
#define EEPROM_I2C_GPIO_SDA_CLK	    RCC_APB2Periph_GPIOB

/*GPI0����(SDA��SCK)����*/
#define EEPROM_I2C_GPIO_SCK_PORT	GPIOB
#define EEPROM_I2C_GPIO_SCK_PIN		GPIO_Pin_6
#define EEPROM_I2C_GPIO_SDA_PORT	GPIOB
#define EEPROM_I2C_GPIO_SDA_PIN		GPIO_Pin_7

/*����һ������*/
#define IIC_TIMEOUT					10000

void IIC_EEPROM_Init(void);
uint8_t IIC_EEPROM_WriteByte(uint16_t addr,uint8_t data);
uint8_t IIC_EEPROM_Read(uint16_t addr,uint8_t* data,uint8_t numReadToBytes);
uint8_t EEPROM_Waite_Write_End(void);
uint8_t IIC_EEPROM_PageWrite(uint16_t addr,uint8_t* data,uint8_t numWriteBytes);
/*�ú�����Ҫʵ���˵�ַ���룬�����پ�����ֻ��д��32�ֽ����ݣ�����д��������Χ�ڵ�������ֽ�*/
uint8_t IIC_EEPROM_BuffWrite(uint16_t addr,uint8_t* buff,uint8_t numBuffBytes);

#endif /* __BSP_E2PROM_H__ */

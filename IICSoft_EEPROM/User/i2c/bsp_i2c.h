#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "delay.h"

/*调试信息*/
#define	EEPROM_DEBUG_ON 					1
#define EEPROM_INFO(fmt,arg...)				printf("<<-EEPROM-INFO->>"fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)			printf("<<-EEPROM-ERROR->>"fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)			do{\
												if(EEPROM_DEBUG_ON)\
													printf("<<-EEPROM-DEBUG->> [%s][%d]"fmt"\n",__FILE__,__LINE__,##arg);\
											}while(0)
/*定义软件I2C的宏*/
/*时钟定义*/											
#define EEPROM_I2C_GPIO_CLK_FUN				RCC_APB2PeriphClockCmd
#define	EEPROM_I2C_GPIO_SCK_CLK				RCC_APB2Periph_GPIOB
#define EEPROM_I2C_GPIO_SDA_CLK				RCC_APB2Periph_GPIOB
/*引脚及端口定义*/
#define EEPROM_I2C_GPIO_SCK_PORT			GPIOB
#define EEPROM_I2C_GPIO_SCK_PIN				GPIO_Pin_8
#define EEPROM_I2C_GPIO_SDA_PORT			GPIOB
#define EEPROM_I2C_GPIO_SDA_PIN				GPIO_Pin_9
											
/*SDA、SCK引脚的状态*/
#define	I2C_GPIO_SCK_HIGH()					GPIO_SetBits(EEPROM_I2C_GPIO_SCK_PORT, EEPROM_I2C_GPIO_SCK_PIN)
#define	I2C_GPIO_SCK_LOW()					GPIO_ResetBits(EEPROM_I2C_GPIO_SCK_PORT, EEPROM_I2C_GPIO_SCK_PIN)
#define I2C_GPIO_OUT_SDA_HIGH()				GPIO_SetBits(EEPROM_I2C_GPIO_SDA_PORT, EEPROM_I2C_GPIO_SDA_PIN)	
#define I2C_GPIO_OUT_SDA_LOW()				GPIO_ResetBits(EEPROM_I2C_GPIO_SDA_PORT, EEPROM_I2C_GPIO_SDA_PIN)
#define I2C_GPIO_IN_SDA()					GPIO_ReadInputDataBit(EEPROM_I2C_GPIO_SCK_PORT, EEPROM_I2C_GPIO_SDA_PIN)

											/*延时4.7us*/
#define IIC_delay()							delay_us(5)
#define IIC_TimeOut							10000

/*定义EEPROM的器件地址*/
#define	EEPROM_ADDR			0xA0

#define OUTPUT		1
#define INPUT		0

void IIC_EEPROM_GPIO_Config(void);
void I2C_START(void);
void I2C_STOP(void);
void IIC_WriteByte(uint8_t data);
void Master_SendNoAck(void);
void Master_SendAck(void);
uint8_t I2C_ReadByte(uint8_t ack);
uint8_t I2C_Waite_Slaver_ACK(uint8_t errorcode);

#endif /* __BSP_I2C_H__ */

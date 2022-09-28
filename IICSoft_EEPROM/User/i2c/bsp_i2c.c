#include "bsp_i2c.h"

uint16_t TimeOut = 0;

/*初始化引脚配置*/
void IIC_EEPROM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*开启IIC所用到GPIO端口的时钟*/
	EEPROM_I2C_GPIO_CLK_FUN(EEPROM_I2C_GPIO_SCK_CLK|EEPROM_I2C_GPIO_SDA_CLK,ENABLE);
	
	/*配置SCK、SDA所使用的GPIO引脚*/
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			/*配置为GPIO_Mode_Out_OD、GPIO_Mode_Out_PP皆可*/
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_GPIO_SCK_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SDA_PIN;
	/*这里可省略对mode和speed的赋值，因为变量的值未曾改变*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			/*配置为GPIO_Mode_Out_OD、GPIO_Mode_Out_PP皆可*/
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_GPIO_SDA_PORT,&GPIO_InitStruct);
	I2C_STOP();
}
/*SDA数据线的输入输出切换*/
void EEPROM_SDA_InputOrOutput(uint8_t status)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*开启IIC所用到GPIO端口的时钟*/
	EEPROM_I2C_GPIO_CLK_FUN(EEPROM_I2C_GPIO_SDA_CLK,ENABLE);
	
	if(status==OUTPUT)
	{
		/*这里可省略对mode和speed的赋值，因为变量的值未曾改变*/
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	/*配置为GPIO_Mode_Out_OD、GPIO_Mode_Out_PP皆可*/
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	}
	else if(status==INPUT)
	{
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;/*配置为GPIO_Mode_IPU、GPIO_Mode_IPD、GPIO_Mode_Out_OD、GPIO_Mode_IN_FLOATING皆可*/
	}
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SDA_PIN;
	GPIO_Init(EEPROM_I2C_GPIO_SDA_PORT,&GPIO_InitStruct);
}
/*等待超时*/
static uint8_t IIC_Waite_OutTime(uint8_t errorcode)
{
	EEPROM_DEBUG("IIC总线等待超时，errorcode:%d\n",errorcode);
	return 1;
}
/*起始信号*/
void I2C_START(void)
{
	EEPROM_SDA_InputOrOutput(OUTPUT);
	I2C_GPIO_OUT_SDA_HIGH();
	IIC_delay();
	I2C_GPIO_SCK_HIGH();
	IIC_delay();
	I2C_GPIO_OUT_SDA_LOW();
	IIC_delay();
	I2C_GPIO_SCK_LOW();
	IIC_delay();
}
/*写入一个字节
*编写要点：数据变化前保证时钟线为低电平，时钟线高电平时数据保持稳定。
*/
void IIC_WriteByte(uint8_t data)
{
	uint8_t i = 0;
	EEPROM_SDA_InputOrOutput(OUTPUT);
	I2C_GPIO_SCK_LOW();
	IIC_delay();
	for(i=0;i<8;i++)
	{
		if(data&0x80)
			I2C_GPIO_OUT_SDA_HIGH();
		else
			I2C_GPIO_OUT_SDA_LOW();
		data<<=1;
		IIC_delay();
		I2C_GPIO_SCK_HIGH();
		IIC_delay();
		I2C_GPIO_SCK_LOW();
		IIC_delay();
	}
}
/*主机发送响应信号*/
void Master_SendAck(void)
{
	EEPROM_SDA_InputOrOutput(OUTPUT);
	I2C_GPIO_SCK_LOW();
	IIC_delay();
	I2C_GPIO_OUT_SDA_LOW();
	IIC_delay();
	I2C_GPIO_SCK_HIGH();
	IIC_delay();
	I2C_GPIO_SCK_LOW();
}
/*主机发送非应答信号*/
void Master_SendNoAck(void)
{
	EEPROM_SDA_InputOrOutput(OUTPUT);
	I2C_GPIO_SCK_LOW();
	IIC_delay();
	I2C_GPIO_OUT_SDA_HIGH();
	IIC_delay();
	I2C_GPIO_SCK_HIGH();
	IIC_delay();
	I2C_GPIO_SCK_LOW();
	IIC_delay();
}
/*读取一个字节
*编写要点：在时钟线为高电平时读取数据，时钟线为低电平时等待接入数据。
*/
uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t i = 0,data = 0;
	EEPROM_SDA_InputOrOutput(INPUT);
	for(i=0;i<8;i++)
	{
		data <<= 1;
		I2C_GPIO_SCK_HIGH();
		IIC_delay();
		if(I2C_GPIO_IN_SDA()==1)
		{
			data |= 0x01;
		}
		I2C_GPIO_SCK_LOW();
		IIC_delay();
	}
	if(ack)
	{
		Master_SendAck();
	}
	else
	{
		Master_SendNoAck();
	}
	return data;
}

/*等待从机响应*/
uint8_t I2C_Waite_Slaver_ACK(uint8_t errorcode)
{
	TimeOut = IIC_TimeOut;
	EEPROM_SDA_InputOrOutput(INPUT);
	I2C_GPIO_OUT_SDA_HIGH();
	IIC_delay();
	I2C_GPIO_SCK_HIGH();
	while(I2C_GPIO_IN_SDA()==1)
	{
		if(TimeOut==0)
			return IIC_Waite_OutTime(errorcode);
		TimeOut--;
	}
	I2C_GPIO_SCK_LOW();
	return 0;
}
/*停止信号*/
void I2C_STOP(void)
{
	EEPROM_SDA_InputOrOutput(OUTPUT);
	I2C_GPIO_SCK_LOW();
	I2C_GPIO_OUT_SDA_LOW();
	IIC_delay();
	I2C_GPIO_SCK_HIGH();
	IIC_delay();
	I2C_GPIO_OUT_SDA_HIGH();
	IIC_delay();
}


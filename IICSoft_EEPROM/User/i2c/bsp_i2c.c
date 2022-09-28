#include "bsp_i2c.h"

uint16_t TimeOut = 0;

/*��ʼ����������*/
void IIC_EEPROM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*����IIC���õ�GPIO�˿ڵ�ʱ��*/
	EEPROM_I2C_GPIO_CLK_FUN(EEPROM_I2C_GPIO_SCK_CLK|EEPROM_I2C_GPIO_SDA_CLK,ENABLE);
	
	/*����SCK��SDA��ʹ�õ�GPIO����*/
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			/*����ΪGPIO_Mode_Out_OD��GPIO_Mode_Out_PP�Կ�*/
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_GPIO_SCK_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SDA_PIN;
	/*�����ʡ�Զ�mode��speed�ĸ�ֵ����Ϊ������ֵδ���ı�*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			/*����ΪGPIO_Mode_Out_OD��GPIO_Mode_Out_PP�Կ�*/
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_GPIO_SDA_PORT,&GPIO_InitStruct);
	I2C_STOP();
}
/*SDA�����ߵ���������л�*/
void EEPROM_SDA_InputOrOutput(uint8_t status)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*����IIC���õ�GPIO�˿ڵ�ʱ��*/
	EEPROM_I2C_GPIO_CLK_FUN(EEPROM_I2C_GPIO_SDA_CLK,ENABLE);
	
	if(status==OUTPUT)
	{
		/*�����ʡ�Զ�mode��speed�ĸ�ֵ����Ϊ������ֵδ���ı�*/
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	/*����ΪGPIO_Mode_Out_OD��GPIO_Mode_Out_PP�Կ�*/
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	}
	else if(status==INPUT)
	{
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;/*����ΪGPIO_Mode_IPU��GPIO_Mode_IPD��GPIO_Mode_Out_OD��GPIO_Mode_IN_FLOATING�Կ�*/
	}
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SDA_PIN;
	GPIO_Init(EEPROM_I2C_GPIO_SDA_PORT,&GPIO_InitStruct);
}
/*�ȴ���ʱ*/
static uint8_t IIC_Waite_OutTime(uint8_t errorcode)
{
	EEPROM_DEBUG("IIC���ߵȴ���ʱ��errorcode:%d\n",errorcode);
	return 1;
}
/*��ʼ�ź�*/
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
/*д��һ���ֽ�
*��дҪ�㣺���ݱ仯ǰ��֤ʱ����Ϊ�͵�ƽ��ʱ���߸ߵ�ƽʱ���ݱ����ȶ���
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
/*����������Ӧ�ź�*/
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
/*�������ͷ�Ӧ���ź�*/
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
/*��ȡһ���ֽ�
*��дҪ�㣺��ʱ����Ϊ�ߵ�ƽʱ��ȡ���ݣ�ʱ����Ϊ�͵�ƽʱ�ȴ��������ݡ�
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

/*�ȴ��ӻ���Ӧ*/
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
/*ֹͣ�ź�*/
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


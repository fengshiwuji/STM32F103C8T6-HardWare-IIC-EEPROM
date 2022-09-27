#include "bsp_e2prom.h"

uint16_t I2C_TimeOut = 0;

static void IIC_EEPROM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*����IIC���õ�GPIO�˿ڵ�ʱ��*/
	EEPROM_I2C_GPIO_CLK_FUN(EEPROM_I2C_GPIO_SCK_CLK|EEPROM_I2C_GPIO_SDA_CLK,ENABLE);
	/*����IIC����ʱ��*/
	EEPROM_I2C_CLK_FUN(EEPROM_I2C_CLK,ENABLE);
	
	/*����SCK��SDA��ʹ�õ�GPIO����*/
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_GPIO_SCK_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C_GPIO_SDA_PIN;
	/*�����ʡ�Զ�mode��speed�ĸ�ֵ����Ϊ������ֵδ���ı�*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_GPIO_SDA_PORT,&GPIO_InitStruct);
}
/*I2C ģʽ������*/
static void IIC_EEPROM_Mode_Config(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = IIC_CLK_SPEED;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = STM32_OWN_ADDR;
	I2C_Init(EEPROM_I2Cx,&I2C_InitStruct);
	/*ʹ��IIC*/
	I2C_Cmd(EEPROM_I2Cx, ENABLE);
}
/*��ʼ��IIC����*/
void IIC_EEPROM_Init(void)
{
	IIC_EEPROM_GPIO_Config();
	IIC_EEPROM_Mode_Config();
}

static uint8_t IIC_Waite_OutTime(uint8_t errorcode)
{
	EEPROM_DEBUG("IIC���ߵȴ���ʱ��errorcode:%d\n",errorcode);
	return 1;
}

/*
*��EEPROMдһ�ֽ�
*д��ɹ�����0�����򷵻ط�0
*/
uint8_t IIC_EEPROM_WriteByte(uint16_t addr,uint8_t data)
{
	I2C_TimeOut = IIC_TIMEOUT;
	uint8_t addrH = (addr>>8)&0xff;
	uint8_t addrL = addr&0xff;
	
	/*�ȴ�EEPROM ����*/
	while(I2C_GetFlagStatus (EEPROM_I2Cx,I2C_FLAG_BUSY)==SET)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(1);
	}
	
	/*������ʼ�ź�*/
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	/*���EV5�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(1);
	}
	I2C_TimeOut = IIC_TIMEOUT;
	/*��⵽EV5�¼�������������ַ���Ͷ�д����*/
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_DEVICE_ADDR,I2C_Direction_Transmitter);
	/*���EV6�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(2);
	}
	/*��⵽EV6�¼������ʹ洢��Ԫ��ַ*/
	I2C_SendData(EEPROM_I2Cx,addrH);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(3);
	}
	I2C_SendData(EEPROM_I2Cx,addrL);
	
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(3);
	}
	/*��⵽EV8�¼�������һ�ֽ�����*/
	I2C_SendData(EEPROM_I2Cx,data);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(4);
	}
	/*����ֹͣ�ź�*/
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	return 0;
}

/*
*��EEPROMдҳд��
*д��ɹ�����0�����򷵻ط�0
*/
uint8_t IIC_EEPROM_PageWrite(uint16_t addr,uint8_t* data,uint8_t numWriteBytes)
{
	I2C_TimeOut = IIC_TIMEOUT;
	uint8_t addrH = (addr>>8)&0xff;
	uint8_t addrL = addr&0xff;
	/*������ʼ�ź�*/
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	/*���EV5�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(1);
	}
	I2C_TimeOut = IIC_TIMEOUT;
	/*��⵽EV5�¼�������������ַ���Ͷ�д����*/
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_DEVICE_ADDR,I2C_Direction_Transmitter);
	/*���EV6�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(2);
	}
	/*��⵽EV6�¼������ʹ洢��Ԫ��ַ*/
	I2C_SendData(EEPROM_I2Cx,addrH);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(3);
	}
	I2C_SendData(EEPROM_I2Cx,addrL);
	
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(3);
	}
	while(numWriteBytes)
	{
		/*��⵽EV8�¼�������һ�ֽ�����*/
		I2C_SendData(EEPROM_I2Cx,*data);
		I2C_TimeOut = IIC_TIMEOUT;
		/*���EV8�¼�*/
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR)
		{
			I2C_TimeOut--;
			if(I2C_TimeOut==0)
				return IIC_Waite_OutTime(4);
		}
		data++;
		numWriteBytes--;
	}
	/*����ֹͣ�ź�*/
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	return 0;
}

uint8_t IIC_EEPROM_BuffWrite(uint16_t addr,uint8_t* buff,uint8_t numBuffBytes)
{
	uint8_t Countaddr = addr%EEPROM_PAGESIZE;
	uint8_t CountFullPages = numBuffBytes/EEPROM_PAGESIZE;
	uint8_t CountMorePages = numBuffBytes%EEPROM_PAGESIZE;
	/*�ȴ�EEPROM ����*/
	while(I2C_GetFlagStatus (EEPROM_I2Cx,I2C_FLAG_BUSY)==SET)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(1);
	}
	/*�жϵ�ַ�Ƿ����*/
	if(Countaddr==0)
	{
		if(CountMorePages==0)
		{
			while(CountFullPages--)
			{
				IIC_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE);
				EEPROM_Waite_Write_End();
				addr += EEPROM_PAGESIZE;
				buff += EEPROM_PAGESIZE;
			}
		}
		else
		{
			if(CountFullPages>0)
			{
				while(CountFullPages--)
				{
					IIC_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE);
					EEPROM_Waite_Write_End();
					addr += EEPROM_PAGESIZE;
					buff += EEPROM_PAGESIZE;
				}
			}
			IIC_EEPROM_PageWrite(addr,buff,CountMorePages);
			EEPROM_Waite_Write_End();
		}
	}
	else
	{
		IIC_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE-Countaddr);
		EEPROM_Waite_Write_End();
		addr += (EEPROM_PAGESIZE-Countaddr);
		buff += (EEPROM_PAGESIZE-Countaddr);
		CountFullPages = (numBuffBytes-EEPROM_PAGESIZE+Countaddr)/EEPROM_PAGESIZE;
		CountMorePages = (numBuffBytes-EEPROM_PAGESIZE+Countaddr)%EEPROM_PAGESIZE;
		if(CountMorePages==0)
		{
			while(CountFullPages--)
			{
				IIC_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE);
				EEPROM_Waite_Write_End();
				addr += EEPROM_PAGESIZE;
				buff += EEPROM_PAGESIZE;
			}
		}
		else
		{
			if(CountFullPages>0)
			{
				while(CountFullPages--)
				{
					IIC_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE);
					EEPROM_Waite_Write_End();
					addr += EEPROM_PAGESIZE;
					buff += EEPROM_PAGESIZE;
				}
			}
			IIC_EEPROM_PageWrite(addr,buff,CountMorePages);
			EEPROM_Waite_Write_End();
		}
	}
	return 0;
}

/*��EEPROM��ȡ����
*����ִ�гɹ�����0�����򷵻ط�0
*/
uint8_t IIC_EEPROM_Read(uint16_t addr,uint8_t* data,uint8_t numReadToBytes)
{
	I2C_TimeOut = IIC_TIMEOUT;
	uint8_t addrH = (addr>>8)&0xff;
	uint8_t addrL = addr&0xff;
	/*������ʼ�ź�*/
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	/*���EV5�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(5);
	}
	/*��⵽EV5�¼�������������ַ���Ͷ�д����*/
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_DEVICE_ADDR,I2C_Direction_Transmitter);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV6�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(6);
	}
	/*��⵽EV6�¼������ʹ洢��Ԫ��ַ*/
	I2C_SendData(EEPROM_I2Cx,addrH);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(7);
	}
	/*��⵽EV8�¼������ʹ洢��Ԫ��ַ*/
	I2C_SendData(EEPROM_I2Cx,addrL);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV8�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(7);
	}
	
	/*�ٴη�����ʼ�ź�*/
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV5�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(8);
	}
	/*��⵽EV5�¼�������������ַ���Ͷ�д����*/
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_DEVICE_ADDR,I2C_Direction_Receiver);
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV6�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(9);
	}
	I2C_TimeOut = IIC_TIMEOUT;
	/*���EV7�¼�*/
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR)
	{
		I2C_TimeOut--;
		if(I2C_TimeOut==0)
			return IIC_Waite_OutTime(10);
	}
	while(numReadToBytes)
	{
		if(numReadToBytes==1)
		{
			I2C_AcknowledgeConfig(EEPROM_I2Cx,DISABLE);
		}
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR)
		{
			I2C_TimeOut--;
			if(I2C_TimeOut==0)
				return IIC_Waite_OutTime(11);
		}
		/*��⵽EV7�¼�����������*/
		*data = I2C_ReceiveData(EEPROM_I2Cx);

		data++;
		numReadToBytes--;
	}
	/*����ֹͣ�ź�*/
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	/*ͨѶ���ڽ����󣬻ָ�Ĭ������ack��Ӧʹ��*/
	I2C_AcknowledgeConfig(EEPROM_I2Cx,ENABLE);
	return 0;
}

uint8_t EEPROM_Waite_Write_End(void)
{
	I2C_TimeOut = IIC_TIMEOUT;
	do
	{
		/*������ʼ�ź�*/
		I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
		/*���EV5�¼�*/
		while(I2C_GetFlagStatus (EEPROM_I2Cx,I2C_FLAG_SB)==RESET)
		{
			I2C_TimeOut--;
			if(I2C_TimeOut==0)
				return IIC_Waite_OutTime(12);
		}
		/*��⵽EV5�¼�������������ַ���Ͷ�д����*/
		I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_DEVICE_ADDR,I2C_Direction_Transmitter);
		/*���EV6�¼�*/
	}while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_ADDR)==RESET);

	/*����stop�ź�*/
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	return 0;
}

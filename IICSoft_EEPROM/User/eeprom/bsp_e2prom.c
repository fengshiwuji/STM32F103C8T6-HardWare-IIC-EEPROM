#include "bsp_e2prom.h"

/*д�뵥���ֽ�*/
void I2C_EEPROM_WriteByte(uint16_t addr,uint8_t data)
{
	I2C_START();
	IIC_WriteByte(EEPROM_ADDR|0x00);
	I2C_Waite_Slaver_ACK(1);
	IIC_WriteByte((addr>>8)&0xff);
	I2C_Waite_Slaver_ACK(2);
	IIC_WriteByte(addr&0xff);
	I2C_Waite_Slaver_ACK(3);
	IIC_WriteByte(data);
	I2C_Waite_Slaver_ACK(4);
	I2C_STOP();
}

/*��ȡ�����ֽ�*/
uint8_t I2C_EEPROM_ReadByte(uint16_t addr)
{
	uint8_t data = 0;
	I2C_START();
	IIC_WriteByte(EEPROM_ADDR|0x00);
	I2C_Waite_Slaver_ACK(5);
	IIC_WriteByte((addr>>8)&0xff);
	I2C_Waite_Slaver_ACK(6);
	IIC_WriteByte(addr&0xff);
	I2C_Waite_Slaver_ACK(7);
	/*�ٴη�����ʼ�źţ��豸Ѱַ*/
	I2C_START();
	IIC_WriteByte(EEPROM_ADDR|0x01);
	I2C_Waite_Slaver_ACK(8);
	
	data = I2C_ReadByte(1);  //0��ʾ��Ӧ��
	data = I2C_ReadByte(0);  //0��ʾ��Ӧ��
	
	I2C_STOP();
	return data;
}
/*��ȡ����ֽ�*/
void I2C_EEPROM_ReadBytes(uint16_t addr,uint8_t*data,uint8_t numReadBytes)
{
	I2C_START();
	IIC_WriteByte(EEPROM_ADDR|0x00);
	I2C_Waite_Slaver_ACK(1);
	IIC_WriteByte((addr>>8)&0xff);
	I2C_Waite_Slaver_ACK(2);
	IIC_WriteByte(addr&0xff);
	I2C_Waite_Slaver_ACK(3);

	I2C_START();
	IIC_WriteByte(EEPROM_ADDR|0x01);
	I2C_Waite_Slaver_ACK(1);
	
	while(numReadBytes)
	{
		*data = (numReadBytes==1)?I2C_ReadByte(0):I2C_ReadByte(1);
		data++;
		numReadBytes--;
	}
	I2C_STOP();
}

/*��ַ����ҳ���룬����Ӧд������ (EEPROM_PAGESIZE-addr)���ֽ�����*/
void I2C_EEPROM_PageWrite(uint16_t addr,uint8_t* data,uint8_t numsWriteBytes)
{
	I2C_START();
	IIC_WriteByte(EEPROM_ADDR|0x00);
	I2C_Waite_Slaver_ACK(1);
	IIC_WriteByte((addr>>8)&0xff);
	I2C_Waite_Slaver_ACK(2);
	IIC_WriteByte(addr&0xff);
	I2C_Waite_Slaver_ACK(3);
	
	while(numsWriteBytes)
	{
		IIC_WriteByte(*data);
		I2C_Waite_Slaver_ACK(4);
		data++;
		numsWriteBytes--;
	}
	I2C_STOP();
}

/*��ַ�����Ƿ���룬д������ (EEPROM_SIZE(4096)-addr)���ֽ�����*/
void I2C_EEPROM_BuffWrite(uint16_t addr,uint8_t* buff,uint8_t numsBuffWrite)
{
	uint8_t CountAddr = addr%EEPROM_PAGESIZE;
	uint8_t CountFullPage = numsBuffWrite/EEPROM_PAGESIZE;
	uint8_t CountMoreBytes = numsBuffWrite%EEPROM_PAGESIZE;
	/*����ַ����*/
	if(CountAddr==0)
	{
		if(CountFullPage>0)
		{
			while(CountFullPage--)
			{
				I2C_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE);
				delay_ms(5);
				addr += EEPROM_PAGESIZE;
				buff += EEPROM_PAGESIZE;
			}
		}
		if(CountMoreBytes!=0)
			I2C_EEPROM_PageWrite(addr,buff,CountMoreBytes);
		delay_ms(5);
	}
	/*����ַ������*/
	else
	{
		I2C_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE-CountAddr);
		delay_ms(5);
		addr += EEPROM_PAGESIZE-CountAddr;
		buff += EEPROM_PAGESIZE-CountAddr;
		CountFullPage = (numsBuffWrite-EEPROM_PAGESIZE+CountAddr)/EEPROM_PAGESIZE;
		CountMoreBytes = (numsBuffWrite-EEPROM_PAGESIZE+CountAddr)%EEPROM_PAGESIZE;
		if(CountFullPage>0)
		{
			while(CountFullPage--)
			{
				I2C_EEPROM_PageWrite(addr,buff,EEPROM_PAGESIZE);
				delay_ms(5);
				addr += EEPROM_PAGESIZE;
				buff += EEPROM_PAGESIZE;
			}
		}
		if(CountMoreBytes!=0)
			I2C_EEPROM_PageWrite(addr,buff,CountMoreBytes);
		delay_ms(5);
	}
}



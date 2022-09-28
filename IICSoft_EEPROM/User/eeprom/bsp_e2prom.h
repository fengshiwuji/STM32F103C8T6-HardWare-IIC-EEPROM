#ifndef __BSP_E2PROM_H__
#define __BSP_E2PROM_H__

#include "bsp_i2c.h"

#define EEPROM_PAGESIZE				32
/*AT24C32 ´æ´¢ÈÝÁ¿4KB*/
#define EEPROM_SIZE					4096

void I2C_EEPROM_WriteByte(uint16_t addr,uint8_t data);
uint8_t I2C_EEPROM_ReadByte(uint16_t addr);
void I2C_EEPROM_PageWrite(uint16_t addr,uint8_t* data,uint8_t numsWriteBytes);
void I2C_EEPROM_ReadBytes(uint16_t addr,uint8_t*data,uint8_t numReadBytes);
void I2C_EEPROM_BuffWrite(uint16_t addr,uint8_t* buff,uint8_t numsBuffWrite);

#endif /* __BSP_E2PROM_H__ */

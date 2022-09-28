#ifndef __BSP_LED_H__
#define __BSP_LED_H__
#include "stm32f10x.h"

#define ON  			0
#define OFF				1
#define LED(N)	 		if(N) LED_GPIO_High();else LED_GPIO_Low();

#define LED_G_TOGGLE 	{LED_GPIO_PORT->ODR ^= LED_GPIO_PIN;}

#define  LED_GPIO_PIN   GPIO_Pin_13
#define  LED_GPIO_PORT  GPIOC
#define	 LED_GPIO_CLOCK RCC_APB2Periph_GPIOC

void LED_GPIO_Config(void);
void LED_GPIO_High(void);
void LED_GPIO_Low(void);

#endif

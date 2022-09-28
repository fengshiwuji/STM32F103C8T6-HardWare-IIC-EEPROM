#include "bsp_led.h"


void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//≈‰÷√ ±÷”
	RCC_APB2PeriphClockCmd(LED_GPIO_CLOCK, ENABLE);
	//≈‰÷√“˝Ω≈

	GPIO_InitStruct.GPIO_Pin =	LED_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	LED(ON);
}

void LED_GPIO_High(void)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}

void LED_GPIO_Low(void)
{
	GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}

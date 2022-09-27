#include "bsp_usart.h"

//static void NVIC_Configuration(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	/*Ƕ�������жϿ�������ѡ��*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//	/*����USARTΪ�ж�Դ*/
//	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
//	/*��ռ���ȼ�*/
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
//	/*��Ӧ���ȼ�*/
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	/*ʹ���ж�*/
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	/*��ʼ������NVIC*/
//	NVIC_Init(&NVIC_InitStructure);	
//}

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//�򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK,ENABLE);
	//�򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK,ENABLE);
	
	//��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	//��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	//���ô��ڵĹ�������
	//���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	//����֡�����ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	//��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	//�����ж����ȼ�����
//	NVIC_Configuration();

	//ʹ�ܴ��ڽ����ж�	
//	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);

	//ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);
}

void Usart_SendByte(USART_TypeDef * pUSARTx,uint8_t data)
{
	USART_SendData(pUSARTx, data);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

uint8_t USART_RecevieByte(USART_TypeDef * pUSARTx)
{
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART_ReceiveData(pUSARTx);
}

int fputc(int ch,FILE* f)
{
	USART_SendData(DEBUG_USARTx, (uint8_t) ch);
	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE)==RESET);
	return (ch);
}


int fgetc(FILE* f)
{
	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE)==RESET);
	//USART_ClearFlag(DEBUG_USARTx, USART_FLAG_RXNE);
	return ((int)USART_ReceiveData(DEBUG_USARTx));
}

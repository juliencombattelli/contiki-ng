#include "uart1.h"

#define USARTx							USART1
#define USARTx_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()            __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()          __HAL_RCC_USART1_RELEASE_RESET()

#define USARTx_TX_PIN                   GPIO_PIN_9
#define USARTx_TX_GPIO_PORT             GPIOA
#define USARTx_TX_AF                    GPIO_AF7_USART1
#define USARTx_RX_PIN                   GPIO_PIN_10
#define USARTx_RX_GPIO_PORT             GPIOA
#define USARTx_RX_AF                    GPIO_AF7_USART1

#define USARTx_IRQn                     USART1_IRQn

void uart1_arch_init(UART_HandleTypeDef *UartHandle)
{
	UartHandle->Instance        = USARTx;

	UartHandle->Init.BaudRate   = UART1_BAUDRATE;
	UartHandle->Init.WordLength = UART1_WORDLENGTH;
	UartHandle->Init.StopBits   = UART1_STOPBITS;
	UartHandle->Init.Parity     = UART1_PARITY;
	UartHandle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle->Init.Mode       = UART_MODE_TX_RX;

	HAL_UART_Init(UartHandle);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	USARTx_TX_GPIO_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();

	USARTx_CLK_ENABLE();

	GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USARTx_TX_AF;

	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = USARTx_RX_PIN;
	GPIO_InitStruct.Alternate = USARTx_RX_AF;

	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	USARTx_FORCE_RESET();
	USARTx_RELEASE_RESET();

	HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
	HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
}

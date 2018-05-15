#include <stm32l1xx.h>
#include <dev/uart1.h>
#include <dev/serial-line.h>

static UART_HandleTypeDef UartHandle;
static uint8_t byte_received;

void console_arch_init(void)
{
	uart1_arch_init(&UartHandle);
    /* Put Uart in IT reception mode */
    HAL_UART_Receive_IT(&UartHandle, &byte_received, sizeof(byte_received));
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}

int __io_getchar(void)
{
	return byte_received;
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&UartHandle);
    serial_line_input_byte(byte_received);
    HAL_UART_Receive_IT(&UartHandle, &byte_received, sizeof(byte_received));
}

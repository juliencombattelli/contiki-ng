#include <stm32l1xx.h>
#include <dev/uart1.h>

static UART_HandleTypeDef UartHandle;

void console_arch_init(void)
{
	uart1_arch_init(&UartHandle);
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}

int __io_getchar(void)
{
	int ch = 0;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}

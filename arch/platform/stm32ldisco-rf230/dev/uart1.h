#ifndef DEV_UART1_H_
#define DEV_UART1_H_

#include <stm32l1xx.h>

#define UART1_BAUDRATE		(115200u)
#define UART1_WORDLENGTH	(UART_WORDLENGTH_8B)
#define UART1_STOPBITS		(UART_STOPBITS_1)
#define UART1_PARITY		(UART_PARITY_NONE)

#ifdef __cplusplus
extern "C" {
#endif

void uart1_arch_init(UART_HandleTypeDef *UartHandle);

#ifdef __cplusplus
}
#endif

#endif /* DEV_UART1_H_ */

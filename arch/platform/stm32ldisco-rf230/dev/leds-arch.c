#include <dev/leds.h>
#include <stm32l1xx_hal.h>

#define LEDn                             2

#define LED3_PIN                         GPIO_PIN_7           /* PB.07 */
#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED4_PIN                         GPIO_PIN_6           /* PB.06 */
#define LED4_GPIO_PORT                   GPIOB
#define LED4_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

void leds_arch_init(void)
{
	GPIO_InitTypeDef  gpioinitstruct = {0};

	/* Enable the GPIO_LED Clock */
	LED3_GPIO_CLK_ENABLE();
	LED4_GPIO_CLK_ENABLE();

	/* Configure the GPIO_LED pin */
	gpioinitstruct.Pin   = LED3_PIN;
	gpioinitstruct.Mode  = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull  = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LED3_GPIO_PORT, &gpioinitstruct);

	/* Configure the GPIO_LED pin */
	gpioinitstruct.Pin   = LED4_PIN;
	gpioinitstruct.Mode  = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull  = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LED4_GPIO_PORT, &gpioinitstruct);

	/* Reset PIN to switch off the LED */
	HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
}

unsigned char leds_arch_get(void)
{
	return (HAL_GPIO_ReadPin(LED3_GPIO_PORT, LED3_PIN) ? LEDS_GREEN : 0)
		 | (HAL_GPIO_ReadPin(LED4_GPIO_PORT, LED4_PIN) ? LEDS_BLUE  : 0);
}

void leds_arch_set(unsigned char leds)
{
	HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, (leds & LEDS_GREEN) ? 0 : 1);
	HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, (leds & LEDS_BLUE)  ? 0 : 1);
}

#include "at86rf2xx-radio.h"
#include "spi2.h"

void radio_arch_init(at86rf2xx_t *radio)
{
	/* Enable GPIOB clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Common init values for GPIOs */
	GPIO_InitTypeDef gpioInit;
	gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioInit.Pull = GPIO_NOPULL;
	gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;

	/* Initialize CS pin */
	radio->hal.gpioCS = GPIOB;
	radio->hal.pinCS = GPIO_PIN_10;
	gpioInit.Pin = radio->hal.pinCS;
	HAL_GPIO_Init(radio->hal.gpioCS, &gpioInit);

	/* Initialize sleep pin */
	radio->hal.gpioSleep = GPIOB;
	radio->hal.pinSleep = GPIO_PIN_11;
	gpioInit.Pin = radio->hal.pinSleep;
	HAL_GPIO_Init(radio->hal.gpioSleep, &gpioInit);

	/* Initialize reset pin */
	radio->hal.gpioReset = GPIOB;
	radio->hal.pinReset = GPIO_PIN_12;
	gpioInit.Pin = radio->hal.pinReset;
	HAL_GPIO_Init(radio->hal.gpioReset, &gpioInit);

	/* Initialize int pin */
	radio->hal.gpioInt = GPIOB;
	radio->hal.pinInt = GPIO_PIN_2;
	gpioInit.Pin = radio->hal.pinInt;
	gpioInit.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(radio->hal.gpioInt, &gpioInit);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	/* Initialize SPI */
	spi2_arch_init(&radio->hal.hspi);

	/* Initialize Radio */
	//at86rf2xx_init(&at86rf2xx);
}

void EXTI2_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
		at86rf2xx_irq_handler();
	}
}

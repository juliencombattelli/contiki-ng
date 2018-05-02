/*
 * at86rf2xx-platform.c
 *
 *  Created on: Apr 12, 2018
 *      Author: jucom
 */

#include <at86rf2xx-hal.h>
#include <at86rf2xx-driver.h>

inline void at86rf2xx_hal_spi_write(at86rf2xx_hal_t *hal, const uint8_t *data, uint16_t size)
{
	HAL_SPI_Transmit(&hal->hspi, (uint8_t*)data, size, 0xFFFFFFFF);// ST HAL does not use 'const' for read-only parameters ...
}

inline void at86rf2xx_hal_spi_read(at86rf2xx_hal_t *hal, uint8_t *data, uint16_t size)
{
	HAL_SPI_Receive(&hal->hspi, data, size, 0xFFFFFFFF);
}

inline void at86rf2xx_hal_reset(at86rf2xx_hal_t *hal, uint8_t state)
{
	HAL_GPIO_WritePin(hal->gpioReset, hal->pinReset, state);
}

inline void at86rf2xx_hal_cs(at86rf2xx_hal_t *hal, uint8_t state)
{
	HAL_GPIO_WritePin(hal->gpioCS, hal->pinCS, state);
}

inline void at86rf2xx_hal_sleep(at86rf2xx_hal_t *hal, uint8_t state)
{
	HAL_GPIO_WritePin(hal->gpioSleep, hal->pinSleep, state);
}

inline void at86rf2xx_hal_delay(at86rf2xx_hal_t *hal, uint32_t ms)
{
	HAL_Delay(ms);
}


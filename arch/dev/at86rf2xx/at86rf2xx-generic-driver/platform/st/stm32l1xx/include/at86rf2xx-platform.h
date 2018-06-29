/*
 * at86rf2xx-platform.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jucom
 */

#ifndef AT86RF2XX_PLATFORM_ST_STM32L1XX_INCLUDE_AT86RF2XX_PLATFORM_H_
#define AT86RF2XX_PLATFORM_ST_STM32L1XX_INCLUDE_AT86RF2XX_PLATFORM_H_

#include "stm32l1xx.h"
#include <stdint.h>

/* 
 * Define at86rf2xx_hal struct for STM32L1 MCU family
 */
struct at86rf2xx_hal
{
	SPI_HandleTypeDef hspi;

	GPIO_TypeDef *gpioCS;
	uint16_t pinCS;

	GPIO_TypeDef *gpioSleep;
	uint16_t pinSleep;

	GPIO_TypeDef *gpioReset;
	uint16_t pinReset;

	GPIO_TypeDef *gpioInt;
	uint16_t pinInt;

};

#endif /* AT86RF2XX_PLATFORM_ST_STM32L1XX_INCLUDE_AT86RF2XX_PLATFORM_H_ */

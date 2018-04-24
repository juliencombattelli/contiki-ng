/*
 * at86rf2xx-driver.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jucom
 */

#ifndef AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_DRIVER_H_
#define AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_DRIVER_H_

#include <at86rf2xx.h>

extern at86rf2xx_t at86rf2xx;

void at86rf2xx_irq_handler(void);

#endif /* AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_DRIVER_H_ */

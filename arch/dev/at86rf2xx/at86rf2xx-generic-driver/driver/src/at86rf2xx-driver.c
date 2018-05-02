/*
 * at86rf2xx-instance.c
 *
 *  Created on: Apr 12, 2018
 *      Author: jucom
 */

#include <at86rf2xx-driver.h>

at86rf2xx_t at86rf2xx;

void at86rf2xx_irq_handler(void)
{
	at86rf2xx.events++;
}


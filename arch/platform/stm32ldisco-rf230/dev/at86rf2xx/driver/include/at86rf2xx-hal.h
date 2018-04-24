/*
 * at86rf2xx-hal.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jucom
 */

#ifndef AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_HAL_H_
#define AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_HAL_H_

#include <at86rf2xx-platform.h>
#include <stdint.h>

struct at86rf2xx_hal;
typedef struct at86rf2xx_hal at86rf2xx_hal_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void at86rf2xx_hal_spi_write(at86rf2xx_hal_t *hal, uint8_t *data, uint16_t size);
void at86rf2xx_hal_spi_read(at86rf2xx_hal_t *hal, uint8_t *data, uint16_t size);
void at86rf2xx_hal_reset(at86rf2xx_hal_t *hal, uint8_t state);
void at86rf2xx_hal_cs(at86rf2xx_hal_t *hal, uint8_t state);
void at86rf2xx_hal_sleep(at86rf2xx_hal_t *hal, uint8_t state);
void at86rf2xx_hal_delay(at86rf2xx_hal_t *hal, uint32_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_HAL_H_ */

#ifndef CONTINI_NG_ARCH_DEV_AT86RF2XX_AT86RF2XX_CONTIKI_DRIVER_H_
#define CONTINI_NG_ARCH_DEV_AT86RF2XX_AT86RF2XX_CONTIKI_DRIVER_H_

#include "contiki.h"
#include "dev/radio.h"
#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif 

extern at86rf2xx_t at86rf2xx;
extern const struct radio_driver at86rf2xx_driver; 

void at86rf2xx_irq_handler(void); 

#ifdef __cplusplus
}
#endif 

#endif /* CONTINI_NG_ARCH_DEV_AT86RF2XX_AT86RF2XX_CONTIKI_DRIVER_H_ */

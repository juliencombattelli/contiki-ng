#ifndef DEV_RADIO_H_
#define DEV_RADIO_H_

#include <at86rf2xx-defaults.h>
#include <at86rf2xx-driver.h>
#include <at86rf2xx-hal.h>
#include <at86rf2xx-registers.h>

#ifdef __cplusplus
extern "C" {
#endif

void radio_arch_init(at86rf2xx_t *radio);

#ifdef __cplusplus
}
#endif

#endif /* DEV_RADIO_H_ */

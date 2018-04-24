#ifndef DEV_SPI2_H_
#define DEV_SPI2_H_

#include <stm32l1xx.h>

#ifdef __cplusplus
extern "C" {
#endif

void spi2_arch_init(SPI_HandleTypeDef *SpiHandle);

#ifdef __cplusplus
}
#endif

#endif /* DEV_SPI2_H_ */

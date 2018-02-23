#ifndef CONTIKI_ARCH_PLATFORM_STM32LDISCO_RF230_CONTIKI_CONF_H_
#define CONTIKI_ARCH_PLATFORM_STM32LDISCO_RF230_CONTIKI_CONF_H_

#include <stdint.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
/* Include Project Specific conf */
#ifdef PROJECT_CONF_PATH
#include PROJECT_CONF_PATH
#endif /* PROJECT_CONF_PATH */
/*---------------------------------------------------------------------------*/
#include "stm32l1xx-def.h"
/*---------------------------------------------------------------------------*/

/* Include Board specific definitions */
#include "stm32ldisco-rf230-def.h"

#define USART_CONF_BAUDRATE 115200

/*---------------------------------------------------------------------------*/
/* Include CPU-related configuration */
#include "stm32l1xx-conf.h"
/*---------------------------------------------------------------------------*/
#endif /* CONTIKI_ARCH_PLATFORM_STM32LDISCO_RF230_CONTIKI_CONF_H_ */
/*---------------------------------------------------------------------------*/
/** @} */

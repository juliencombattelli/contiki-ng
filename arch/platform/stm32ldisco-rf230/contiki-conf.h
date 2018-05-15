#ifndef CONTIKI_ARCH_PLATFORM_STM32LDISCO_RF230_CONTIKI_CONF_H_
#define CONTIKI_ARCH_PLATFORM_STM32LDISCO_RF230_CONTIKI_CONF_H_

#include <stdint.h>
#include <string.h>
#include "at86rf2xx-contiki-driver.h"
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

#ifndef IEEE_ADDR_CONF_HARDCODED
#define IEEE_ADDR_CONF_HARDCODED             1
#endif

#ifndef IEEE_ADDR_CONF_ADDRESS
#define IEEE_ADDR_CONF_ADDRESS {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef} 
#endif

#define IEEE802154_CONF_PANID 0x0023

#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_DBG /* Only for rpl-lite */
#define LOG_CONF_LEVEL_TCPIP                       LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_IPV6                        LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_6LOWPAN                     LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_NULLNET                     LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_FRAMER                      LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_6TOP                        LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_COAP                        LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_LWM2M                       LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_MAIN                        LOG_LEVEL_DBG                 


/*---------------------------------------------------------------------------*/
/* Include CPU-related configuration */
#include "stm32l1xx-conf.h"
/*---------------------------------------------------------------------------*/
#endif /* CONTIKI_ARCH_PLATFORM_STM32LDISCO_RF230_CONTIKI_CONF_H_ */
/*---------------------------------------------------------------------------*/
/** @} */

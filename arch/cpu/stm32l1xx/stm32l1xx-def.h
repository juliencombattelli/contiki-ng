/*---------------------------------------------------------------------------*/
#ifndef CONTIKI_ARCH_CPU_STM32L1XX_STM32L1XX_DEF_H_
#define CONTIKI_ARCH_CPU_STM32L1XX_STM32L1XX_DEF_H_
/*---------------------------------------------------------------------------*/
#include "cm3/cm3-def.h"
#include "stm32l1xx.h"
#include "stm32l1xx_hal.h"
/*---------------------------------------------------------------------------*/
#define F_CPU 32000000
#define RTIMER_ARCH_SECOND 32768
/*---------------------------------------------------------------------------*/
/* 352us from calling transmit() until the SFD byte has been sent */
#define RADIO_DELAY_BEFORE_TX     ((unsigned)US_TO_RTIMERTICKS(352))
/* 192us as in datasheet but ACKs are not always received, so adjusted to 250us */
#define RADIO_DELAY_BEFORE_RX     ((unsigned)US_TO_RTIMERTICKS(250))
#define RADIO_DELAY_BEFORE_DETECT 0
#ifndef TSCH_CONF_BASE_DRIFT_PPM
/* The drift compared to "true" 10ms slots.
 * Enable adaptive sync to enable compensation for this.
 * Slot length 10000 usec
 *             328 ticks
 * Tick duration 30.517578125 usec
 * Real slot duration 10009.765625 usec
 * Target - real duration = -9.765625 usec
 * TSCH_CONF_BASE_DRIFT_PPM -977
 */
#define TSCH_CONF_BASE_DRIFT_PPM -977
#endif

#if MAC_CONF_WITH_TSCH
#define TSCH_CONF_HW_FRAME_FILTERING  0
#endif /* MAC_CONF_WITH_TSCH */
/*---------------------------------------------------------------------------*/
/* Path to headers with implementation of mutexes and memory barriers */
#define MUTEX_CONF_ARCH_HEADER_PATH          "mutex-cortex.h"
#define MEMORY_BARRIER_CONF_ARCH_HEADER_PATH "memory-barrier-cortex.h"
/*---------------------------------------------------------------------------*/
#endif /* CONTIKI_ARCH_CPU_STM32L1XX_STM32L1XX_DEF_H_  */
/*---------------------------------------------------------------------------*/

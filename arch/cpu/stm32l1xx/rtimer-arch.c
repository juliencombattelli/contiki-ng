/*
 * Copyright (c) 2012, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 */
/*---------------------------------------------------------------------------*/
#include "stm32l1xx-def.h"
#include "contiki.h"
#include "sys/rtimer.h"
#include "sys/process.h"
#include "sys/energest.h"
#include "dev/watchdog.h"

#include "stm32l1xx.h"
#include "stm32l1xx_hal.h"

/* The prescaler assumes the timer is sourced with same clock speed as the CPU */
#define RTIMER_PRESCALER 				(F_CPU / (RTIMER_SECOND*2))

static volatile rtimer_clock_t rtimer_clock = 0;
static volatile rtimer_clock_t timeout_value = 0;

TIM_HandleTypeDef htim2;

/*
 * @brief Timer 2 IRQ Handler
 *        It is responsible for the clock update
 */
void TIM2_IRQHandler(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);

	/* Update clock */
    rtimer_clock++;

	/* clear interrupt pending flag */
	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

	/* Check for, and run, any expired rtimers */
	if(rtimer_clock == timeout_value)
		rtimer_run_next();

	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/*
 * @brief Initialize the rtimer module
 */
void rtimer_arch_init(void)
{
	TIM_ClockConfigTypeDef clock_source_config;
	TIM_OC_InitTypeDef oc_config;

	/* Enable the clock of the timer2 peripheral */
    __TIM2_CLK_ENABLE();

    /* Configure timer2 instance */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = RTIMER_PRESCALER;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 1;
	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_Base_Start_IT(&htim2);

    /* Configure clock source for timer2 */
	clock_source_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim2, &clock_source_config);

    /* Output Compare configuration */
	HAL_TIM_OC_Init(&htim2);
	oc_config.OCMode = TIM_OCMODE_TIMING;
	oc_config.Pulse = 0;
	oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
	HAL_TIM_OC_ConfigChannel(&htim2, &oc_config, TIM_CHANNEL_1);

	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

	/* Enable TIM2 Update interrupt */
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

    /* Enable TIM2
	__HAL_TIM_ENABLE(&htim2);

    /* Configure NVIC */
	HAL_NVIC_SetPriority((IRQn_Type)TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ((IRQn_Type)(TIM2_IRQn));
}

/*
 * @brief Retrieve the actual clock value
 */ 
rtimer_clock_t rtimer_arch_now(void)
{
  return rtimer_clock;
}

/*
 * @brief Schedule a new timeout, overriding older one
 */
void rtimer_arch_schedule(rtimer_clock_t t)
{
	timeout_value = rtimer_clock + t;
}
/*---------------------------------------------------------------------------*/

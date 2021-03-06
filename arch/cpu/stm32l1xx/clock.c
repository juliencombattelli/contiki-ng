#include <stdio.h>
#include <contiki.h>
#include <sys/energest.h>
#include <stm32l1xx_hal.h>

/* After how many clock cycles should the systick interrupt be fired */
#define RELOAD_VALUE			((F_CPU/CLOCK_CONF_SECOND) - 1)

static volatile unsigned long seconds;
//static volatile clock_time_t ticks;

/* This interrupt handler will increase the tick counter */
void SysTick_Handler(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);

    /* Update clock */
    HAL_IncTick();
	
    /* Update seconds */
    if((HAL_GetTick() % CLOCK_SECOND) == 0)
    {
		seconds++;
		energest_flush();
	}

	/* If an etimer expired, continue its process */
	if(etimer_pending())
		etimer_request_poll();

	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/* 
 * @brief Overload the HAL_InitTick function in stm32l1xx_hal.c
 *        It is called by HAL_Init
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	/* Configure the SysTick to have interrupt in 1ms time basis */
	HAL_SYSTICK_Config(RELOAD_VALUE);

	/* Configure the SysTick IRQ priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);

	/* Return function status */
	return HAL_OK;
}

/*
 * @brief Initialize the clock module
 */
void clock_init(void)
{
	//ticks = 0;
	seconds = 0;
}

/*
 * @brief Retrieve the current second count
 */
unsigned long clock_seconds(void)
{
	return seconds;
}

/*
 * @brief Set the clock
 */
void clock_set_seconds(unsigned long sec)
{
	seconds = sec;
}

/*
 * @brief Retrieve the current clock tick count
 */
clock_time_t clock_time(void)
{
	//return ticks;
    return HAL_GetTick();
}

/* 
 * @brief Busy-wait the CPU for a duration depending on CPU speed 
 */
void clock_delay(unsigned int i)
{
	/*for(; i > 0; i--)
	{
		unsigned int j;
		for(j = 50; j > 0; j--)
		{
			__NOP();
		}
	}*/
    HAL_Delay(i);
}

/* 
 * @brief Wait for a multiple of clock ticks (7.8ms per tick at 128Hz) 
 */
void clock_wait(clock_time_t i)
{
	clock_time_t start;
	start = clock_time();
	while(clock_time() - start < i);
}

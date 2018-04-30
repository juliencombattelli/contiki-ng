/*
 * at86rf2xx.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jucom
 */

#ifndef AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_H_
#define AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_H_

#include <at86rf2xx-hal.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief   Transition time from SLEEP to TRX_OFF in us, refer figure 7-4, p.42.
 *          For different environments refer figure 13-13, p.201
 */
#define AT86RF2XX_WAKEUP_DELAY          (300U)

/**
 * @brief   Minimum reset pulse width, refer p.190
 */
#define AT86RF2XX_RESET_PULSE_WIDTH     (1U)

/**
 * @brief   Transition time to TRX_OFF after reset pulse in us, refer
 *          figure 7-8, p. 44.
 */
#define AT86RF2XX_RESET_DELAY           (26U)

/**
  * @brief   Frequency configuration for sub-GHz devices.
  * @{
  */
#ifdef MODULE_AT86RF212B
typedef enum {
    AT86RF2XX_FREQ_915MHZ,       /**< frequency 915MHz enabled */
    AT86RF2XX_FREQ_868MHZ,       /**< frequency 868MHz enabled */
} at86rf2xx_freq_t;
#endif
/** @} */

typedef struct
{
	at86rf2xx_hal_t     hal;            /**< hardware devices to communicate with the radio*/
    uint8_t             state;          /**< current state of the radio */
    uint8_t             seq_nr;         /**< sequence number to use next */
    uint8_t             frame_len;      /**< length of the current TX frame */
    uint16_t            pan;            /**< currently used PAN ID */
    uint8_t             chan;           /**< currently used channel */
#ifdef MODULE_AT86RF212B
    at86rf2xx_freq_t    freq;           /**< currently used frequency */
#endif
    uint8_t             addr_short[2];  /**< the radio's short address */
    uint8_t             addr_long[8];   /**< the radio's long address */
    uint16_t            options;        /**< state of used options */
    uint8_t             idle_state;     /**< state to return to after sending */
    volatile int        events;         /**< # of pending interrupts from the radio */

} at86rf2xx_t;

int         at86rf2xx_init(at86rf2xx_t *dev);
void        at86rf2xx_reset(at86rf2xx_t *dev);
uint16_t    at86rf2xx_get_addr_short(at86rf2xx_t *dev);
void        at86rf2xx_set_addr_short(at86rf2xx_t *dev, uint16_t addr);
uint64_t    at86rf2xx_get_addr_long(at86rf2xx_t *dev);
void        at86rf2xx_set_addr_long(at86rf2xx_t *dev, uint64_t addr);
uint8_t	    at86rf2xx_get_chan(at86rf2xx_t *dev);
void        at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t chan);
uint16_t    at86rf2xx_get_pan(at86rf2xx_t *dev);
void        at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan);
int16_t     at86rf2xx_get_txpower(at86rf2xx_t *dev);
void        at86rf2xx_set_txpower(at86rf2xx_t *dev, int16_t txpower);
uint8_t     at86rf2xx_get_max_retries(at86rf2xx_t *dev);
void        at86rf2xx_set_max_retries(at86rf2xx_t *dev, uint8_t max);
uint8_t     at86rf2xx_get_csma_max_retries(at86rf2xx_t *dev);
void        at86rf2xx_set_csma_max_retries(at86rf2xx_t *dev, int8_t retries);
void        at86rf2xx_set_csma_backoff_exp(at86rf2xx_t *dev, uint8_t min, uint8_t max);
void        at86rf2xx_set_csma_seed(at86rf2xx_t *dev, uint8_t entropy[2]);
void        at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state);
void        at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state);
void        at86rf2xx_reset_state_machine(at86rf2xx_t *dev);
size_t      at86rf2xx_send(at86rf2xx_t *dev, uint8_t *data, size_t len);
void        at86rf2xx_tx_prepare(at86rf2xx_t *dev);
size_t      at86rf2xx_tx_load(at86rf2xx_t *dev, uint8_t *data, size_t len, size_t offset);
void        at86rf2xx_tx_exec(at86rf2xx_t *dev);
size_t      at86rf2xx_rx_len(at86rf2xx_t *dev);
void        at86rf2xx_rx_read(at86rf2xx_t *dev, uint8_t *data, size_t len, size_t offset);
uint8_t     at86rf2xx_reg_read(at86rf2xx_t *dev, uint8_t addr);
void        at86rf2xx_reg_write(at86rf2xx_t *dev, uint8_t addr, uint8_t value);
void        at86rf2xx_sram_read(at86rf2xx_t *dev, uint8_t offset, uint8_t *data, size_t len);
void        at86rf2xx_sram_write(at86rf2xx_t *dev, uint8_t offset, uint8_t *data, size_t len);
void        at86rf2xx_fb_read(at86rf2xx_t *dev, uint8_t *data, size_t len);
void        at86rf2xx_force_trx_off(at86rf2xx_t *dev);
uint8_t     at86rf2xx_get_status(at86rf2xx_t *dev);
void        at86rf2xx_assert_awake(at86rf2xx_t *dev);
void        at86rf2xx_hardware_reset(at86rf2xx_t *dev);

#endif /* AT86RF2XX_DRIVER_INCLUDE_AT86RF2XX_H_ */

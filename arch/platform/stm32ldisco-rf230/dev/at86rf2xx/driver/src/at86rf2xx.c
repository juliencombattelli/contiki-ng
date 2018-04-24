#include <at86rf2xx.h>
#include <at86rf2xx-defaults.h>
#include <at86rf2xx-registers.h>
#include <ieee802154.h>
#include <stdbool.h>

#ifdef AT86RF2XX_DEBUG
	#define AT86RF2XX_PRINTF(...) printf(__VA_ARGS__)
#else
    #define AT86RF2XX_PRINTF(...)
#endif

#ifdef MODULE_AT86RF212B
/* See: Table 9-15. Recommended Mapping of TX Power, Frequency Band, and
 * PHY_TX_PWR (register 0x05), AT86RF212B data sheet. */
static const uint8_t dbm_to_tx_pow_868[] = {0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18,
                                            0x17, 0x15, 0x14, 0x13, 0x12, 0x11,
                                            0x10, 0x0f, 0x31, 0x30, 0x2f, 0x94,
                                            0x93, 0x91, 0x90, 0x29, 0x49, 0x48,
                                            0x47, 0xad, 0xcd, 0xcc, 0xcb, 0xea,
                                            0xe9, 0xe8, 0xe7, 0xe6, 0xe4, 0x80,
                                            0xa0};
static const uint8_t dbm_to_tx_pow_915[] = {0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x17,
                                            0x16, 0x15, 0x14, 0x13, 0x12, 0x11,
                                            0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b,
                                            0x09, 0x91, 0x08, 0x07, 0x05, 0x27,
                                            0x04, 0x03, 0x02, 0x01, 0x00, 0x86,
                                            0x40, 0x84, 0x83, 0x82, 0x80, 0xc1,
                                            0xc0};
int16_t tx_pow_to_dbm(at86rf2xx_freq_t freq, uint8_t reg)
{
    for(int i = 0; i < 37; i++)
    {
        if(freq == AT86RF2XX_FREQ_868MHZ)
        {
            if (dbm_to_tx_pow_868[i] == reg)
            {
                return i -25;
            }
        }
        else if (freq == AT86RF2XX_FREQ_915MHZ)
        {
            if (dbm_to_tx_pow_915[i] == reg)
            {
                return i -25;
            }
        }
    }
    return 0;
}

#elif MODULE_AT86RF233
static const int16_t tx_pow_to_dbm[] = {4, 4, 3, 3, 2, 2, 1,
                                        0, -1, -2, -3, -4, -6, -8, -12, -17};
static const uint8_t dbm_to_tx_pow[] = {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                        0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
                                        0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
                                        0x06, 0x05, 0x03,0x00};
#else
static const int16_t tx_pow_to_dbm[] = {3, 3, 2, 2, 1, 1, 0,
                                        -1, -2, -3, -4, -5, -7, -9, -12, -17};
static const uint8_t dbm_to_tx_pow[] = {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                        0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b,
                                        0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                        0x05, 0x03, 0x00};
#endif

bool _at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t tmp;
    uint8_t status;

    at86rf2xx_assert_awake(dev);

    /* trigger CCA measurment */
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, tmp);

    /* wait for result to be ready */
    do
    {
        status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    }
    while (!(status & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE));

    /* return according to measurement */
    if (status & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int at86rf2xx_init(at86rf2xx_t *dev)
{
	/* initialize state machine */
	dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
	dev->state = AT86RF2XX_STATE_SLEEP;

	/*  wait for SPI to be ready  */
	at86rf2xx_hal_delay(&dev->hal, 10);

	/*  initialize GPIOs */
	at86rf2xx_hal_sleep(&dev->hal, 0);
	at86rf2xx_hal_reset(&dev->hal, 1);
	at86rf2xx_hal_cs(&dev->hal, 1);

	/* make sure device is not sleeping, so we can query part number */
	at86rf2xx_assert_awake(dev);

	/* test if the SPI is set up correctly and the device is responding */
	uint8_t part_num = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM);
	if (part_num != AT86RF233_PARTNUM)
	{
		printf("[at86rf2xx] Error: unable to read correct part number (%x).\r\n", part_num);
		return -1;
	}
	printf("[at86rf2xx] Detected part #: 0x%x\r\n", part_num);
	printf("[at86rf2xx] Version: 0x%x\r\n", at86rf2xx_reg_read(dev, AT86RF2XX_REG__VERSION_NUM));

	/* reset device to default values and put it into RX state */
	at86rf2xx_reset(dev);

	return 0;
}

void at86rf2xx_reset(at86rf2xx_t *dev)
{
	at86rf2xx_hardware_reset(dev);

    /* Reset state machine to ensure a known state */
	at86rf2xx_reset_state_machine(dev);

    /* reset options and sequence number */
    dev->seq_nr = 0;
    dev->options = 0;

    /* set short and long address */
    at86rf2xx_set_addr_long(dev, AT86RF2XX_DEFAULT_ADDR_LONG);
    at86rf2xx_set_addr_short(dev, AT86RF2XX_DEFAULT_ADDR_SHORT);

    /* set default PAN id */
    at86rf2xx_set_pan(dev, AT86RF2XX_DEFAULT_PANID);

    /* set default channel */
    at86rf2xx_set_chan(dev, AT86RF2XX_DEFAULT_CHANNEL);

    /* set default TX power */
    at86rf2xx_set_txpower(dev, AT86RF2XX_DEFAULT_TXPOWER);

    /* set default options */
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_PROMISCUOUS, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START, true);
    at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END, true);

    /* enable safe mode (protect RX FIFO until reading data starts) */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, AT86RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE);

#ifdef MODULE_AT86RF212B
    at86rf2xx_set_freq(dev, AT86RF2XX_FREQ_915MHZ);
#endif

    /* don't populate masked interrupt flags to IRQ_STATUS register */
    /*uint8_t tmp = at86rf2xx_reg_read(AT86RF2XX_REG__TRX_CTRL_1);
    tmp &= ~(AT86RF2XX_TRX_CTRL_1_MASK__IRQ_MASK_MODE);
    at86rf2xx_reg_write(AT86RF2XX_REG__TRX_CTRL_1, tmp);*/

    /* disable clock output to save power */
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);

    /* enable interrupts */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, AT86RF2XX_IRQ_STATUS_MASK__TRX_END);

    /* clear interrupt flags */
    at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    /* go into RX state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);

    printf("[at86rf2xx] Reset complete.\r\n");
}

uint16_t at86rf2xx_get_addr_short(at86rf2xx_t *dev)
{
	return (dev->addr_short[0] << 8) | dev->addr_short[1];
}

void at86rf2xx_set_addr_short(at86rf2xx_t *dev, uint16_t addr)
{
    dev->addr_short[0] = addr >> 8;
    dev->addr_short[1] = addr & 0xff;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_0, dev->addr_short[0]);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__SHORT_ADDR_1, dev->addr_short[1]);
}

uint64_t at86rf2xx_get_addr_long(at86rf2xx_t *dev)
{
    uint64_t addr;
    uint8_t *ap = (uint8_t *)(&addr);
    for (int i = 0; i < 8; i++)
    {
        ap[i] = dev->addr_long[7 - i];
    }
    return addr;
}

void at86rf2xx_set_addr_long(at86rf2xx_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++)
    {
        dev->addr_long[i] = (addr >> ((7 - i) * 8));
        at86rf2xx_reg_write(dev, (AT86RF2XX_REG__IEEE_ADDR_0 + i), dev->addr_long[i]);
    }
}

__INLINE uint8_t at86rf2xx_get_chan(at86rf2xx_t *dev)
{
	return dev->chan;
}

void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t chan)
{
    uint8_t tmp;

    if (chan < AT86RF2XX_MIN_CHANNEL || chan > AT86RF2XX_MAX_CHANNEL)
    {
        return;
    }

    dev->chan = chan;
    tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    tmp &= ~(AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    tmp |= (chan & AT86RF2XX_PHY_CC_CCA_MASK__CHANNEL);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, tmp);
}

#ifdef MODULE_AT86RF212B
at86rf2xx_freq_t at86rf2xx_get_freq(at86rf2xx_t *dev)
{
    return dev->freq;
}

void at86rf2xx_set_freq(at86rf2xx_t *dev, at86rf2xx_freq_t freq_)
{
    uint8_t trx_ctrl2 = 0, rf_ctrl0 = 0;
    trx_ctrl2 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_2);
    trx_ctrl2 &= ~(AT86RF2XX_TRX_CTRL_2_MASK__FREQ_MODE);
    rf_ctrl0 = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RF_CTRL_0);
    /* Erase previous conf for GC_TX_OFFS */
    rf_ctrl0 &= ~AT86RF2XX_RF_CTRL_0_MASK__GC_TX_OFFS;

    trx_ctrl2 |= AT86RF2XX_TRX_CTRL_2_MASK__SUB_MODE;
    rf_ctrl0 |= AT86RF2XX_RF_CTRL_0_GC_TX_OFFS__2DB;

    switch(freq_)
    {
        case AT86RF2XX_FREQ_915MHZ:
            if (dev->chan == 0)
            {
            	at86rf2xx_set_chan(dev, AT86RF2XX_DEFAULT_CHANNEL);
            }
            else
            {
            	at86rf2xx_set_chan(dev, dev->chan);
            }
            break;

        case AT86RF2XX_FREQ_868MHZ:
            /* Channel = 0 for 868MHz means 868.3MHz, only one available */
        	at86rf2xx_set_chan(dev, 0x00);
            break;

        default:
            //AT86RF2XX_PRINTF("at86rf2xx: Trying to set unknown frequency 0x%lx\n",
            //    (unsigned long) freq);
            return;
    }
    dev->freq = freq_;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_2, trx_ctrl2);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RF_CTRL_0, rf_ctrl0);
}
#endif

__INLINE uint16_t at86rf2xx_get_pan(at86rf2xx_t *dev)
{
	return dev->pan;
}

void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan)
{
    dev->pan = pan;
    //AT86RF2XX_PRINTF("pan0: %u, pan1: %u\r\n", (uint8_t)pan, pan >> 8);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_0, (uint8_t)dev->pan);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PAN_ID_1, (dev->pan >> 8));
}

int16_t at86rf2xx_get_txpower(at86rf2xx_t *dev)
{
#ifdef MODULE_AT86RF212B
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR);
    //AT86RF2XX_PRINTF("txpower value: %x\n", txpower);
    return tx_pow_to_dbm(dev->freq, txpower);
#else
    uint8_t txpower = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_TX_PWR) & AT86RF2XX_PHY_TX_PWR_MASK__TX_PWR;
    return tx_pow_to_dbm[txpower];
#endif
}

void at86rf2xx_set_txpower(at86rf2xx_t *dev, int16_t txpower)
{
#ifdef MODULE_AT86RF212B
    txpower += 25;
#else
    txpower += 17;
#endif
    if (txpower < 0)
    {
        txpower = 0;
#ifdef MODULE_AT86RF212B
    }
    else if (txpower > 36)
    {
        txpower = 36;
#elif MODULE_AT86RF233
    }
    else if (txpower > 21)
    {
        txpower = 21;
#else
    }
    else if (txpower > 20)
    {
        txpower = 20;
#endif
    }
#ifdef MODULE_AT86RF212B
    if (dev->freq == AT86RF2XX_FREQ_915MHZ)
    {
    	at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR, dbm_to_tx_pow_915[txpower]);
    }
    else if (dev->freq == AT86RF2XX_FREQ_868MHZ)
    {
    	at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR, dbm_to_tx_pow_868[txpower]);
    }
    else
    {
        return;
    }
#else
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_TX_PWR, dbm_to_tx_pow[txpower]);
#endif
}

__INLINE uint8_t at86rf2xx_get_max_retries(at86rf2xx_t *dev)
{
	return (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0) >> 4);
}

void at86rf2xx_set_max_retries(at86rf2xx_t *dev, uint8_t max)
{
    max = (max > 7) ? 7 : max;
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_FRAME_RETRIES);
    tmp |= (max << 4);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

uint8_t at86rf2xx_get_csma_max_retries(at86rf2xx_t *dev)
{
    uint8_t tmp;
    tmp  = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES;
    tmp >>= 1;
    return tmp;
}

void at86rf2xx_set_csma_max_retries(at86rf2xx_t *dev, int8_t retries)
{
    retries = (retries > 5) ? 5 : retries; /* valid values: 0-5 */
    retries = (retries < 0) ? 7 : retries; /* max < 0 => disable CSMA (set to 7) */
    AT86RF2XX_PRINTF("[at86rf2xx] opt: Set CSMA retries to %u\r\n", retries);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_0);
    tmp &= ~(AT86RF2XX_XAH_CTRL_0__MAX_CSMA_RETRIES);
    tmp |= (retries << 1);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_0, tmp);
}

void at86rf2xx_set_csma_backoff_exp(at86rf2xx_t *dev, uint8_t min, uint8_t max)
{
    max = (max > 8) ? 8 : max;
    min = (min > max) ? max : min;
    AT86RF2XX_PRINTF("[at86rf2xx] opt: Set min BE=%u, max BE=%u\r\n", min, max);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_BE, (max << 4) | (min));
}

void at86rf2xx_set_csma_seed(at86rf2xx_t *dev, uint8_t entropy[2])
{
    if(entropy == NULL)
    {
        AT86RF2XX_PRINTF("[at86rf2xx] opt: CSMA seed entropy is nullpointer\r\n");
        return;
    }
    AT86RF2XX_PRINTF("[at86rf2xx] opt: Set CSMA seed to 0x%x 0x%x\r\n", entropy[0], entropy[1]);

    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_0, entropy[0]);

    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
    tmp &= ~(AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1);
    tmp |= entropy[1] & AT86RF2XX_CSMA_SEED_1__CSMA_SEED_1;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
}

void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state)
{
	uint8_t tmp;

	//AT86RF2XX_PRINTF("set option %i to %i\n", option, state);

	/* set option field */
	if (state)
	{
		dev->options |= option;
		/* trigger option specific actions */
		switch (option)
		{
			case AT86RF2XX_OPT_CSMA:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: enabling CSMA mode (4 retries, min BE: 3 max BE: 5)\r\n");
				/* Initialize CSMA seed with hardware address */
				at86rf2xx_set_csma_seed(dev, dev->addr_long);
				at86rf2xx_set_csma_max_retries(dev, 4);
				at86rf2xx_set_csma_backoff_exp(dev, 3, 5);
				break;
			case AT86RF2XX_OPT_PROMISCUOUS:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: enabling PROMISCUOUS mode\r\n");
				/* disable auto ACKs in promiscuous mode */
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
				tmp |= AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK;
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
				/* enable promiscuous mode */
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
				tmp |= AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE;
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
				break;
			case AT86RF2XX_OPT_AUTOACK:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: enabling auto ACKs\r\n");
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
				tmp &= ~(AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
				break;
			case AT86RF2XX_OPT_TELL_RX_START:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: enabling SFD IRQ\r\n");
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
				tmp |= AT86RF2XX_IRQ_STATUS_MASK__RX_START;
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);
				break;
			default:
				/* do nothing */
				break;
		}
	}
	else
	{
		dev->options &= ~(option);
		/* trigger option specific actions */
		switch (option)
		{
			case AT86RF2XX_OPT_CSMA:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: disabling CSMA mode\r\n");
				/* setting retries to -1 means CSMA disabled */
				at86rf2xx_set_csma_max_retries(dev, -1);
				break;
			case AT86RF2XX_OPT_PROMISCUOUS:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: disabling PROMISCUOUS mode\r\n");
				/* disable promiscuous mode */
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_1);
				tmp &= ~(AT86RF2XX_XAH_CTRL_1__AACK_PROM_MODE);
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__XAH_CTRL_1, tmp);
				/* re-enable AUTOACK only if the option is set */
				if (dev->options & AT86RF2XX_OPT_AUTOACK)
				{
					tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
					tmp &= ~(AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK);
					at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
				}
				break;
			case AT86RF2XX_OPT_AUTOACK:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: disabling auto ACKs\r\n");
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
				tmp |= AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK;
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__CSMA_SEED_1, tmp);
				break;
			case AT86RF2XX_OPT_TELL_RX_START:
				AT86RF2XX_PRINTF("[at86rf2xx] opt: disabling SFD IRQ\r\n");
				tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_MASK);
				tmp &= ~AT86RF2XX_IRQ_STATUS_MASK__RX_START;
				at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, tmp);
				break;
			default:
				/* do nothing */
				break;
		}
	}
}

__STATIC_INLINE void _at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state)
{
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, state);
    while (at86rf2xx_get_status(dev) != state);
    dev->state = state;
}

void at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state_)
{
	uint8_t old_state = at86rf2xx_get_status(dev);

	    if (state_ == old_state)
	    {
	        return;
	    }
	    /* make sure there is no ongoing transmission, or state transition already
	     * in progress */
	    while (old_state == AT86RF2XX_STATE_BUSY_RX_AACK ||
	           old_state == AT86RF2XX_STATE_BUSY_TX_ARET ||
	           old_state == AT86RF2XX_STATE_IN_PROGRESS)
	    {
	        old_state = at86rf2xx_get_status(dev);
	    }

	    /* we need to go via PLL_ON if we are moving between RX_AACK_ON <-> TX_ARET_ON */
	    if ((old_state == AT86RF2XX_STATE_RX_AACK_ON &&
	             state_ == AT86RF2XX_STATE_TX_ARET_ON) ||
	        (old_state == AT86RF2XX_STATE_TX_ARET_ON &&
	             state_ == AT86RF2XX_STATE_RX_AACK_ON))
	    {
	        _at86rf2xx_set_state(dev, AT86RF2XX_STATE_PLL_ON);
	    }
	    /* check if we need to wake up from sleep mode */
	    else if (old_state == AT86RF2XX_STATE_SLEEP)
	    {
	        AT86RF2XX_PRINTF("[at86rf2xx] status: waking up from sleep mode\r\n");
	        at86rf2xx_assert_awake(dev);
	    }

	    if (state_ == AT86RF2XX_STATE_SLEEP)
	    {
	        /* First go to TRX_OFF */
	    	at86rf2xx_force_trx_off(dev);
	        /* Discard all IRQ flags, framebuffer is lost anyway */
	        at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);
	        /* Go to SLEEP mode from TRX_OFF */
	        at86rf2xx_hal_sleep(&dev->hal, 1);
	        dev->state = state_;
	    }
	    else
	    {
	        _at86rf2xx_set_state(dev, state_);
	    }
}

void at86rf2xx_reset_state_machine(at86rf2xx_t *dev)
{
    uint8_t old_state;

    at86rf2xx_assert_awake(dev);

    /* Wait for any state transitions to complete before forcing TRX_OFF */
    do
    {
        old_state = at86rf2xx_get_status(dev);
    }
    while (old_state == AT86RF2XX_STATE_IN_PROGRESS);

    at86rf2xx_force_trx_off(dev);
}

size_t at86rf2xx_send(at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    /* check data length */
    if (len > AT86RF2XX_MAX_PKT_LENGTH)
    {
        printf("[at86rf2xx] Error: Data to send exceeds max packet size.\r\n");
        return 0;
    }
    at86rf2xx_tx_prepare(dev);
    at86rf2xx_tx_load(dev, data, len, 0);
    at86rf2xx_tx_exec(dev);

    return len;
}

void at86rf2xx_tx_prepare(at86rf2xx_t *dev)
{
    uint8_t state;

    /* make sure ongoing transmissions are finished */
    do
    {
        state = at86rf2xx_get_status(dev);
    }
    while (state == AT86RF2XX_STATE_BUSY_TX_ARET);

    /* if receiving cancel */
    if(state == AT86RF2XX_STATE_BUSY_RX_AACK)
    {
    	at86rf2xx_force_trx_off(dev);
        dev->idle_state = AT86RF2XX_STATE_RX_AACK_ON;
    }
    else if (state != AT86RF2XX_STATE_TX_ARET_ON)
    {
        dev->idle_state = state;
    }
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
    dev->frame_len = IEEE802154_FCS_LEN;
}

size_t at86rf2xx_tx_load(at86rf2xx_t *dev, uint8_t *data, size_t len, size_t offset)
{
    dev->frame_len += (uint8_t)len;
    at86rf2xx_sram_write(dev, offset + 1, data, len);
    return offset + len;
}

void at86rf2xx_tx_exec(at86rf2xx_t *dev)
{
    /* write frame length field in FIFO */
	at86rf2xx_sram_write(dev, 0, &(dev->frame_len), 1);
    /* trigger sending of pre-loaded frame */
	at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_TRX_STATE__TX_START);
    /*if (at86rf2xx.event_cb && (at86rf2xx.options & AT86RF2XX_OPT_TELL_TX_START)) {
        at86rf2xx.event_cb(NETDEV_EVENT_TX_STARTED, NULL);
    }*/
}

size_t at86rf2xx_rx_len(at86rf2xx_t *dev)
{
    uint8_t phr;
    at86rf2xx_fb_read(dev, &phr, 1);

    /* ignore MSB (refer p.80) and substract length of FCS field */
    return (size_t)((phr & 0x7f) - 2);
}

void at86rf2xx_rx_read(at86rf2xx_t *dev, uint8_t *data, size_t len, size_t offset)
{
    /* when reading from SRAM, the different chips from the AT86RF2xx family
     * behave differently: the AT86F233, the AT86RF232 and the ATRF86212B return
     * frame length field (PHR) at position 0 and the first data byte at
     * position 1.
     * The AT86RF231 does not return the PHR field and return
     * the first data byte at position 0.
     */
#ifndef MODULE_AT86RF231
	at86rf2xx_sram_read(dev, offset + 1, data, len);
#else
	at86rf2xx_sram_read(dev, offset, data, len);
#endif
}

uint8_t at86rf2xx_reg_read(at86rf2xx_t *dev, uint8_t addr)
{
    uint8_t value = 0;
    uint8_t readCommand = addr | AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_READ;
    at86rf2xx_hal_cs(&dev->hal, 0);
	at86rf2xx_hal_spi_write(&dev->hal, &readCommand, 1);
	at86rf2xx_hal_spi_read(&dev->hal, &value, 1);
	at86rf2xx_hal_cs(&dev->hal, 1);
    return (uint8_t)value;
}

void at86rf2xx_reg_write(at86rf2xx_t *dev, uint8_t addr, uint8_t value)
{
    uint8_t writeCommand = addr | AT86RF2XX_ACCESS_REG | AT86RF2XX_ACCESS_WRITE;
    at86rf2xx_hal_cs(&dev->hal, 0);
	at86rf2xx_hal_spi_write(&dev->hal, &writeCommand, 1);
	at86rf2xx_hal_spi_read(&dev->hal, &value, 1);
	at86rf2xx_hal_cs(&dev->hal, 1);
}

void at86rf2xx_sram_read(at86rf2xx_t *dev, uint8_t offset, uint8_t *data, size_t len)
{
    uint8_t readCommand = AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_READ;
    at86rf2xx_hal_cs(&dev->hal, 0);
	at86rf2xx_hal_spi_write(&dev->hal, &readCommand, 1);
	at86rf2xx_hal_spi_write(&dev->hal, &offset, 1);
	at86rf2xx_hal_spi_read(&dev->hal, data, len);
	at86rf2xx_hal_cs(&dev->hal, 1);
}

void at86rf2xx_sram_write(at86rf2xx_t *dev, uint8_t offset, uint8_t *data, size_t len)
{
    uint8_t writeCommand = AT86RF2XX_ACCESS_SRAM | AT86RF2XX_ACCESS_WRITE;
    at86rf2xx_hal_cs(&dev->hal, 0);
	at86rf2xx_hal_spi_write(&dev->hal, &writeCommand, 1);
	at86rf2xx_hal_spi_write(&dev->hal, &offset, 1);
	at86rf2xx_hal_spi_read(&dev->hal, data, len);
	at86rf2xx_hal_cs(&dev->hal, 1);
}

void at86rf2xx_fb_read(at86rf2xx_t *dev, uint8_t *data, size_t len)
{
    uint8_t readCommand = AT86RF2XX_ACCESS_FB | AT86RF2XX_ACCESS_READ;
    at86rf2xx_hal_cs(&dev->hal, 0);
	at86rf2xx_hal_spi_write(&dev->hal, &readCommand, 1);
	at86rf2xx_hal_spi_read(&dev->hal, data, len);
	at86rf2xx_hal_cs(&dev->hal, 1);
}

void at86rf2xx_force_trx_off(at86rf2xx_t *dev)
{
	at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_STATE, AT86RF2XX_TRX_STATE__FORCE_TRX_OFF);
	while (at86rf2xx_get_status(dev) != AT86RF2XX_STATE_TRX_OFF);
}

uint8_t at86rf2xx_get_status(at86rf2xx_t *dev)
{
	/* if sleeping immediately return state */
	if(dev->state == AT86RF2XX_STATE_SLEEP)
		return dev->state;

	return at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS) & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
}

void at86rf2xx_assert_awake(at86rf2xx_t *dev)
{
    if(at86rf2xx_get_status(dev) == AT86RF2XX_STATE_SLEEP)
    {
        /* wake up and wait for transition to TRX_OFF */
    	at86rf2xx_hal_sleep(&dev->hal, 0);
    	at86rf2xx_hal_delay(&dev->hal, AT86RF2XX_WAKEUP_DELAY);

        /* update state */
        dev->state = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS) & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    }
}

void at86rf2xx_hardware_reset(at86rf2xx_t *dev)
{
    /* wake up from sleep in case radio is sleeping */
	at86rf2xx_assert_awake(dev);

    /* trigger hardware reset */
    at86rf2xx_hal_reset(&dev->hal, 0);
    at86rf2xx_hal_delay(&dev->hal, AT86RF2XX_RESET_PULSE_WIDTH);
    at86rf2xx_hal_reset(&dev->hal, 1);
    at86rf2xx_hal_delay(&dev->hal, AT86RF2XX_RESET_PULSE_WIDTH);
}

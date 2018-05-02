#include <at86rf2xx-defaults.h>
#include <at86rf2xx-driver.h>
#include <at86rf2xx-hal.h>
#include <at86rf2xx-registers.h>

#include "contiki.h"

#include "net/netstack.h"
#include "sys/log.h"

#define LOG_LEVEL           LOG_LEVEL_INFO
#define LOG_MODULE          "at86rf2xx"


static int _at86rf2xx_init(void)
{
    LOG_INFO_("RF: Init\n");
    return at86rf2xx_init(&at86rf2xx) == 0 ? RADIO_TX_OK : RADIO_TX_ERR;
}

static int _at86rf2xx_prepare(const void *payload, unsigned short payload_len)
{
    LOG_INFO("RF: Prepare (%d)\n", payload_len);
    if(payload_len > AT86RF2XX_MAX_PKT_LENGTH)
    {
        LOG_ERR("RF: Invalid payload length!\n");
        return RADIO_TX_ERR;
    }
    at86rf2xx_tx_prepare(&at86rf2xx);
    at86rf2xx_tx_load(&at86rf2xx, payload, payload_len, 0);
    return RADIO_TX_OK;
}

static int _at86rf2xx_transmit(unsigned short transmit_len)
{
    LOG_INFO("RF: Transmit (%d)\n", transmit_len);
    
    if(transmit_len != at86rf2xx.frame_len)
    {
        LOG_ERR_("RF: TX length mismatch!\n");
        return RADIO_TX_ERR;
    }
    at86rf2xx_tx_exec(&at86rf2xx);
    return RADIO_TX_OK;
}

static int _at86rf2xx_send(const void *payload, unsigned short payload_len)
{
    int ret;

    LOG_INFO("RF: Send (%d)\n", payload_len);

    /* payload_len checked within prepare() */
    if((ret = _at86rf2xx_prepare(payload, payload_len)) == RADIO_TX_OK) 
        ret = _at86rf2xx_transmit(payload_len);

    return ret;
}

static int _at86rf2xx_read(void *buf, unsigned short buf_len)
{
    size_t pkt_len = at86rf2xx_rx_len(&at86rf2xx);
    printf("Frame length: %u bytes\r\n", pkt_len);

    /*  Print the frame, byte for byte  */
    printf("Frame dump (ASCII):\r\n");
    uint8_t data[pkt_len];
    at86rf2xx_rx_read(&at86rf2xx, data, pkt_len, 0);
    for (size_t d = 0; d < pkt_len; d++)
        printf("%c", (char)data[d]);
    printf("\r\n");
    return RADIO_TX_OK;
}

static int _at86rf2xx_channel_clear(void)
{
    return at86rf2xx_cca(&at86rf2xx); // FIXME
}

static int _at86rf2xx_receiving_packet(void)
{
    int ret = 0;
    uint8_t state = at86rf2xx_get_status(&at86rf2xx);
    if(state == AT86RF2XX_STATE_SLEEP)
        return ret;
    
    uint8_t irq_mask = at86rf2xx_reg_read(&at86rf2xx, AT86RF2XX_REG__IRQ_STATUS);

    /*  Incoming radio frame! */
    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START)
        ret = 1;
    else
        ret = 0;
    
    LOG_INFO("RF: Receiving (%d)\n", ret);
    return ret;
}

static int _at86rf2xx_pending_packet(void)
{
    int ret = (at86rf2xx.events != 0) ? 1 : 0;
    LOG_INFO("RF: Pending (%d)\n", ret);
    return ret;
}

static int _at86rf2xx_on(void)
{
    return 0;
}

static int _at86rf2xx_off(void)
{
    return 0;
}

static radio_result_t _at86rf2xx_get_value(radio_param_t param, radio_value_t *value)
{
    return 0;
}

static radio_result_t _at86rf2xx_set_value(radio_param_t param, radio_value_t value)
{
    return 0;
}

static radio_result_t _at86rf2xx_get_object(radio_param_t param, void *dest, size_t size)
{
    return 0;
}

static radio_result_t _at86rf2xx_set_object(radio_param_t param, const void *src, size_t size)
{
    return 0;
}
  
const struct radio_driver at86rf2xx_driver =
{
    _at86rf2xx_init,
    _at86rf2xx_prepare,
    _at86rf2xx_transmit,
    _at86rf2xx_send,
    _at86rf2xx_read,
    _at86rf2xx_channel_clear,
    _at86rf2xx_receiving_packet,
    _at86rf2xx_pending_packet,
    _at86rf2xx_on,
    _at86rf2xx_off,
    _at86rf2xx_get_value,
    _at86rf2xx_set_value,
    _at86rf2xx_get_object,
    _at86rf2xx_set_object
};



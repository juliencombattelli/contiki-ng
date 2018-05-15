#include "at86rf2xx.h"

#include "contiki.h"

#include "net/netstack.h"
#include "net/packetbuf.h"
#include "sys/log.h"

#define LOG_LEVEL           LOG_LEVEL_INFO
#define LOG_MODULE          "at86rf2xx"

at86rf2xx_t at86rf2xx;

static int _at86rf2xx_init(void)
{
    LOG_INFO_("RF: Init\n");
    int ret = at86rf2xx_init(&at86rf2xx) == 0 ? RADIO_TX_OK : RADIO_TX_ERR;
    process_start(&at86rf2xx_process, NULL);
    return ret;
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
        LOG_ERR("RF: TX length mismatch (prepsize=%d)!\n",at86rf2xx.frame_len);
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
    return pkt_len;
}

static int _at86rf2xx_channel_clear(void)
{
    LOG_INFO_("RF: channel clear\n");
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
    LOG_INFO_("RF: On\n");
    return 0;
}

static int _at86rf2xx_off(void)
{
    LOG_INFO_("RF: Off\n");
    return 0;
}

static radio_result_t _at86rf2xx_get_value(radio_param_t param, radio_value_t *value)
{
    LOG_INFO("RF: Getting value for param %s\n", param_to_str[param]);

    return RADIO_RESULT_NOT_SUPPORTED; // FIXME

    if(!value)
    {
        LOG_ERR_("RF: Invalid value!\n");
        return RADIO_RESULT_INVALID_VALUE;
    }

    switch(param)
    {
    case RADIO_PARAM_POWER_MODE:
        if(at86rf2xx_get_status(&at86rf2xx) != AT86RF2XX_STATE_SLEEP)
            *value = (radio_value_t)RADIO_POWER_MODE_ON;
        else
            *value = (radio_value_t)RADIO_POWER_MODE_OFF;
        return RADIO_RESULT_OK;

    case RADIO_PARAM_CHANNEL:
        *value = (radio_value_t)at86rf2xx_get_chan(&at86rf2xx);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_PAN_ID:
        *value = (radio_value_t)at86rf2xx_get_pan(&at86rf2xx);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_16BIT_ADDR:
        *value = (radio_value_t)at86rf2xx_get_addr_short(&at86rf2xx);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_RX_MODE:
        *value = 0;
        int autoack = at86rf2xx.options & AT86RF2XX_OPT_AUTOACK ? 1 : 0;
        int addrfilter = at86rf2xx.options & AT86RF2XX_OPT_PROMISCUOUS ? 0 : 1;
        *value = (radio_value_t)((autoack << 1) | (addrfilter << 0));
        return RADIO_RESULT_OK;

    case RADIO_PARAM_TX_MODE:
        return RADIO_RESULT_NOT_SUPPORTED; // FIXME

    case RADIO_PARAM_TXPOWER:
        *value = (radio_value_t)at86rf2xx_get_txpower(&at86rf2xx);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_CCA_THRESHOLD:
    case RADIO_PARAM_RSSI:
    case RADIO_PARAM_64BIT_ADDR:
        return RADIO_RESULT_NOT_SUPPORTED;

    case RADIO_CONST_CHANNEL_MIN:
        *value = (radio_value_t)AT86RF2XX_MIN_CHANNEL;
        return RADIO_RESULT_OK;

    case RADIO_CONST_CHANNEL_MAX:
        *value = (radio_value_t)AT86RF2XX_MAX_CHANNEL;
        return RADIO_RESULT_OK;
   
    case RADIO_CONST_TXPOWER_MIN:
#ifdef MODULE_AT86RF212B
        *value = (radio_value_t)-25;
#else
        *value = (radio_value_t)-17;
#endif
        return RADIO_RESULT_OK;

    case RADIO_CONST_TXPOWER_MAX:
#ifdef MODULE_AT86RF212B
        *value = (radio_value_t)11;
#elif  MODULE_AT86RF233
        *value = (radio_value_t)4;
#else
        *value = (radio_value_t)3;
#endif
        return RADIO_RESULT_OK;

    default:
        return RADIO_RESULT_NOT_SUPPORTED;
    }
}

static radio_result_t _at86rf2xx_set_value(radio_param_t param, radio_value_t value)
{
    LOG_INFO("RF: Setting value %d for param %s\n", value, param_to_str[param]);

    return RADIO_RESULT_NOT_SUPPORTED; // FIXME

    switch(param) 
    {
    case RADIO_PARAM_POWER_MODE:
        if(value == RADIO_POWER_MODE_ON) {
            at86rf2xx_assert_awake(&at86rf2xx);  
            return RADIO_RESULT_OK;
        }
        if(value == RADIO_POWER_MODE_OFF) { 
            at86rf2xx_set_state(&at86rf2xx, AT86RF2XX_STATE_SLEEP);
            return RADIO_RESULT_OK;
        }
        return RADIO_RESULT_INVALID_VALUE;

    case RADIO_PARAM_CHANNEL:
        if(value < AT86RF2XX_MIN_CHANNEL || value > AT86RF2XX_MAX_CHANNEL)
            return RADIO_RESULT_INVALID_VALUE;
        at86rf2xx_set_chan(&at86rf2xx, value);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_PAN_ID:
        at86rf2xx_set_pan(&at86rf2xx, value);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_16BIT_ADDR:
        at86rf2xx_set_addr_short(&at86rf2xx, value);
        return RADIO_RESULT_OK;

    case RADIO_PARAM_RX_MODE:
    case RADIO_PARAM_TX_MODE:
    case RADIO_PARAM_TXPOWER:
    case RADIO_PARAM_CCA_THRESHOLD: 
    case RADIO_PARAM_RSSI:  
    case RADIO_PARAM_64BIT_ADDR:
    default:
        return RADIO_RESULT_NOT_SUPPORTED;
    }
}

static radio_result_t _at86rf2xx_get_object(radio_param_t param, void *dest, size_t size)
{
    LOG_INFO_("RF: Get object\n");
    return RADIO_RESULT_NOT_SUPPORTED;
}

static radio_result_t _at86rf2xx_set_object(radio_param_t param, const void *src, size_t size)
{
    LOG_INFO_("RF: Set object\n");
    return RADIO_RESULT_NOT_SUPPORTED;
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

void at86rf2xx_irq_handler(void)
{
    printf("\n\nat86rf2xx_irq_handler !!!!!!!\n\n");
    while(1);
    at86rf2xx.events++;
    process_poll(&at86rf2xx_process);
}

static void at86rf2xx_eventHandler()
{
    /* One less event to handle! */
    at86rf2xx.events--;

    /* If transceiver is sleeping register access is impossible and frames are
    * lost anyway, so return immediately.
    */
    uint8_t state = at86rf2xx_get_status(&at86rf2xx);
    if(state == AT86RF2XX_STATE_SLEEP)
    {   
        printf("AT86RF2XX is sleeping !!!\n"); 
        return;
    }

    /* read (consume) device status */
    uint8_t irq_mask = at86rf2xx_reg_read(&at86rf2xx, AT86RF2XX_REG__IRQ_STATUS);
    printf("irq_mask=%x\n", (int)irq_mask);

    /*  Incoming radio frame! */
    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START)
        printf("[at86rf2xx] EVT - RX_START\r\n");
    //else
    //    printf("[at86rf2xx] no EVT - RX_START\r\n");

    /*  Done receiving radio frame; call our receive_data function.
    */
    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__TRX_END)
    {
        if(state == AT86RF2XX_STATE_RX_AACK_ON || state == AT86RF2XX_STATE_BUSY_RX_AACK)
        {
            printf("[at86rf2xx] EVT - RX_END\r\n");
            packetbuf_clear();
            size_t pkt_len = at86rf2xx_rx_len(&at86rf2xx);
            at86rf2xx_rx_read(&at86rf2xx, packetbuf_dataptr(), PACKETBUF_SIZE, 0);
            packetbuf_set_datalen(pkt_len);
            printf("frame : ");
            for(int i = 0; i < pkt_len; i++) printf("%x ", ((char*)packetbuf_dataptr())[i]);
            printf("\n");
        }
        //else
        //    printf("[at86rf2xx] no EVT - RX_END");
    }
    //else
    //    printf("[at86rf2xx] no EVT -TRX_END\r\n");
}

PROCESS_THREAD(at86rf2xx_process, ev, data)
{ 
    PROCESS_BEGIN();

    printf("at86rf2xx_process: started\n");

    while(1) 
    {
        PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

        printf("at86rf2xx_process\n");

        if(at86rf2xx.events)
        {
            printf("number of events: %d\n)", at86rf2xx.events);
            at86rf2xx_eventHandler();
            NETSTACK_MAC.input();    
        }
    }

    PROCESS_END();
}


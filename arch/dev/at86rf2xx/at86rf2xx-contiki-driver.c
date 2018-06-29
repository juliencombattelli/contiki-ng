#include "at86rf2xx.h"

#include "contiki.h"

#include "net/netstack.h"
#include "net/packetbuf.h"
#include "sys/log.h"

#define LOG_LEVEL           LOG_LEVEL_INFO
#define LOG_MODULE          "at86rf2xx"

/**
 * @brief   String version of all parameters used.
 *          Used for logging.
 */
char* param_to_str[] = {
    "RADIO_PARAM_POWER_MODE",
    "RADIO_PARAM_CHANNEL",
    "RADIO_PARAM_PAN_ID",
    "RADIO_PARAM_16BIT_ADDR",
    "RADIO_PARAM_RX_MODE",
    "RADIO_PARAM_TX_MODE",
    "RADIO_PARAM_TXPOWER",
    "RADIO_PARAM_CCA_THRESHOLD",
    "RADIO_PARAM_RSSI",
    "RADIO_PARAM_LAST_RSSI",
    "RADIO_PARAM_LAST_LINK_QUALITY",
    "RADIO_PARAM_64BIT_ADDR",
    "RADIO_PARAM_LAST_PACKET_TIMESTAMP",
    "RADIO_CONST_CHANNEL_MIN",
    "RADIO_CONST_CHANNEL_MAX",
    "RADIO_CONST_TXPOWER_MIN",
    "RADIO_CONST_TXPOWER_MAX"
};

/**
 * @brief   Contiki-NG process declaration to handle packet reception
 */
PROCESS(at86rf2xx_process, "AT86RF2XX driver");

/**
 * @brief   AT86RF2XX driver definition
 */
at86rf2xx_t at86rf2xx;

/**
 * @brief   Initialize the driver and start the reception process
 */
static int _at86rf2xx_init(void)
{
    LOG_INFO_("RF: Init\n");
    int ret = at86rf2xx_init(&at86rf2xx) == 0 ? RADIO_TX_OK : RADIO_TX_ERR;
    process_start(&at86rf2xx_process, NULL);
    return ret;
}

/**
 * @brief   Send a packet to the radio to be emit later
 */
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

/**
 * @brief   Tell the radio to transmit a packet previously prepared
 */
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

/**
 * @brief   Perform a packet preparation and transmition
 * @result  Return 
 */
static int _at86rf2xx_send(const void *payload, unsigned short payload_len)
{
    int ret;

    LOG_INFO("RF: Send (%d)\n", payload_len);

    /* payload_len checked within prepare() */
    if((ret = _at86rf2xx_prepare(payload, payload_len)) == RADIO_TX_OK) 
        ret = _at86rf2xx_transmit(payload_len);

    return ret;
}

/**
 * @brief   Read a packet
 * @return  Return the number of byte read
 */
static int _at86rf2xx_read(void *buf, unsigned short buf_len)
{
    size_t pkt_len = at86rf2xx_rx_len(&at86rf2xx);
    if (pkt_len != 0) // There is something to read
    {
        uint8_t data[pkt_len];
        at86rf2xx_rx_read(&at86rf2xx, data, pkt_len, 0);
    }
    return pkt_len;
}

/**
 * @brief   Perform a clear channel assessment
 * @return  Return 1 if channel is clear, 0 otherwise
 */
static int _at86rf2xx_channel_clear(void)
{
    LOG_INFO_("RF: channel clear\n");
    return at86rf2xx_cca(&at86rf2xx) == true ? 1 : 0; 
}

/**
 * @brief   Check if the radio is receiving data
 * @return  Return 1 if there is an incoming frame, 0 otherwise
 */
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

/**
 * @brief   Check if the radio has some packet pending
 * @return  Return 1 if there is a packet to process, 0 otherwise
 */
static int _at86rf2xx_pending_packet(void)
{
    int ret = (at86rf2xx.events != 0) ? 1 : 0;
    LOG_INFO("RF: Pending (%d)\n", ret);
    return ret;
}

/**
 * @brief   Turn on the radio
 */
static int _at86rf2xx_on(void)
{
    LOG_INFO_("RF: On\n");
    // FIXME
    return 0;
}

/**
 * @brief   Turn off the radio
 */
static int _at86rf2xx_off(void)
{
    LOG_INFO_("RF: Off\n");
    // FIXME
    return 0;
}


/**
 * @brief   Retrieve the value of a parameter
 * @result  RADIO_RESULT_OK if no error. See radio_result_t enum in os/dev/radio.h
 */
static radio_result_t _at86rf2xx_get_value(radio_param_t param, radio_value_t *value)
{
    LOG_INFO("RF: Getting value for param %s\n", param_to_str[param]);
    
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

/**
 * @brief   Set the value of a parameter
 * @result  RADIO_RESULT_OK if no error. See radio_result_t enum in os/dev/radio.h
 */
static radio_result_t _at86rf2xx_set_value(radio_param_t param, radio_value_t value)
{
    LOG_INFO("RF: Setting value %d for param %s\n", value, param_to_str[param]);

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


/**
 * @brief   Retrieve the value of a parameter as a void*
 * @result  RADIO_RESULT_OK if no error. See radio_result_t enum in os/dev/radio.h
 */
static radio_result_t _at86rf2xx_get_object(radio_param_t param, void *dest, size_t size)
{
    LOG_INFO_("RF: Get object\n");
    return RADIO_RESULT_NOT_SUPPORTED;
}

/**
 * @brief   Set the value of a parameter as a void*
 * @result  RADIO_RESULT_OK if no error. See radio_result_t enum in os/dev/radio.h
 */
static radio_result_t _at86rf2xx_set_object(radio_param_t param, const void *src, size_t size)
{
    LOG_INFO_("RF: Set object\n");
    return RADIO_RESULT_NOT_SUPPORTED;
}


/**
 * @brief   Define a Contiki-NG-compatible radio driver
 */
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

/**
 * @brief  IRQ handler called when a data is received
 */
void at86rf2xx_irq_handler(void)
{
    /* Increment the pending event count */
    at86rf2xx.events++;
    /* Signal Contiki to schedule the reception process to handle the event */
    process_poll(&at86rf2xx_process);
}

/**
 * @brief  Consume the last event that has occuried
 */
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
    
    /*  Done receiving radio frame; call our receive_data function */
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
    }
}

/**
 * @brief  Define a Contiki-NG process to handle incoming frame
 */
PROCESS_THREAD(at86rf2xx_process, ev, data)
{ 
    PROCESS_BEGIN();

    printf("at86rf2xx_process: started\n");

    while(1) 
    {
        /* Wait until there is a polling event to handle */
        PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

        /* If there was an activity on the radio */
        if(at86rf2xx.events)
        {
            /* Handle last event */
            at86rf2xx_eventHandler();
            /* Tell the Contiki-NG MAC layer there is something to process */
            NETSTACK_MAC.input();    
        }
    }

    PROCESS_END();
}


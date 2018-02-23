/*---------------------------------------------------------------------------*/
#ifndef CONTIKI_ARCH_CPU_STM32L1XX_STM32L1XX_CONF_H_
#define CONTIKI_ARCH_CPU_STM32L1XX_STM32L1XX_CONF_H_
/*---------------------------------------------------------------------------*/
/**
 * \name CFS configuration
 *
 * @{
 */
#ifndef COFFEE_CONF_SIZE
#define COFFEE_CONF_SIZE            (0 * COFFEE_SECTOR_SIZE)
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Watchdog Timer configuration
 *
 * @{
 */
#ifndef WATCHDOG_CONF_ENABLE
#define WATCHDOG_CONF_ENABLE        0 /**< Enable the watchdog timer */
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Character I/O Configuration
 *
 * @{
 */
#ifndef UART_CONF_ENABLE
#define UART_CONF_ENABLE            0 /**< Enable/Disable UART I/O */
#endif

#ifndef UART0_CONF_BAUD_RATE
#define UART0_CONF_BAUD_RATE   115200 /**< Default UART0 baud rate */
#endif

#ifndef UART1_CONF_BAUD_RATE
#define UART1_CONF_BAUD_RATE   115200 /**< Default UART1 baud rate */
#endif

#ifndef SLIP_ARCH_CONF_USB
#define SLIP_ARCH_CONF_USB          0 /**< SLIP over UART by default */
#endif

#ifndef DBG_CONF_USB
#define DBG_CONF_USB                0 /**< All debugging over UART by default */
#endif

#ifndef SERIAL_LINE_CONF_UART
#define SERIAL_LINE_CONF_UART       0 /**< UART to use with serial line */
#endif

#if !SLIP_ARCH_CONF_USB
#ifndef SLIP_ARCH_CONF_UART
#define SLIP_ARCH_CONF_UART         0 /**< UART to use with SLIP */
#endif
#endif

#if !DBG_CONF_USB
#ifndef DBG_CONF_UART
#define DBG_CONF_UART               0 /**< UART to use for debugging */
#endif
#endif

#ifndef UART1_CONF_UART
#define UART1_CONF_UART             0 /**< UART to use for examples relying on
                                           the uart1_* API */
#endif

#ifndef SLIP_ARCH_CONF_ENABLED
/*
 * Determine whether we need SLIP
 * This will keep working while UIP_FALLBACK_INTERFACE and CMD_CONF_OUTPUT
 * keep using SLIP
 */
#if defined(UIP_FALLBACK_INTERFACE) || defined(CMD_CONF_OUTPUT)
#define SLIP_ARCH_CONF_ENABLED      1
#endif
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name RF configuration
 *
 * @{
 */
/* RF Config */

/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name LPM configuration
 * @{
 */
#ifndef LPM_CONF_ENABLE
#define LPM_CONF_ENABLE       0 /**< Set to 0 to disable LPM entirely */
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Radio Configuration
 *
 * @{
 */
#ifndef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO         nullradio_driver
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name IEEE address configuration
 *
 * Used to generate our link-layer & IPv6 address
 * @{
 */
/**
 * \brief Location of the IEEE address
 * 0 => Read from InfoPage,
 * 1 => Use a hardcoded address, configured by IEEE_ADDR_CONF_ADDRESS
 */
#ifndef IEEE_ADDR_CONF_HARDCODED
#define IEEE_ADDR_CONF_HARDCODED             1
#endif

/**
 * \brief The hardcoded IEEE address to be used when IEEE_ADDR_CONF_HARDCODED
 * is defined as 1
 */
#ifndef IEEE_ADDR_CONF_ADDRESS
#define IEEE_ADDR_CONF_ADDRESS { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }
#endif

/**
 * \brief Location of the IEEE address in the InfoPage when
 * IEEE_ADDR_CONF_HARDCODED is defined as 0
 * 0 => Use the primary address location
 * 1 => Use the secondary address location
 */
#ifndef IEEE_ADDR_CONF_USE_SECONDARY_LOCATION
#define IEEE_ADDR_CONF_USE_SECONDARY_LOCATION 0
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Security
 *
 * @{
 */
#ifndef CRYPTO_CONF_INIT
#define CRYPTO_CONF_INIT        0 /**< Whether to init cryptoprocessor */
#endif
/** @} */
/*---------------------------------------------------------------------------*/
#endif /* CONTIKI_ARCH_CPU_STM32L1XX_STM32L1XX_CONF_H_ */
/*---------------------------------------------------------------------------*/

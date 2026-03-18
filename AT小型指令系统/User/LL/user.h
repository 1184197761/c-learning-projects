/**
 ******************************************************************************
 * @file    User/LL/user.h
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    29-01-2019
 * @brief   Main program body header file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 HUGE-IC</center></h2>
 *
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_H__
#define __USER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "include.h"

/** @addtogroup Template_Project
 * @{
 */
/* UART baud rate */
#define UART_BAUDRATE 115200
/** @addtogroup USER
 * @{
 */

/** @defgroup USER_Exported_Constants
 * @{
 */

/*! PHY address
 */
#define PHY_ADDR 0x01
/*! Define the functions supported by the GMAC module(10M/100M full duples
 *  and half duplex).
 */
#define GMAC_SUPPORTED (ADVERTISED_MII | ADVERTISED_Autoneg |               \
                        ADVERTISED_10baseT_Full | ADVERTISED_10baseT_Half | \
                        ADVERTISED_100baseT_Full | ADVERTISED_100baseT_Half) /*ADVERTISED_100baseT_Full | ADVERTISED_100baseT_Half |\*/

     /**
      * @}
      */

     /** @defgroup USER_Exported_Functions
      * @{
      */

     /**
      * @brief  Usart initialization function
      * @param  None
      * @retval None
      */
     void usart_init(void);

     /**
      * @brief  Usart Configuration function
      * @param  None
      * @retval None
      */
     void usart_config(void);

     /**
      * @brief  USART detele init function
      * @param  None
      * @retval None
      */
     void usart_deinit(void);

     /**
      * @brief  VERIFY initialization function
      * @param  None
      * @retval None
      */
     void verify_init(void);

     /**
      * @brief  VERIFY Configuration function
      * @param  None
      * @retval None
      */
     void verify_config(void);

     /**
      * @brief  VERIFY data handle function
      * @param  None
      * @retval None
      */
     void verify_handle(void);

     //    void lab1_uart();

     /**
      * @}
      */

#ifdef __cplusplus
}
#endif

#endif //__USER_H__

/**
 * @}
 */

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

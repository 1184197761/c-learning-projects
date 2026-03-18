/**
  ******************************************************************************
  * @file    User/LL/config.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    29-01-2019
  * @brief   project macro configuration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2019 HUGE-IC</center></h2>
  *
  *
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************
 *                              Chip Configuration                             *
 *******************************************************************************/
/*! define sdk target chip.
 */
#define TX4102

#define TXW801_100_QFN24    1
//#define TXW802_350_QFN32    1
//#define TXW806_350_QFN32    1
//#define TXW806_650_QFN40    1
//#define TXW806_850_QFN48    1

//#if TXW801_100_QFN24
//#define CHIP_TYPE_ID 0x01
//#endif
//
//#if TXW802_350_QFN32
//#define CHIP_TYPE_ID 0x02
//#endif
//
//#if TXW806_350_QFN32
//#define CHIP_TYPE_ID 0x03
//#endif
//
//#if TXW806_650_QFN40
//#define CHIP_TYPE_ID 0x04
//#endif
//
//#if TXW806_850_QFN48
//#define CHIP_TYPE_ID 0x05
//#endif


/*******************************************************************************
 *                             System Configuration                            *
 *******************************************************************************/


/*******************************************************************************
 *                            Function Configuration                           *
 *******************************************************************************/
#define USB_FUNC_SEL_USB20          1 /* 0:USB11, 1:USB20 */

#if USB_FUNC_SEL_USB20
#define USB_HS_EN                   1
#else
#define USB_HS_EN                   0       /* force 0 */
#endif

#ifdef __cplusplus
}
#endif

#endif //__CONFIG_H

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

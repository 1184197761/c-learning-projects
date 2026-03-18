/**
  ******************************************************************************
  * @file    User/LL/tx_peg_irq.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    29-01-2019
  * @brief   This file contains all the IRQ firmware functions.
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
#ifndef __TX_PEG_LL_IRQ_H
#define __TX_PEG_LL_IRQ_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_peg.h"

/** @addtogroup Template_Project
  * @{
  */

/** @addtogroup TX_PEG_IRQ
  * @{
  */ 

     
/** @defgroup Exported_Constants
  * @{
  */


  
/**
  * @}
  */


/** @defgroup Exported_Functions
  * @{
  */
extern void SysTick_Handler(void);

/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif //__TX_PEG_LL_IRQ_H

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2019 HUGE-IC *****END OF FILE****/

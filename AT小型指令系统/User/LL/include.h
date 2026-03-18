/**
  ******************************************************************************
  * @file    User/LL/include.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    29-01-2019
  * @brief   A generic header file containing various required header files
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
#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#ifdef __cplusplus
 extern "C" {
#endif
#include "typesdef.h"
#include "errno.h"
#include "osal_list.h"
#include "dev.h"

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "tx_peg.h"

#include "tx_peg_system.h"
#include "tx_peg_assert.h"
#include "tx_peg_ll_module.h"
#include "k_ffs.h"

//#include "core_cmFunc.h"
//#include "core_cmSimd.h"
// math
#include <math.h>

// lib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "version.h"

// test
#include "test.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif //__INCLUDE_H__

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

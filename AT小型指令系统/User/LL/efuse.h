/**
  ******************************************************************************
  * @file    Libraries/Driver/include/LL/tx_peg_ll_crc.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    01-07-2019
  * @brief   This file contains all the CRC LL firmware functions.
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
#ifndef __TX_PEG_LL_EFUSE_TEST_H
#define __TX_PEG_LL_EFUSE_TEST_H

#ifdef __cplusplus
 extern "C" {
#endif

uint8_t crc8(uint8_t *data, uint8_t length);
void efuse_init(void);
int32 rf_para_efuse_write(void);
int32 rf_para_efuse_read(int32 check_flag);
uint8 module_area_crc_cal(uint8 *module_area_data);
int32 module_area_crc_write(void);
int32 module_area_crc_check(void);
int32 module_area_crc_empty(void);
int32 read_efuse_data(void);
int32 xtal_bios_id_tempr_efuse_write(int8 XTAL_ID,int8 MODULE_ID,int8 BIOS_ID);
int32 xtal_cap_efuse_write(void);
int32 xtal_cap_efuse_read(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif //__TX_PEG_LL_CRC_TEST_H

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

/**
  ******************************************************************************
  * @file    User/LL/tx_peg_irq.c
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    29-01-2019
  * @brief   This file contains all the Interrupt IRQ firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2019 HUGE-IC</center></h2>
  *
  *
  *
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "test.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if defined (CPU_TYPE_ARM)

#elif defined (CPU_TYPE_CSKY)

#endif

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
    dump_fault_state();
    while(1);
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    __breakpoint(0x00);
    
//    dump_fault_state();
    /* Go to infinite loop when Hard Fault exception occurs */
    while(1) {
        
    }
}
/** @defgroup USART_Interrupt USART Interrupt Handle function
  * @brief    Interrupt Handle function
  * @{
  */

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    SysTickCnt++; 
}

/******************************************************************************/
/*                 TX_PEG Peripherals Interrupt Handlers                      */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (tx_peg_startup.c).                                                  */
/******************************************************************************/

ATTRIBUTE_ISR void CORET_IRQHandler(void)
{
    readl(0xE000E010);
    
    SysTick_Handler();
}
/**
  * @brief  LVD_IRQHandler
  * @param  none
  * @retval None
  */
ATTRIBUTE_ISR void LVD_IRQHandler(void)
{


}

/**
  * @brief  WDT_IRQHandler
  * @param  none
  * @retval None
  */
ATTRIBUTE_ISR void WDT_IRQHandler(void)
{
#if TEST_WDT_EN
    extern void inc_wdt_int_cnt(void);

    if (LL_WDT_CHECK_INTERRUPT_ENABLE(WDT) && LL_WDT_GET_PENDING(WDT)) {

        LL_WDT_CLR_PENDING(WDT);
        inc_wdt_int_cnt();
    }
#endif

}

/**
  * @brief  AES_IRQHandler
  * @param  none
  * @retval None
  */
ATTRIBUTE_ISR void SYSAES_IRQHandler(void)
{
    /* aes */
    if (sys_aes_get_pending(SYS_AES)) {
        sys_aes_clear_pending(SYS_AES);
        sys_aes_done_flag_handle();
    }
}

/**
  * @brief  CRC_IRQHandler
  * @param  none
  * @retval None
  */
ATTRIBUTE_ISR void CRC_IRQHandler(void)
{
    /* crc */
    if (LL_CRC_GET_DONE_PENDING(CRC)) {
        ll_crc_clear_done_pending(CRC);
        add_crc_intr_cnts();
    }

}

/**
  * @brief  TK_IRQHandler
  * @param  none
  * @retval None
  */
ATTRIBUTE_ISR void TK_IRQHandler(void)
{
#if (TEST_TK_EN)
    extern u8 irq_cnt_tk[5];
    if (LL_TK_CHECK_SG_CONV_DONE_INT_ENABLE(TK) && LL_TK_GET_SG_CONV_PND(TK)) {
		ll_tk_clear_sg_conv_pending(TK);
        ll_gpio_bit_toggle(GPIOC,BIT(5));
        irq_cnt_tk[0]++;
    }

    if (LL_TK_CHECK_CH_DONE_INT_ENABLE(TK) &&  LL_TK_GET_CH_DONE_PND(TK)) {
		ll_tk_clear_ch_done_pending(TK);
        irq_cnt_tk[1]++;
    } 

    if (LL_TK_CHECK_SCAN_DONE_INT_ENABLE(TK) && LL_TK_GET_SCAN_DONE_PND(TK)) {
		ll_tk_clear_scan_done_pending(TK);
        irq_cnt_tk[2]++;
    } 
    
    if (LL_TK_CHECK_DMA_INT_ENABLE(TK) && LL_TK_GET_DMA_PND(TK)) {
		ll_tk_clear_dma_pending(TK);
        irq_cnt_tk[3]++;
    } 

    if (LL_TK_CHECK_DMA_INT_ENABLE(TK) && LL_TK_GET_DMA_ERR_PND(TK)) {
		ll_tk_clear_dma_err_pending(TK);
        irq_cnt_tk[4]++;
    } 
#endif

}

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

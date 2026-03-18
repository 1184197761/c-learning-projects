/**
 ******************************************************************************
 * @file    User/LL/main.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    29-01-2019
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 HUGE-IC</center></h2>
 *
 * 版权说明后续补上
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "main.h"
#include "user.h"

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
    u32 cur_tick;

    extern u32 g_intstackbase;
    *((int *)((u32)&g_intstackbase)) = 0xdeadbeef;

    /* 清除SRAM_BOOT标志，强制下一次重置重启 */
    CORE_SEC_OPT_B(PMU->PMUCON7, PMU->PMUCON7 & ~(BIT(0)));

    csi_vic_clear_all_pending_irq();
    csi_vic_clear_all_active();

#if defined(CPU_TYPE_ARM)
    __enable_irq();
#elif defined(CPU_TYPE_CSKY)
    __enable_excp_irq();
#endif

    system_tick_init();
    NVIC_EnableIRQ(CORET_IRQn);

    // /* VERIFY initialization function */
    // verify_init();

    /* VERIFY Configuration function */
    verify_config();

    /* VERIFY data handle function */
    verify_handle();

    // cur_tick = get_system_tick();

    /* sign app code run successed , if not sign & watchdog reset, bootloader will skip this code */
    // while (1) {
    //     if (is_systick_expired(cur_tick, 5000)) {
    //         cur_tick = get_system_tick();
    //         printf("alive\r\n");
    //     }
    // }
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

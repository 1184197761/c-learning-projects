/**
 ******************************************************************************
 * @file    User/LL/user.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    29-01-2019
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 HUGE-IC</center></h2>
 *
 *
 ******************************************************************************
 */
#include "osal_list.h"
/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "jpg.h"
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
 * @brief  Usart initialization function
 * @param  None
 * @retval None
 */
void uart_init(void)
{
    //    /* USART initialization structure variable */
    //    TYPE_LL_UART_INIT init;
    //    usart_config();
    //    memset(&init, 0x0, sizeof(TYPE_LL_UART_INIT));
    //
    //    /* Reset the USART module to enable the clock. */
    //    ll_uart_init(UART0, &init);
}

/**
 * @brief  Usart Configuration function
 * @param  None
 * @retval None
 */
void uart_config(void)
{
    TYPE_LL_UART_CFG uart_cfg;                // USART配置结构变量
    memset(&uart_cfg, 0x0, sizeof(uart_cfg)); // 初始化USART的相关变量
    /* 配置USART相关信息 */
    uart_cfg.baudrate = SYS_CLK / UART_BAUDRATE - 1;
    uart_cfg.bit_width_sel = LL_UART_WORD_LENGTH_8B;
    uart_cfg.stop_bit_sel = LL_UART_STOP_1B;
    uart_cfg.parity = LL_UART_PARITY_NO;
    uart_cfg.mode = LL_UART_FULL_DUPLEX;
    uart_cfg.timeout_en = FALSE;
    uart_cfg.carrier_output_en = FALSE;
    uart_cfg.tx_signal_invert_flag = FALSE;
    uart_cfg.rx_signal_invert_flag = FALSE;
    /* 调用USART驱动配置函数 */
    ll_uart232_config(UART1, &uart_cfg);
    debug_select_interface(DEBUG_UART1);
    ll_gpio_iomap_output_config(GPIOA, LL_GPIO_PIN_11, LL_GPIO_IOMAP_OUT_UART1_OUT);
}

/**
 * @brief  USART detele init function
 * @param  None
 * @retval None
 */
void uart_deinit(void)
{
    //    ll_uart_deinit(UART0);
}

/**
 * @brief  VERIFY initialization function
 * @param  None
 * @retval None
 */
void verify_init(void)
{
    /* close watchdog */
    // ll_wdt_stop(WDT);

    uart_init();
}

/**
 * @brief  VERIFY Configuration function
 * @param  None
 * @retval None
 */
void verify_config(void)
{
#if !TEST_USART_EN & !LAB_AT_EN
    uart_config();

    // void sys_reset_show(void);
    // sys_reset_show();

    // VERSION_SHOW();
#if 0
    //printf("wdt : %08x\r\n", WDT->WDTCON);
    extern const u16            __jpeg_htable[];
    printf("__jpeg_htable = { ");
    for (int i=0; i<512/2; i++) {
        if (0 == (i & 0xF)) {
            printf("\r\n");
        }
        printf("%04x ", __jpeg_htable[i]);
    }
    printf("\r\n}\r\n");
#endif
#endif
}

/**
 * @brief  VERIFY data handle function
 * @param  None
 * @retval None
 */
bool fatfs_init();
bool fatfs_test();
void verify_handle(void)
{
    // CORE_SEC_OPT_B(PMU->PMUCON11, PMU->PMUCON11 | (BIT(9) | BIT(10)));
    // CORE_SEC_OPT_B(PMU->PMUCON11, PMU->PMUCON11 | (BIT(14)));
#if TEST_SYSMONITOR_EN
    sysmonitor_test();
#endif
#if LAB_AT_EN
    lab_at();
#endif
#if TEST_MEM_EN
    mem_test();
#endif
#if TEST_GMAC_EN
    gmac_test();
#endif
#if TEST_TIMER_EN
    timer_test();
#endif
#if TEST_LED_TIMER_EN
    led_timer_test();
#endif
#if TEST_SUPER_TIMER_EN
    super_timer_test();
#endif
#if TEST_LED_EN
    led_test();
#endif
#if TEST_GPIO_EN
    gpio_test();
#endif
#if TEST_WDT_EN
    wdt_test();
#endif
#if TEST_CRC_EN
    crc_test();
#endif
#if TEST_SYS_EN
    sysctrl_reg_test();
#endif
#if TEST_RF_EN
    rfctrl_test();
#endif
#if TEST_SPI_EN
    spi_test();
#endif
#if TEST_SIMPLE_SPI_EN
    simple_spi_test();
#endif
#if TEST_USART_EN
    usart_test();
#endif
#if TEST_SIMPLE_USART_EN
    simple_usart_test();
#endif
#if TEST_DMAC_EN
    dmac_test();
#endif
#if TEST_QSPI_EN
    cqspi_test();
#endif
#if TEST_IIC_EN
    iic_test();
#endif
#if TEST_SYS_AES_TEST_EN
    sys_aes_test();
#endif
#if TEST_RFSPI_EN
    rfspi_test();
#endif
#if TEST_LMAC_AES_EN
    lmac_aes_test();
#endif
#if TEST_SDIO_EN
    my_sdio_test();
    // sdio_s_test();
#endif
#if TEST_MEM_COMPETITION_EN
    mem_competition_test();
#endif
#if TEST_AH_LMAC_EN
    ah_lmac_test();
#endif
#if TEST_MIPI_EN
    mipi_test();
#endif
#if TEST_INTERRUPT_EN
    interrupt_test();
#endif
#if TEST_JTAG_MAP_EN
    mars_jtag_test();
#endif
#if TEST_MARS_PD_EN
    mars_pd_test();
#endif
#if TEST_MEM_WRITE_PROTECT_EN
    mem_write_protect_test();
#endif
#if TEST_RF_DIGITAL_REG
    rf_reg_test();
#endif
#if TEST_MBIST_EN
    mbist_test();
#endif
#if TEST_EFUSE_EN
    efuse_test();
#endif
#if TEST_IIS_EN
    iis_test();
#endif

#if TEST_M2M_DMA_EN
    m2m_dma_test();
#endif
#if TEST_BGN_LMAC_EN
    lmac_test_top();
#endif
#if TEST_LMAC_AUTO_EN
    lmac_auto_test();
#endif
#if TEST_SDHC_EN
    sdhc_test();
#endif

#if TEST_FS_EN
    fatfs_init();
#if (TEST_JPG_EN == 0)
    fatfs_test();
#endif
#endif
#if TEST_JPG_EN
    jpg_test();
#endif

#if TEST_CSI_EN
    csi_test();
#endif

#if TEST_USB_EN
    usb_test();
#endif
#if TEST_JPG_EN
#if TEST_FS_EN
    jpeg_write_photo();
#else
    jpeg_user();
#endif
#endif
#if TEST_TCP_IP_CHECKSUM_EN
    tcp_ip_checksum_test();
#endif

#if TEST_RF_TRX_EN
    rf_trx_test();
#endif
#if TEST_PDM_EN
    pdm_test();
#endif
#if TEST_TK_EN
    tk_test();
#endif
#if TEST_ADKEY_EN
    adkey_test();
#endif

#if TEST_XZ_EN
    // char updat[1024];
    char *updat;
    updat = 0x18000000;
    printf("xzdec start:%d\r\n", get_system_tick());
    xzdec(updat, 433736);
    printf("xzdec end:%d\r\n", get_system_tick());
#endif
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2019 HUGE-IC ***** END OF FILE *****/

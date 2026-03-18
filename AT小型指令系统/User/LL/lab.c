#include "include.h"
#include "user.h"

#define CFG_OFFSET(enum) ((enum & 0x3) << 3)   // 函数配置位偏移
#define TX_PIN LL_GPIO_PIN_11				   // 发送引脚
#define RX_PIN LL_GPIO_PIN_0				   // 接收引脚
#define RX_DMA_BUF_LEN 16					   // 接收DMA长度
#define UART_OUT LL_GPIO_IOMAP_OUT_UART0_OUT   // 串口输出函数
#define UART_IN LL_GPIO_IOMAP_IN_UART0_IN	   // 串口输入函数
#define UART_DMA_EN 1						   // 串口DMA使能
#define PWM_PIN LL_GPIO_PIN_1				   // PWM引脚
#define IR_PIN LL_GPIO_PIN_2				   // 红外引脚
#define PWM_TIMER TIMER0					   // PWM定时器
#define PWM_OUT LL_GPIO_IOMAP_OUT_TMR0_PWM_OUT // 定时器输出函数
#define IR_OUT LL_GPIO_IOMAP_OUT_TMR1_PWM_OUT  // 红外定时器输出函数
#define TIMER_PRD (SYS_CLK >> 10)			   // 定时器PWM周期
#define WS2812_PRD (SYS_CLK >> 20)			   // 灯带PWM周期
#define TIMER_IR_BCNT 24 * 30				   // 定时器红外码位数
#define RGB_BUF_LEN sizeof rgb_buf / 4		   // RGB缓存长度

#if LAB_AT_EN

u8 rx_dma_buf[RX_DMA_BUF_LEN];			  // DMA接收缓冲区
u32 led_mode = 0x00;					  // LED显示模式
u32 rgb_buf[] =							  // RGB数据缓冲区
	{0x00FF00FF, 0x00000000, 0xFFFF00FF,  // 红绿蓝青
	 0x00FFFFFF, 0xFF00FFFF, 0xFF000000}; // 紫黄白黑

ATTRIBUTE_ISR void UART0_IRQHandler() // 串口0中断服务函数
{
	if (UART0->STA & LL_UART_STA_TO_PEND) // 若串口0接收超时
	{
		if (strstr(rx_dma_buf, "at+baud")) // 若输入at+baud开头
		{
			int baud_rate;							   // 串口波特率
			TEST_INFO_SHOW("波特率->%d", baud_rate);   // 打印波特率
			sscanf(rx_dma_buf, "%[^=]=%d", baud_rate); // 提取波特率
			UART0->BAUD = SYS_CLK / baud_rate - 1;	   // 调整波特率
		}
		else if (strstr(rx_dma_buf, "at+bit")) // 若输入at+bit开头
		{
			int bit_width;							   // 串口数据位宽
			sscanf(rx_dma_buf, "%[^=]=%d", bit_width); // 提取数据位宽
			if (bit_width == 9)						   // 若为9位字长
				UART0->CON |= LL_UART_CON_BIT9_EN;	   // 使能9位数据传输
			else if (bit_width == 8)				   // 若为8位字长
				UART0->CON &= ~LL_UART_CON_BIT9_EN;	   // 关闭9位数据传输
			else
				TEST_INFO_SHOW("数据位宽：8~9");
		}
		else if (strstr(rx_dma_buf, "at+prd")) // 若输入at+prd开头
		{
			int period;										   // PWM周期值
			sscanf(rx_dma_buf, "%[^=]=%d", period);			   // 提取周期值
			if (period > 0 && period <= 1000)				   // 若周期值有效
				PWM_TIMER->TMR_CAP3 = SYS_CLK * period / 1000; // 调整周期值
			else
				TEST_INFO_SHOW("PWM周期值：0~1000ms");
		}
		else if (strstr(rx_dma_buf, "at+cmp")) // 若输入at+cmp开头
		{
			int compare;									// PWM比较值
			sscanf(rx_dma_buf, "%[^=]=%d", compare);		// 提取比较值
			if (compare >= 0 && compare <= 100)				// 若比较值有效
				PWM_TIMER->TMR_CAP4 = PWM_TIMER->TMR_CAP3 * // 调整比较值
									  compare / 100;
			else
				TEST_INFO_SHOW("PWM比较值：0~100%");
		}
		else if (strstr(rx_dma_buf, "at+led")) // 若输入at+led开头
		{
			int mode = 0;						  // 灯带显示模式
			sscanf(rx_dma_buf, "%[^=]=%d", mode); // 提取显示模式
			led_mode = mode;					  // 设置呼吸灯标志位
		}
		else if (strstr(rx_dma_buf, "at+?"))				   // 若输入at+?
			TEST_INFO_SHOW("AT指令：at+baud/bit/prd/cmp/led"); // 打印AT指令
		else												   // 若非at指令
			TEST_INFO_SHOW("%s", rx_dma_buf);				   // 返回发送数据

		memset(rx_dma_buf, 0, RX_DMA_BUF_LEN);	   // 清空接收缓冲区
		UART0->STA |= LL_UART_STA_TO_PEND;		   // 清除超时中断
		UART0->DMACON |= LL_UART_DMACON_RX_DMA_EN; // 重置DMA指针
	}
}

ATTRIBUTE_ISR void TIM1_IRQHandler() // 红外定时器中断函数
{
	if (IR_TIMER->TMR_FLG & LL_TIMER_IE_IR_TX_DONE_FLG) // 若定时器IR发送完成
	{
		static u8 bright = 0xFF; // 调光挡位
		static s8 direct = -1;	 // 调光方向
		static u8 count = 10;	 // 流水灯计数
		u32 *rgb = rgb_buf;		 // RGB数组指针

		if (led_mode & 0x01) // 若为呼吸灯模式
		{
			for (; rgb < rgb_buf + RGB_BUF_LEN; rgb++)				  // 遍历RGB数组
				for (u8 bit = 0; bit < 32; bit += 8)				  // 遍历RGB颜色位
					if (*rgb & 0xFF << bit)							  // 若此位存在颜色
						*rgb = *rgb & ~(0xFF << bit) | bright << bit; // 调整RGB颜色值
			if (bright + direct & 0xFF)								  // 若下一调光档有效
				bright += direct;									  // 调整调光挡位
			else													  // 若为挡位0或256
				direct = -direct;									  // 反转调光方向
		}

		if (led_mode & 0x02 && !count--) // 若为流水灯模式
		{
			u32 tmp = *rgb_buf;							   // RGB数组首元素值
			rgb = rgb_buf;								   // 重置RGB指针位置
			for (; rgb < rgb_buf + RGB_BUF_LEN - 1; rgb++) // 遍历RGB数组
				*rgb = *rgb << 24 | *(rgb + 1) >> 8;	   // 移动RGB数据
			*rgb = *rgb << 24 | tmp >> 8;				   // 数组尾元素补齐
			count = 10;									   // 重置流水灯计数
		}

		delay_ms(10);
		IR_TIMER->TMR_CLR |= LL_TIMER_IE_IR_TX_DONE_CLR; // 清除发送IR完成标志位
		IR_TIMER->TMR_EN = LL_TIMER_EN_TMREN;			 // 重启红外定时器模块
	}
}

void lab_at()
{
	u32 *tx_reg = &GPIOA->IOFUNCOUTCON0 + (TX_PIN >> 2);   // TX端输出控制寄存器
	u32 *rx_reg = &SYSCTRL->IOFUNCINCON0 + (UART_IN >> 2); // RX端输入控制寄存器
	u32 *pwm_reg = &GPIOA->IOFUNCOUTCON0 + (PWM_PIN >> 2); // PWM端输出控制寄存器
	u32 *ir_reg = &GPIOA->IOFUNCOUTCON0 + (IR_PIN >> 2);   // IR端输出控制寄存器

	*tx_reg |= UART_OUT << CFG_OFFSET(TX_PIN);	   // 设置发送引脚输出控制位
	*rx_reg &= ~(0xFF << CFG_OFFSET(UART_IN));	   // 清除串口输入引脚选择位
	*rx_reg |= RX_PIN << CFG_OFFSET(UART_IN);	   // 设置串口输入引脚选择位
	*pwm_reg |= PWM_OUT << CFG_OFFSET(PWM_PIN);	   // 设置PWM引脚输出控制位
	*ir_reg |= IR_OUT << CFG_OFFSET(IR_PIN);	   // 设置红外引脚输出控制位
	UART0->BAUD = SYS_CLK / UART_BAUDRATE - 1;	   // 设置串口0波特率
	UART0->CON |= LL_UART_CON_UART_EN;			   // 使能串口0模块
	GPIOA->MODE |= 0x1 << TX_PIN * 2 |			   // 设置发送引脚为输出模式
				   0x1 << PWM_PIN * 2 |			   // 设置PWM引脚为输出模式
				   0x1 << IR_PIN * 2;			   // 设置红外引脚为输出模式
	PWM_TIMER->TMR_CON |= LL_TIMER_MODE_PWM;	   // 设置PWM定时器模式为PWM
	PWM_TIMER->TMR_CAP1 = TIMER_PRD;			   // 设置PWM周期寄存器
	PWM_TIMER->TMR_CAP3 = TIMER_PRD;			   // 设置PWM周期影子寄存器
	PWM_TIMER->TMR_CAP2 = TIMER_PRD >> 1;		   // 设置PWM比较值寄存器
	PWM_TIMER->TMR_CAP4 = TIMER_PRD >> 1;		   // 设置PWM比较值影子寄存器
	PWM_TIMER->TMR_EN = LL_TIMER_EN_TMREN;		   // 使能PWM定时器模块
	IR_TIMER->TMR_CON |= LL_TIMER_MODE_PWM;		   // 设置红外定时器模式为PWM
	IR_TIMER->TMR_CAP1 = WS2812_PRD;			   // 设置红外周期寄存器
	IR_TIMER->TMR_CAP3 = WS2812_PRD;			   // 设置红外周期影子寄存器
	IR_TIMER->TMR_CAP2 = WS2812_PRD * 0.25;		   // 设置红外比较值寄存器
	IR_TIMER->TMR_CAP4 = WS2812_PRD * 0.75;		   // 设置红外比较值影子寄存器
	IR_TIMER->TMR_IR_BCNT = TIMER_IR_BCNT;		   // 设置定时器红外码位数
	IR_TIMER->TMR_CON |= LL_TIMER_CON_IR_EN;	   // 使能红外定时器红外功能
	IR_TIMER->TMR_DCTL |= LL_TIMER_DMA_LPBK |	   // 使能红外定时器DMA循环
						  LL_TIMER_DMA_EN;		   // 使能红外定时器DMA功能
	IR_TIMER->TMR_DADR = (u32)rgb_buf;			   // 设置红外定时器DMA地址
	IR_TIMER->TMR_DLEN = RGB_BUF_LEN - 1;		   // 设置红外定时器DMA长度
	IR_TIMER->TMR_IE |= LL_TIMER_IE_IR_TX_DONE_IE; // 使能定时器IR完成中断
	IR_TIMER->TMR_EN = LL_TIMER_EN_TMREN;		   // 使能红外定时器模块

	debug_select_interface(DEBUG_UART0); // 配置串口0为调试接口
	NVIC_EnableIRQ(TIM1_IRQn);

#if UART_DMA_EN									 // 若使能DMA模式
	NVIC_EnableIRQ(UART0_IRQn);					 // 使能串口0总中断
	UART0->RSTADR = rx_dma_buf;					 // 设置接收DMA地址
	UART0->RDMALEN = RX_DMA_BUF_LEN;			 // 设置接收DMA长度
	UART0->DMACON |= LL_UART_DMACON_RX_DMA_EN;	 // 使能接收DMA模块
	UART0->CON |= LL_UART_CON_UART_EN;			 // 使能串口0模块
	UART0->TOCON |= LL_UART_TOCON_TO_IE_EN |	 // 使能超时中断
					LL_UART_TOCON_TO_EN |		 // 使能超时模块
					LL_UART_TOCON_TO_BIT_LEN(2); // 设置超时位长

	while (TRUE) // 循环等待中断
		;
#else
	while (TRUE)
		if (UART0->STA & LL_UART_STA_RX_BUF_NOT_EMPTY) // 若接收缓存非空
			TEST_INFO_SHOW("%c", UART0->DATA);		   // 打印缓存数据
#endif
}
#endif
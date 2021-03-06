/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.c
*	版    本 : V1.1
*	说    明 : 硬件底层驱动程序集合
*
*	修改记录 :
*		版本号  日期         作者      说明
*		v1.0    2012-12-17  Eric2013  BSP驱动包V1.0
*		v1.1    2014-05-23  Eric2013  BSP驱动包V1.2
*	
*	Copyright (C), 2014-2015, 安富莱电子 www.delta.com
*
*********************************************************************************************************
*/
#include  <includes.h>
#include  "MainTask.h"

/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/ 
#define SysLineNum     8     /* 系统信息要在LCD上显示8行 */
#define LineCap       16     /* 字符点阵的高是16         */
#define UserLineNum   40     /* 供用户使用的行数是40行   */
#define BytesPerLine  50     /* 每行支持50个字节         */

/*
*********************************************************************************************************
*                                             函数和变量
*********************************************************************************************************
*/  
void NVIC_Configuration(void);
extern void uip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate);
extern void lwip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate);
extern __IO uint32_t  EthInitStatus;
extern __IO uint8_t EthLinkStatus;

/*
*********************************************************************************************************
*                                             寄存器
*********************************************************************************************************
*/
#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)

/*
*********************************************************************************************************
*                                            寄存器位
*********************************************************************************************************
*/

#define  BSP_DBGMCU_CR_TRACE_IOEN_MASK                   0x10
#define  BSP_DBGMCU_CR_TRACE_MODE_ASYNC                  0x00
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_01                0x40
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_02                0x80
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04                0xC0
#define  BSP_DBGMCU_CR_TRACE_MODE_MASK                   0xC0

#define  BSP_BIT_DEM_CR_TRCENA                    DEF_BIT_24

#define  BSP_BIT_DWT_CR_CYCCNTENA                 DEF_BIT_00

/*
*********************************************************************************************************
*	函 数 名: DispDelay
*	功能说明: 显示延迟
*	形    参：nCount  延迟计数
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispDelay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

/*
*********************************************************************************************************
*	函 数 名: HW_InitDisp
*	功能说明: 用于在屏上滚动显示初始化结果
*	形    参：_ucLineNum  屏上最大支持的显示行数
*             _ucNum      当前要显示的行
*             DispAdress  要显示信息的地址指针
*	返 回 值: 无
*********************************************************************************************************
*/
void HW_InitDisp(uint8_t _ucLineNum, uint16_t _ucNum, char *DispAdress, uint8_t *_FontColor)
{
	uint16_t i; 
	static uint16_t offset;          /* 整屏显示满以后实现滚动显示的效果，用于记录首位置 */
	uint16_t y = SysLineNum*LineCap; /* 减去前面的系统信息后，记录硬件初始化开始位置  */
	
	/*  
		更新偏移地址，显示满以后实现滚动显示的效果 
		后面+1是因为_ucNum从0开始计数。
	*/
	if(_ucNum >= _ucLineNum)
	{
		offset = (_ucNum/_ucLineNum - 1) * _ucLineNum   + _ucNum%_ucLineNum + 1;
	}
	/* 没有显示满之前还是从0开始 */
	else
	{
		offset = 0;
	}
	
	/* 刷新显示 用函数GUI_DispStringAtCEOL是因为滚动的时候后面字符要清除，防止重叠 */
	for(i = 0; i < _ucLineNum; i++)
	{
		/* 初始化出错的话显示红色，否则显示白色 */
		if(_FontColor[i+offset] == 0)
		{
			GUI_SetColor(GUI_WHITE);
		}
		else
		{
			GUI_SetColor(GUI_RED);
		}
		
		GUI_DispStringAtCEOL(DispAdress+(i+offset)*50,0, y);
		y += LineCap;
	}
	
	DispDelay(4000000);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备, 初始化过程中在LCD上滚动显示初始化结果。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	char buf[UserLineNum*BytesPerLine];    /* 用于存储每行要显示的字符，最大支持40行，每行50个字节 */
	uint8_t ErrRecord[UserLineNum];        /* 用于初始化字体颜色记录，0:表示白色 1：表示红色 */
	uint16_t y;				               /* Y轴的递增 */
	uint16_t usLineNum;                    /* 字符是usLineCap点阵时，整个屏可以显示的行数 */
	uint16_t usLineCurrent;                /* 当前显示的行数 */
	
	/* 使能CRC 因为使用STemWin前必须要使能 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
	/* 第1步：初始化NVIC 中断优先级分组配置 ***************************************/
	NVIC_Configuration();  
	
	/* 第2步：初始化串口FIFO ******************************************************/
	bsp_InitUart(); 

	/* 第3步：初始化LED ***********************************************************/
	bsp_InitLed(); 	

	/* 第4步：初始化按键 **********************************************************/
	bsp_InitKey(); 	       
	
	/* 第5步：初始化RTC ***********************************************************/
	bsp_InitRTC(); 
	
	/* 第6步：初始化I2C总线的IO引脚配置 *******************************************/	
    bsp_InitI2C();	  	  
	bsp_InitSPIBus();
	
	
	/* 第7步：初始化以下四款传感器 ************************************************/	
    bsp_InitHMC5883L();
    bsp_InitMPU6050();
    bsp_InitBH1750();
    bsp_InitBMP085();
	
	/* 第8步：初始化SPI FLASH  ***************************************************/
	bsp_InitSFlash();
	
	/* 第9步：NOR FLASH 初始化 **************************************************/	
norflash_failed:

    bsp_InitNorFlash();		/* 初始化FSMC NOR Flash */
    if (NOR_ReadID() != S29GL128P)
    {
        printf("norflash_failed");
        goto norflash_failed;
    }

	/* 第10步：EEPROM初始化 ****************************************************/	
eeprom_failed:
    if (ee_CheckOk() == 0)
	{
		/* 没有检测到EEPROM */
		printf("eeprom_failed");
        goto eeprom_failed;
	}
	
	/* 第11步：初始化以下四款传感器 ********************************************/	
	bsp_InitExtSRAM();
	
	/* 第12步：LCD初始化 *******************************************************/	
  	LCD_InitHard();	   
	
	/* 第13步：触摸初始化 ******************************************************/	
	TOUCH_InitHard();

	/**************************************************************************/
	GUI_Init();

	/* 获取整屏可以显示字符的行数 */
	usLineNum = LCD_GetYSize() / LineCap;
	usLineNum = usLineNum - SysLineNum;
	
	/* 设置颜色，绿底红字 */
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_GREEN);
	GUI_SetFont(&GUI_FontHZ_SimSun_16);	
	
	/* 第1步：显示系统信息 *************************************************************************/
	y = 0;
	GUI_DispStringAt("*************************************************************", 0, y);
	y += LineCap;
	GUI_DispStringAt("安富莱STM32-V5开发板的综合测试程序V1.2d版本", 0, y);
	y += LineCap;
	GUI_DispStringAt("   STemWin--V5.28", 0, y);
	y += LineCap;
	GUI_DispStringAt("   uCOS-III--V3.03.01", 0, y);
	y += LineCap;
	GUI_DispStringAt("   Lwip--V1.3.2", 0, y);
	y += LineCap;
	GUI_DispStringAt("   FatFS--V0.11", 0, y);
	y += LineCap;
	GUI_DispStringAt("Copyright (C)2015-2016 安富莱电子", 0, y);
	y += LineCap;
	GUI_DispStringAt("*************************************************************", 0, y);
	
	/* 当前行从0开始 */
	usLineCurrent = 0;
	
	/* 第2步：显示CPU ID *************************************************************************/
	{
		/* 参考手册：
			32.6.1 MCU device ID code
			33.1 Unique device ID register (96 bits)
		*/
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

		CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);
		
		sprintf(&buf[usLineCurrent*BytesPerLine], "1 CPU : STM32F407IGT6, LQFP176");
		ErrRecord[usLineCurrent] = 0;
		HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		
		sprintf(&buf[usLineCurrent*BytesPerLine], "2 UID = %08X %08X %08X", CPU_Sn2, CPU_Sn1, CPU_Sn0);
		ErrRecord[usLineCurrent] = 0;
		HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
	}
	
	
	/* 第3步：显示NOR Flash *************************************************************************/
	{
		uint32_t id;

		id = NOR_ReadID();

		if (id == S29GL128P)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "4 NOR Flash ID = 0x%08X, OK", id);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "4 NOR Flash ID = 0x%08X, Model = xxxx, Err", id);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
	}

	/* 第4步：显示I2C设备 *************************************************************************/
	{
		if (i2c_CheckDevice(EE_DEV_ADDR) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "5 AT24C128 Ok (0x%02X)", EE_DEV_ADDR);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "5 AT24C128 Err (0x%02X)", EE_DEV_ADDR);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}

		if (i2c_CheckDevice(I2C_ADDR_SI4730_W) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "6 Si4730 Ok (0x%02X)", I2C_ADDR_SI4730_W);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "6 Si4730 Err (0x%02X)", I2C_ADDR_SI4730_W);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}

		if (i2c_CheckDevice(HMC5883L_SLAVE_ADDRESS) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "7 HMC5883L Ok (0x%02X)", HMC5883L_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "7 HMC5883L Err (0x%02X)", HMC5883L_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}

		if (i2c_CheckDevice(MPU6050_SLAVE_ADDRESS) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "8 MPU6050 Ok (0x%02X)", MPU6050_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "8 MPU6050 Err (0x%02X)", MPU6050_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}

		if (i2c_CheckDevice(BMP085_SLAVE_ADDRESS) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "9 BMP085 Ok (0x%02X)", BMP085_SLAVE_ADDRESS);
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "9 BMP085 Err (0x%02X)", BMP085_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}

		if (i2c_CheckDevice(WM8978_SLAVE_ADDRESS) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "10 WM8978 Ok (0x%02X)", WM8978_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "10 WM8978 Err (0x%02X)", WM8978_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}

		if (i2c_CheckDevice(BH1750_SLAVE_ADDRESS) == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "11 BH1750 Ok (0x%02X)", BH1750_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "11 BH1750 Err (0x%02X)", BH1750_SLAVE_ADDRESS);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
	}
	
	/* 第5步：显示UIP *************************************************************************/
#if APP_CFG_UIP_ENABLED
	{
		uint32_t id;
		uip_start(g_tParam.uip_ip, g_tParam.uip_net_mask, g_tParam.uip_gateway);
		id = dm9k_ReadID();
		if (id == 0x0A469000)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "12 DM9000 ID = 0x%08X, Ok", id);
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "12 DM9000 ID = 0x%08X, Err", id);
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
	}
#endif /* #if APP_CFG_UIP_ENABLED */
	
	/* 第6步：显示Lwip *************************************************************************/
	{
		/* 如果不插网线，此函数执行时间过长 */
		lwip_start(g_tParam.lwip_ip, g_tParam.lwip_net_mask, g_tParam.lwip_gateway);
		
		/* EthInitStatus 和 EthLinkStatus 是LwIP模块中的全局变量 */
		if (EthInitStatus == 0)
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "13 DM9161 PHY Err");
			ErrRecord[usLineCurrent] = 1;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
		else
		{
			sprintf(&buf[usLineCurrent*BytesPerLine], "13 DM9161 PHY OK");
			ErrRecord[usLineCurrent] = 0;
			HW_InitDisp(usLineNum, usLineCurrent++, buf, ErrRecord);
		}
	}
			
#ifdef TRACE_EN                                                 /* See project / compiler preprocessor options.         */
    BSP_CPU_REG_DBGMCU_CR |=  BSP_DBGMCU_CR_TRACE_IOEN_MASK;    /* Enable tracing (see Note #2).                        */
    BSP_CPU_REG_DBGMCU_CR &= ~BSP_DBGMCU_CR_TRACE_MODE_MASK;    /* Clr trace mode sel bits.                             */
    BSP_CPU_REG_DBGMCU_CR |=  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04; /* Cfg trace mode to synch 4-bit.                       */
#endif
}

/*
*********************************************************************************************************
*	函 数 名: NVIC_Configuration
*	功能说明: 中断优先级配置，这里先把优先级分组分配好，然后在
*			  具体的模块里面实现优先级配置 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void NVIC_Configuration(void)
{			
	/* 设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DelayMS
*	功能说明: 为了让底层驱动在带RTOS和裸机情况下有更好的兼容性
*             专门制作一个阻塞式的延迟函数，在底层驱动中ms毫秒延迟主要用于初始化，并不会影响实时性。
*	形    参: n 延迟长度，单位1 ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t _ulDelayTime)
{
	bsp_DelayUS(1000*_ulDelayTime);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DelayUS
*	功能说明: 这里的延时采用CPU的内部计数实现，32位计数器
*             	OSSchedLock(&err);
*				bsp_DelayUS(5);
*				OSSchedUnlock(&err); 根据实际情况看看是否需要加调度锁或选择关中断
*	形    参: _ulDelayTime  延迟长度，单位1 us
*	返 回 值: 无
*   说    明: 1. 主频168MHz的情况下，32位计数器计满是2^32/168000000 = 25.565秒
*                建议使用本函数做延迟的话，延迟在1秒以下。  
*             2. 实际通过示波器测试，微妙延迟函数比实际设置实际多运行0.25us左右的时间。
*             下面数据测试条件：
*             （1）. MDK5.15，优化等级0, 不同的MDK优化等级对其没有影响。
*             （2）. STM32F407IGT6
*             （3）. 测试方法：
*				 GPIOI->BSRRL = GPIO_Pin_8;
*				 bsp_DelayUS(10);
*				 GPIOI->BSRRH = GPIO_Pin_8;
*             -------------------------------------------
*                测试                 实际执行
*             bsp_DelayUS(1)          1.2360us
*             bsp_DelayUS(2)          2.256us
*             bsp_DelayUS(3)          3.256us
*             bsp_DelayUS(4)          4.256us
*             bsp_DelayUS(5)          5.276us
*             bsp_DelayUS(6)          6.276us
*             bsp_DelayUS(7)          7.276us
*             bsp_DelayUS(8)          8.276us
*             bsp_DelayUS(9)          9.276us
*             bsp_DelayUS(10)         10.28us
*            3. 两个32位无符号数相减，获取的结果再赋值给32位无符号数依然可以正确的获取差值。
*              假如A,B,C都是32位无符号数。
*              如果A > B  那么A - B = C，这个很好理解，完全没有问题
*              如果A < B  那么A - B = C， C的数值就是0xFFFFFFFF - B + A + 1。这一点要特别注意，正好用于本函数。
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t tCnt, tDelayCnt;
	uint32_t tStart;
		
	tStart = (uint32_t)CPU_TS_TmrRd();                       /* 刚进入时的计数器值 */
	tCnt = 0;
	tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */ 		      

	while(tCnt < tDelayCnt)
	{
		tCnt = (uint32_t)CPU_TS_TmrRd() - tStart; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */	
	}
}

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


/*
*********************************************************************************************************
*                                            BSP_Tick_Init()
*
* Description : Initialize all the peripherals that required OS Tick services (OS initialized)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_Tick_Init (void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    
#if (OS_VERSION >= 30000u)
    cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;       /* Determine nbr SysTick increments.                    */
#else
    cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* Determine nbr SysTick increments.                    */
#endif
    
//   OS_CPU_SysTickInit(cnts);                                 /* 这里默认的是最高优先级，根据实际情况修改             */
	SysTick_Config(cnts);   //这里默认的是最低优先级
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_TS_Init().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer 
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR' 
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater 
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR' 
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be 
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets 
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple 
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the 
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time 
*                       but MUST be less than the maximum measured time; otherwise, timer resolution 
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #2'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;    /* Enable Cortex-M4's DWT CYCCNT reg.                   */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : Timestamp timer count (see Notes #2a & #2b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_Get32(),
*               CPU_TS_Get64(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer 
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR' 
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater 
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR' 
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be 
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets 
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple 
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the 
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is 
*                           calculated by either of the following equations :
*
*                           (A) Time measured  =  Number timer counts  *  Timer period
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured 
*                                       Timer period            Timer's period in some units of 
*                                                                   (fractional) seconds
*                                       Time measured           Amount of time measured, in same 
*                                                                   units of (fractional) seconds 
*                                                                   as the Timer period
*
*                                                  Number timer counts
*                           (B) Time measured  =  ---------------------
*                                                    Timer frequency
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer frequency         Timer's frequency in some units 
*                                                                   of counts per second
*                                       Time measured           Amount of time measured, in seconds
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less 
*                           than the maximum measured time; otherwise, timer resolution inadequate to 
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;

                                                                
    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         CPU_TSxx_to_uSec()
*
* Description : Convert a 32-/64-bit CPU timestamp from timer counts to microseconds.
*
* Argument(s) : ts_cnts   CPU timestamp (in timestamp timer counts [see Note #2aA]).
*
* Return(s)   : Converted CPU timestamp (in microseconds           [see Note #2aD]).
*
* Caller(s)   : Application.
*
*               This function is an (optional) CPU module application programming interface (API) 
*               function which MAY be implemented by application/BSP function(s) [see Note #1] & 
*               MAY be called by application function(s).
*
* Note(s)     : (1) CPU_TS32_to_uSec()/CPU_TS64_to_uSec() are application/BSP functions that MAY be 
*                   optionally defined by the developer when either of the following CPU features is 
*                   enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) The amount of time measured by CPU timestamps is calculated by either of 
*                       the following equations :
*
*                                                                        10^6 microseconds
*                       (1) Time measured  =   Number timer counts   *  -------------------  *  Timer period
*                                                                            1 second
*
*                                              Number timer counts       10^6 microseconds
*                       (2) Time measured  =  ---------------------  *  -------------------
*                                                Timer frequency             1 second
*
*                               where
*
*                                   (A) Number timer counts     Number of timer counts measured
*                                   (B) Timer frequency         Timer's frequency in some units 
*                                                                   of counts per second
*                                   (C) Timer period            Timer's period in some units of 
*                                                                   (fractional)  seconds
*                                   (D) Time measured           Amount of time measured, 
*                                                                   in microseconds
*
*                   (b) Timer period SHOULD be less than the typical measured time but MUST be less 
*                       than the maximum measured time; otherwise, timer resolution inadequate to 
*                       measure desired times.
*
*                   (c) Specific implementations may convert any number of CPU_TS32 or CPU_TS64 bits 
*                       -- up to 32 or 64, respectively -- into microseconds.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;

    
    fclk_freq = BSP_CPU_ClkFreq();    
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;
    

    fclk_freq = BSP_CPU_ClkFreq();    
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif

/***************************** 安富莱电子 www.delta.com (END OF FILE) *********************************/

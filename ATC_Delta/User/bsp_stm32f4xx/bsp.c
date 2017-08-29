/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.c
*	��    �� : V1.1
*	˵    �� : Ӳ���ײ��������򼯺�
*
*	�޸ļ�¼ :
*		�汾��  ����         ����      ˵��
*		v1.0    2012-12-17  Eric2013  BSP������V1.0
*		v1.1    2014-05-23  Eric2013  BSP������V1.2
*	
*	Copyright (C), 2014-2015, ���������� www.delta.com
*
*********************************************************************************************************
*/
#include  <includes.h>
#include  "MainTask.h"

/*
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/ 
#define SysLineNum     8     /* ϵͳ��ϢҪ��LCD����ʾ8�� */
#define LineCap       16     /* �ַ�����ĸ���16         */
#define UserLineNum   40     /* ���û�ʹ�õ�������40��   */
#define BytesPerLine  50     /* ÿ��֧��50���ֽ�         */

/*
*********************************************************************************************************
*                                             �����ͱ���
*********************************************************************************************************
*/  
void NVIC_Configuration(void);
extern void uip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate);
extern void lwip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate);
extern __IO uint32_t  EthInitStatus;
extern __IO uint8_t EthLinkStatus;

/*
*********************************************************************************************************
*                                             �Ĵ���
*********************************************************************************************************
*/
#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)

/*
*********************************************************************************************************
*                                            �Ĵ���λ
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
*	�� �� ��: DispDelay
*	����˵��: ��ʾ�ӳ�
*	��    �Σ�nCount  �ӳټ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispDelay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

/*
*********************************************************************************************************
*	�� �� ��: HW_InitDisp
*	����˵��: ���������Ϲ�����ʾ��ʼ�����
*	��    �Σ�_ucLineNum  �������֧�ֵ���ʾ����
*             _ucNum      ��ǰҪ��ʾ����
*             DispAdress  Ҫ��ʾ��Ϣ�ĵ�ַָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HW_InitDisp(uint8_t _ucLineNum, uint16_t _ucNum, char *DispAdress, uint8_t *_FontColor)
{
	uint16_t i; 
	static uint16_t offset;          /* ������ʾ���Ժ�ʵ�ֹ�����ʾ��Ч�������ڼ�¼��λ�� */
	uint16_t y = SysLineNum*LineCap; /* ��ȥǰ���ϵͳ��Ϣ�󣬼�¼Ӳ����ʼ����ʼλ��  */
	
	/*  
		����ƫ�Ƶ�ַ����ʾ���Ժ�ʵ�ֹ�����ʾ��Ч�� 
		����+1����Ϊ_ucNum��0��ʼ������
	*/
	if(_ucNum >= _ucLineNum)
	{
		offset = (_ucNum/_ucLineNum - 1) * _ucLineNum   + _ucNum%_ucLineNum + 1;
	}
	/* û����ʾ��֮ǰ���Ǵ�0��ʼ */
	else
	{
		offset = 0;
	}
	
	/* ˢ����ʾ �ú���GUI_DispStringAtCEOL����Ϊ������ʱ������ַ�Ҫ�������ֹ�ص� */
	for(i = 0; i < _ucLineNum; i++)
	{
		/* ��ʼ������Ļ���ʾ��ɫ��������ʾ��ɫ */
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
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ��Ӳ���豸, ��ʼ����������LCD�Ϲ�����ʾ��ʼ�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
	char buf[UserLineNum*BytesPerLine];    /* ���ڴ洢ÿ��Ҫ��ʾ���ַ������֧��40�У�ÿ��50���ֽ� */
	uint8_t ErrRecord[UserLineNum];        /* ���ڳ�ʼ��������ɫ��¼��0:��ʾ��ɫ 1����ʾ��ɫ */
	uint16_t y;				               /* Y��ĵ��� */
	uint16_t usLineNum;                    /* �ַ���usLineCap����ʱ��������������ʾ������ */
	uint16_t usLineCurrent;                /* ��ǰ��ʾ������ */
	
	/* ʹ��CRC ��Ϊʹ��STemWinǰ����Ҫʹ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
	/* ��1������ʼ��NVIC �ж����ȼ��������� ***************************************/
	NVIC_Configuration();  
	
	/* ��2������ʼ������FIFO ******************************************************/
	bsp_InitUart(); 

	/* ��3������ʼ��LED ***********************************************************/
	bsp_InitLed(); 	

	/* ��4������ʼ������ **********************************************************/
	bsp_InitKey(); 	       
	
	/* ��5������ʼ��RTC ***********************************************************/
	bsp_InitRTC(); 
	
	/* ��6������ʼ��I2C���ߵ�IO�������� *******************************************/	
    bsp_InitI2C();	  	  
	bsp_InitSPIBus();
	
	
	/* ��7������ʼ�������Ŀ���� ************************************************/	
    bsp_InitHMC5883L();
    bsp_InitMPU6050();
    bsp_InitBH1750();
    bsp_InitBMP085();
	
	/* ��8������ʼ��SPI FLASH  ***************************************************/
	bsp_InitSFlash();
	
	/* ��9����NOR FLASH ��ʼ�� **************************************************/	
norflash_failed:

    bsp_InitNorFlash();		/* ��ʼ��FSMC NOR Flash */
    if (NOR_ReadID() != S29GL128P)
    {
        printf("norflash_failed");
        goto norflash_failed;
    }

	/* ��10����EEPROM��ʼ�� ****************************************************/	
eeprom_failed:
    if (ee_CheckOk() == 0)
	{
		/* û�м�⵽EEPROM */
		printf("eeprom_failed");
        goto eeprom_failed;
	}
	
	/* ��11������ʼ�������Ŀ���� ********************************************/	
	bsp_InitExtSRAM();
	
	/* ��12����LCD��ʼ�� *******************************************************/	
  	LCD_InitHard();	   
	
	/* ��13����������ʼ�� ******************************************************/	
	TOUCH_InitHard();

	/**************************************************************************/
	GUI_Init();

	/* ��ȡ����������ʾ�ַ������� */
	usLineNum = LCD_GetYSize() / LineCap;
	usLineNum = usLineNum - SysLineNum;
	
	/* ������ɫ���̵׺��� */
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_GREEN);
	GUI_SetFont(&GUI_FontHZ_SimSun_16);	
	
	/* ��1������ʾϵͳ��Ϣ *************************************************************************/
	y = 0;
	GUI_DispStringAt("*************************************************************", 0, y);
	y += LineCap;
	GUI_DispStringAt("������STM32-V5��������ۺϲ��Գ���V1.2d�汾", 0, y);
	y += LineCap;
	GUI_DispStringAt("   STemWin--V5.28", 0, y);
	y += LineCap;
	GUI_DispStringAt("   uCOS-III--V3.03.01", 0, y);
	y += LineCap;
	GUI_DispStringAt("   Lwip--V1.3.2", 0, y);
	y += LineCap;
	GUI_DispStringAt("   FatFS--V0.11", 0, y);
	y += LineCap;
	GUI_DispStringAt("Copyright (C)2015-2016 ����������", 0, y);
	y += LineCap;
	GUI_DispStringAt("*************************************************************", 0, y);
	
	/* ��ǰ�д�0��ʼ */
	usLineCurrent = 0;
	
	/* ��2������ʾCPU ID *************************************************************************/
	{
		/* �ο��ֲ᣺
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
	
	
	/* ��3������ʾNOR Flash *************************************************************************/
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

	/* ��4������ʾI2C�豸 *************************************************************************/
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
	
	/* ��5������ʾUIP *************************************************************************/
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
	
	/* ��6������ʾLwip *************************************************************************/
	{
		/* ����������ߣ��˺���ִ��ʱ����� */
		lwip_start(g_tParam.lwip_ip, g_tParam.lwip_net_mask, g_tParam.lwip_gateway);
		
		/* EthInitStatus �� EthLinkStatus ��LwIPģ���е�ȫ�ֱ��� */
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
*	�� �� ��: NVIC_Configuration
*	����˵��: �ж����ȼ����ã������Ȱ����ȼ��������ã�Ȼ����
*			  �����ģ������ʵ�����ȼ����� 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void NVIC_Configuration(void)
{			
	/* ����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayMS
*	����˵��: Ϊ���õײ������ڴ�RTOS�����������и��õļ�����
*             ר������һ������ʽ���ӳٺ������ڵײ�������ms�����ӳ���Ҫ���ڳ�ʼ����������Ӱ��ʵʱ�ԡ�
*	��    ��: n �ӳٳ��ȣ���λ1 ms
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t _ulDelayTime)
{
	bsp_DelayUS(1000*_ulDelayTime);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayUS
*	����˵��: �������ʱ����CPU���ڲ�����ʵ�֣�32λ������
*             	OSSchedLock(&err);
*				bsp_DelayUS(5);
*				OSSchedUnlock(&err); ����ʵ����������Ƿ���Ҫ�ӵ�������ѡ����ж�
*	��    ��: _ulDelayTime  �ӳٳ��ȣ���λ1 us
*	�� �� ֵ: ��
*   ˵    ��: 1. ��Ƶ168MHz������£�32λ������������2^32/168000000 = 25.565��
*                ����ʹ�ñ��������ӳٵĻ����ӳ���1�����¡�  
*             2. ʵ��ͨ��ʾ�������ԣ�΢���ӳٺ�����ʵ������ʵ�ʶ�����0.25us���ҵ�ʱ�䡣
*             �������ݲ���������
*             ��1��. MDK5.15���Ż��ȼ�0, ��ͬ��MDK�Ż��ȼ�����û��Ӱ�졣
*             ��2��. STM32F407IGT6
*             ��3��. ���Է�����
*				 GPIOI->BSRRL = GPIO_Pin_8;
*				 bsp_DelayUS(10);
*				 GPIOI->BSRRH = GPIO_Pin_8;
*             -------------------------------------------
*                ����                 ʵ��ִ��
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
*            3. ����32λ�޷������������ȡ�Ľ���ٸ�ֵ��32λ�޷�������Ȼ������ȷ�Ļ�ȡ��ֵ��
*              ����A,B,C����32λ�޷�������
*              ���A > B  ��ôA - B = C������ܺ���⣬��ȫû������
*              ���A < B  ��ôA - B = C�� C����ֵ����0xFFFFFFFF - B + A + 1����һ��Ҫ�ر�ע�⣬�������ڱ�������
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t tCnt, tDelayCnt;
	uint32_t tStart;
		
	tStart = (uint32_t)CPU_TS_TmrRd();                       /* �ս���ʱ�ļ�����ֵ */
	tCnt = 0;
	tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */ 		      

	while(tCnt < tDelayCnt)
	{
		tCnt = (uint32_t)CPU_TS_TmrRd() - tStart; /* ��������У����������һ��32λ���������¼�������Ȼ������ȷ���� */	
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
    
//   OS_CPU_SysTickInit(cnts);                                 /* ����Ĭ�ϵ���������ȼ�������ʵ������޸�             */
	SysTick_Config(cnts);   //����Ĭ�ϵ���������ȼ�
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

/***************************** ���������� www.delta.com (END OF FILE) *********************************/

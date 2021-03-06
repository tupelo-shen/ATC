/*
*********************************************************************************************************
*	                                  
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.2d
*	说    明 : 例程
*	修改记录 : 
*		版本号  日期         作者        说明
*		v1.0    2013-03-26  Eric2013    ST固件库V1.0.2版本
*		v1.1    2014-05-23  Eric2013    1. 升级ST固件库到V1.3.0
*                                       2. STemWin到V5.22
*                                       3. BSP驱动包到V1.2
*                                       4. 修改显示界面自适应4.3寸，5寸和7寸。
*                                       5. 加入触摸校准功能。
*       V1.2    2014-07-19  Eric2013    1. 升级STemWin到V5.24b
*                                       2. 升级FatFS到V0.10b
*                                       3. 加入新的触摸校准算法，触摸更加准确和灵敏
*                                       4. 实现快速的背景图片绘制
*                                       5. 其它更新看txt文档更新记录
*       V1.2a   2014-10-21  Eric2013    1. 修改RTC闹钟中断使能了，但没有写RTC中断函数的错误。
*       V1.2b   2014-12-05  Eric2013    1. 暂时删除背景图片的显示
*                                       2. 加入四点触摸校准。
*       V1.2c   2015-05-10  Eric2013    1. 升级固件库到V1.5.0
*       								2. 升级BSP板级支持包 
*      									3. 升级fatfs到0.11
*       								4. 升级STemWin到5.26
*       								5. 添加7寸800*480分辨率电容屏支持，添加3.5寸480*320的ILI9488支持。
*      									6. 在应用App_FileBrowse中通过函数CHOOSEFILE_SetButtonText修改按键大小，以便电容触摸屏可以点击到。
*	    V1.2d  2015-12-18  Eric2013     1. 升级BSP板级支持包 
*                                       2. 升级STemWin到5.28
*                                       3. 添加4.3寸和5寸电容屏支持。
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include  <includes.h>
#include  "MainTask.h"

/*
*********************************************************************************************************
*                                      宏定义
*********************************************************************************************************
*/  
#define DEMO_PAGE_COUNT		7	/* 演示页面的个数 */
#define OSTCBSize  sizeof(OS_TCB)/sizeof(CPU_STK) + 1 

/*
*********************************************************************************************************
*                                  本文件使用的全局变量
*********************************************************************************************************
*/                                                            
static  OS_TCB   *AppTaskStartTCB;
static  CPU_STK  *AppTaskStartStk;

static  OS_TCB   *AppTaskUpdateTCB;
static  CPU_STK  *AppTaskUpdateStk;

static  OS_TCB   *AppTaskCOMTCB;
static  CPU_STK  *AppTaskCOMStk;

static  OS_TCB   *AppTaskUserIFTCB;
static  CPU_STK  *AppTaskUserIFStk;

static  OS_TCB   *AppTaskGUITCB;
static  CPU_STK  *AppTaskGUIStk;

static  OS_TCB   *AppTaskGUIRefreshTCB;
static  CPU_STK  *AppTaskGUIRefreshStk;

static  OS_SEM     	SEM_SYNCH;	   /* 此信号量用于实现按键按下后进行屏幕截图       */
        OS_SEM     	SEM_OV7670;	   /* 此信号量用于进入摄像头界面后，按下K3进行退出 */

/*
*********************************************************************************************************
*                                        本文件内部的函数
*********************************************************************************************************
*/
static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);
static void   AppTaskUserIF         (void     *p_arg);
static void   AppTaskGUI            (void     *p_arg);
static void   AppTaskGUIRefresh     (void     *p_arg);
static void   AppTaskCOM			(void 	  *p_arg);

/*
*********************************************************************************************************
*	函 数 名: Stack_Init
*	功能说明: 任务和系统堆栈全部使用CCM，任务控制块也使用CCM空间。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void Stack_Init(void)
{
	CPU_STK *STK_Temp;
	OS_TCB *TCB_Temp;

/***********用户任务控制块，共6个任务的TCB*********************/	
	TCB_Temp = (OS_TCB *)0x10000000;
	AppTaskStartTCB = TCB_Temp;
	TCB_Temp ++; 

	AppTaskUpdateTCB = TCB_Temp;
	TCB_Temp ++; 

	AppTaskCOMTCB = TCB_Temp;
	TCB_Temp ++;
	 
	AppTaskUserIFTCB = TCB_Temp;
	TCB_Temp ++; 

	AppTaskGUITCB = TCB_Temp;
	TCB_Temp ++; 

	AppTaskGUIRefreshTCB = TCB_Temp;
	TCB_Temp ++; 

/***********系统任务控制块**os.h 1133行修改变量*******************/
//#if (OS_CFG_TMR_EN > 0u)	
//	OSTmrTaskTCB = TCB_Temp;/***os_tmr***/
//	TCB_Temp ++;
//#endif
//	  
//	OSTickTaskTCB = TCB_Temp;/***os_stick***/
//	TCB_Temp ++; 
//
//#if OS_CFG_STAT_TASK_EN > 0u
//	OSStatTaskTCB = TCB_Temp;/***os_stat***/
//	TCB_Temp ++; 
//#endif
//
//#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
//	OSIntQTaskTCB = TCB_Temp;/***os_int***/
//	TCB_Temp ++; 
//#endif
//
//	OSIdleTaskTCB = TCB_Temp;/***os_core 817行*/ 
//	TCB_Temp ++; 

/***********用户任务堆栈空间*************************************/
	STK_Temp = (CPU_STK *)TCB_Temp; 
	AppTaskStartStk = STK_Temp;

	STK_Temp = STK_Temp + APP_CFG_TASK_START_STK_SIZE; 	
	AppTaskUpdateStk = STK_Temp;

	STK_Temp = STK_Temp + APP_CFG_TASK_UPDATE_STK_SIZE; 	
	AppTaskCOMStk = STK_Temp;

	STK_Temp = STK_Temp + APP_CFG_TASK_COM_STK_SIZE; 	
	AppTaskUserIFStk = STK_Temp;

	STK_Temp = STK_Temp + APP_CFG_TASK_USER_IF_STK_SIZE; 	
	AppTaskGUIStk = STK_Temp;

	STK_Temp = STK_Temp + APP_CFG_TASK_GUI_STK_SIZE;
	AppTaskGUIRefreshStk = STK_Temp;

/**********下面的6个是UCOS-III的内核服务程序*****************/
	STK_Temp = STK_Temp + APP_CFG_TASK_GUIRefresh_STK_SIZE;
	OSCfg_IdleTaskStk = STK_Temp;
	OSCfg_IdleTaskStkBasePtr = OSCfg_IdleTaskStk;
	STK_Temp = STK_Temp + OS_CFG_IDLE_TASK_STK_SIZE; 

/***************中断队列任务*********************************/
#if (OS_CFG_ISR_POST_DEFERRED_EN > 0u)
	
	OSCfg_IntQTaskStk = STK_Temp;
	OSCfg_IntQTaskStkBasePtr = OSCfg_IntQTaskStk;
	STK_Temp = STK_Temp + OS_CFG_INT_Q_TASK_STK_SIZE;
#else
	OSCfg_IntQTaskStkBasePtr = (CPU_STK *)0; 
#endif 

/****************ISR堆栈*************************************/
#if (OS_CFG_ISR_STK_SIZE > 0u)
	OSCfg_ISRStk = STK_Temp;
	OSCfg_ISRStkBasePtr = OSCfg_ISRStk;
	STK_Temp = STK_Temp + OS_CFG_ISR_STK_SIZE;
#else
	OSCfg_ISRStkBasePtr = (CPU_STK *)0; 
#endif

/**************统计任务*************************************/
#if (OS_CFG_STAT_TASK_EN > 0u)
	OSCfg_StatTaskStk = STK_Temp;
	OSCfg_StatTaskStkBasePtr = OSCfg_StatTaskStk;
	STK_Temp = STK_Temp + OS_CFG_STAT_TASK_STK_SIZE;
#else
	OSCfg_StatTaskStkBasePtr = (CPU_STK *)0; 
#endif

/****************滴答任务**********************************/	
	OSCfg_TickTaskStk = STK_Temp;
	OSCfg_TickTaskStkBasePtr = OSCfg_TickTaskStk;
	STK_Temp = STK_Temp + OS_CFG_TICK_TASK_STK_SIZE;

/*****************定时器任务*******************************/
#if (OS_CFG_TMR_EN > 0u)
	OSCfg_TmrTaskStk = STK_Temp;
    OSCfg_TmrTaskStkBasePtr = OSCfg_TmrTaskStk;
	STK_Temp = STK_Temp + OS_CFG_TMR_TASK_STK_SIZE;
#else
    OSCfg_TmrTaskStkBasePtr = (CPU_STK *)0; 
#endif
}
					       
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;
		
	Stack_Init();   /* 堆栈初始化 */
    OSInit(&err);   /* 初始化uC/OS-III */

	OSTaskCreate((OS_TCB       *)AppTaskStartTCB,         /* 创建启动任务 */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);         /* 开始多任务的执行 */
    
    (void)&err;
    
    return (0);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*             这里主要是实现LED2的闪烁，表示系统正在稳定的工作中。
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
#include "lwip_client.h"
static  void  AppTaskStart (void *p_arg)
{
	uint8_t  ucCount = 0;
	uint8_t  ucCountGT811 = 0;
	uint8_t  ucCountFT = 0;
	OS_ERR      err;
	
	(void)p_arg;
	CPU_Init();
 	bsp_Init();
	BSP_Tick_Init();                        

#if OS_CFG_STAT_TASK_EN > 0u
     OSStatTaskCPUUsageInit(&err);   
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    AppObjCreate();                                            
    AppTaskCreate(); 
    LwipClientConnect();
    while (1)
	{  
		/* 1ms一次触摸扫描，电阻触摸屏 */
		if(g_tTP.Enable == 1)
		{
			TOUCH_Scan();	
		}
		/* 10ms一次触摸扫描，电容触摸屏GT811 */
		else if(g_GT811.Enable == 1)
		{
			ucCountGT811++;
			if(ucCountGT811 == 10)
			{
				ucCountGT811 = 0;
				GT811_OnePiontScan();
			}
		}
		/* 10ms一次触摸扫描，电容触摸屏FT5X06 */
		else if(g_tFT5X06.Enable == 1)
		{
			ucCountFT++;
			if(ucCountFT == 10)
			{
				ucCountFT = 0;
				FT5X06_OnePiontScan();
			}
		}
		
		/* 10ms一次按键检测 */
		ucCount++;
		if(ucCount == 10)
		{
			ucCount = 0;
			bsp_KeyScan();
		}
		BSP_OS_TimeDlyMs(1);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUIUpdate
*	功能说明: 本函数主要用于实现截图和串口打印UCOS任务的执行情况，此任务主要实现截图功能，		
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/
static void AppTaskGUIUpdate(void *p_arg)
{
	OS_ERR      err;
	uint8_t		Pic_Name = 0;
	char buf[20];
	CPU_BOOLEAN SemFlag;

	(void)p_arg;
		  
	while(1)
	{	
		SemFlag = BSP_OS_SemWait(&SEM_SYNCH, 0);
		
		if(SemFlag == DEF_OK)
		{	
			sprintf(buf,"0:/PicSave/%d.bmp",Pic_Name);
			OSSchedLock(&err);
			
			/* 如果SD卡中没有PicSave文件，会进行创建 */
			result = f_mkdir("0:/PicSave");
			/* 创建截图 */
			result = f_open(&file,buf, FA_WRITE|FA_CREATE_ALWAYS);
			/* 向SD卡绘制BMP图片 */
			GUI_BMP_Serialize(_WriteByte2File, &file);
			
			/* 创建完成后关闭file */
		    result = f_close(&file);
			
			OSSchedUnlock(&err);
			Pic_Name++; 		
		}										  	 	       											  
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 用于USBHOST		
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/
static void AppTaskCOM(void *p_arg)
{	
   (void)p_arg;
	 
	while(1)
	{	
        if(g_ucState == USB_HOST)
        {
            if(HCD_IsDeviceConnected(&USB_OTG_Core))
            {
              USBH_Process(&USB_OTG_Core, &USB_Host);
            }
        }
		BSP_OS_TimeDlyMs(10);	
	}	 						  	 	       											    
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 此函数主要用于得到按键的键值。
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
#if APP_CFG_UIP_ENABLED
extern void uip_pro(void);
#endif
extern void lwip_pro(void);
WM_HWIN    _hLastFrame;
static void AppTaskUserIF(void *p_arg)
{
	uint8_t uKeyCode;
	OS_ERR  err;
	uint8_t ucCalibrate = 0;
	
	(void)p_arg;	/* 避免编译器报警 */
	
	while (1) 
	{   		
		uKeyCode = bsp_GetKey();
		/* 有键按下 */
		if(uKeyCode != KEY_NONE)
		{
           
			switch (uKeyCode)
			{
			    case KEY_1_DOWN:		/* 按键K1 用于实现屏幕截图功能 */
				    BSP_OS_SemPost(&SEM_SYNCH);  	
					break;
				
				case KEY_2_DOWN:		/* 按键K2 用于进入触摸校准界面 */
				    ucCalibrate = 1;
					OSTaskQPost(AppTaskGUITCB,
								(void *)&ucCalibrate,
								1,
								OS_OPT_POST_FIFO,
								&err);	
					break;
				
                case KEY_3_DOWN:		/* 按键K3 用于退出摄像头子界面 */
				    // BSP_OS_SemPost(&SEM_OV7670);
                    WM_SendMessageNoPara(_hLastFrame, WM_NOTIFY_PARENT);                
					break;
             
                case JOY_DOWN_U:		/* 摇杆上键按下 */
					
					break;

				case JOY_DOWN_D:		/* 摇杆下键按下 */
					
					break;

				case JOY_DOWN_L:		/* 摇杆LEFT键按下 */
					
					break;

				case JOY_DOWN_R:	    /* 摇杆RIGHT键按下 */
					
					break;

				case JOY_DOWN_OK:		/* 摇杆OK键 */
					break;
			}		
		}
		/************************uip和LWIP的刷新******************************************/
		lwip_pro();
        #if APP_CFG_UIP_ENABLED
		uip_pro();
        #endif
        BSP_OS_TimeDlyMs(5);	     
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUI
*	功能说明: GUI任务			  			  
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级：OS_CFG_PRIO_MAX - 3u
*********************************************************************************************************
*/
static void AppTaskGUI(void *p_arg)
{
    (void)p_arg;		/* 避免编译器告警 */
		
	while (1) 
	{
		MainTask();					 
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUIRefresh
*	功能说明: emWin触摸	  			  
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级：OS_CFG_PRIO_MAX - 3u
*********************************************************************************************************
*/
static void AppTaskGUIRefresh(void *p_arg)
{
	(void)p_arg;		/* 避免编译器告警 */
	  	
	while (1) 
	{
		bsp_LedToggle(2);
        BSP_OS_TimeDlyMs(100); 
    }						 
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	OS_ERR      err;
	
	/**************创建UPDATE任务*********************/
	OSTaskCreate((OS_TCB       *)AppTaskUpdateTCB,             
                 (CPU_CHAR     *)"App Task Update",
                 (OS_TASK_PTR   )AppTaskGUIUpdate, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_UPDATE_PRIO,
                 (CPU_STK      *)&AppTaskUpdateStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_UPDATE_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_UPDATE_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

	/**************创建COM任务*********************/
	OSTaskCreate((OS_TCB       *)AppTaskCOMTCB,            
                 (CPU_CHAR     *)"App Task COM",
                 (OS_TASK_PTR   )AppTaskCOM, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_COM_PRIO,
                 (CPU_STK      *)&AppTaskCOMStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_COM_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_COM_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/**************创建USER IF任务*********************/
	OSTaskCreate((OS_TCB       *)AppTaskUserIFTCB,             
                 (CPU_CHAR     *)"App Task UserIF",
                 (OS_TASK_PTR   )AppTaskUserIF, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_USER_IF_PRIO,
                 (CPU_STK      *)&AppTaskUserIFStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/**************创建GUI任务*********************/			 
	OSTaskCreate((OS_TCB       *)AppTaskGUITCB,              
                 (CPU_CHAR     *)"App Task GUI",
                 (OS_TASK_PTR   )AppTaskGUI, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_GUI_PRIO,
                 (CPU_STK      *)&AppTaskGUIStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_GUI_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_GUI_STK_SIZE,
                 (OS_MSG_QTY    )1,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/**************创建GUI刷新任务*********************/			 
	OSTaskCreate((OS_TCB       *)AppTaskGUIRefreshTCB,              
                 (CPU_CHAR     *)"App Task GUIRefresh",
                 (OS_TASK_PTR   )AppTaskGUIRefresh, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_GUIRefresh_PRIO,
                 (CPU_STK      *)&AppTaskGUIRefreshStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_GUIRefresh_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_GUIRefresh_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);			 
}

/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create application kernel objects tasks.
* Argument(s) : none
* Return(s)   : none
* Caller(s)   : AppTaskStart()
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{

	/* 创建同步信号量 */ 
   	BSP_OS_SemCreate(&SEM_OV7670,
					 0,	
					 (CPU_CHAR *)"SEM_OV7670");

	/* 创建同步信号量 */ 
   	BSP_OS_SemCreate(&SEM_SYNCH,
					 0,	
					 (CPU_CHAR *)"SEM_SYNCH");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

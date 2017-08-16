/*
*********************************************************************************************************
*
*	模块名称 : 中断模块
*	文件名称 : stm32f4xxx_it.c
*	版    本 : V1.1
*	说    明 : 本文件存放中断服务函数。
*
*			我们只需要添加需要的中断函数即可。一般中断函数名是固定的，除非您修改了启动文件中的函数名
*				\Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates\arm\startup_stm32f4xx.s
*
*			启动文件是汇编语言文件，定了每个中断的服务函数，这些函数使用了WEAK 关键字，表示弱定义，因此如
*			果我们在c文件中重定义了该服务函数（必须和它同名），那么启动文件的中断函数将自动无效。这也就
*			函数重定义的概念。
*
*			为了加强模块化，我们建议将中断服务程序分散到对应的驱动模块文件中。比如systick中断服务程序
*			放在 bsp_timer.c 文件中。
*
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2013-02-01 armfly  首版
*		v1.0    2013-06-20 armfly  在硬件异常中断服务程序中增加打印错误信息的功能。
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f4xx_it.h"
#include  <includes.h>
#include  "MainTask.h"


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  printf("HardFault_Handler\r\n");
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/*
*********************************************************************************************************
*	函 数 名: SDIO_IRQHandler
*	功能说明: This function handles WWDG interrupt request.
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SDIO_IRQHandler(void)
{
  	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();

	SD_ProcessIRQSrc();

	/* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
	OSIntExit();
}

/*
*********************************************************************************************************
*	函 数 名: SD_SDIO_DMA_IRQHANDLER
*	功能说明: This function handles WWDG interrupt request.
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SD_SDIO_DMA_IRQHANDLER(void)
{
  	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();

	SD_ProcessDMAIRQ();

	/* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
	OSIntExit();
}

/*
*********************************************************************************************************
*	函 数 名: SPI2_IRQHandler
*	功能说明: I2S数据传输中断服务程序。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void I2S_CODEC_DataTransfer(void);
void SPI2_IRQHandler(void)
{
	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();

    I2S_CODEC_DataTransfer();

    /* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
	OSIntExit();
}

/*
*********************************************************************************************************
*	函 数 名: RTC_Alarm_IRQHandler
*	功能说明: 闹钟中断。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RTC_Alarm_IRQHandler(void)
{
	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();

	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	} 
	
	/* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
	OSIntExit();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

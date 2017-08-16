/*
*********************************************************************************************************
*
*	ģ������ : �ж�ģ��
*	�ļ����� : stm32f4xxx_it.c
*	��    �� : V1.1
*	˵    �� : ���ļ�����жϷ�������
*
*			����ֻ��Ҫ�����Ҫ���жϺ������ɡ�һ���жϺ������ǹ̶��ģ��������޸��������ļ��еĺ�����
*				\Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates\arm\startup_stm32f4xx.s
*
*			�����ļ��ǻ�������ļ�������ÿ���жϵķ���������Щ����ʹ����WEAK �ؼ��֣���ʾ�����壬�����
*			��������c�ļ����ض����˸÷��������������ͬ��������ô�����ļ����жϺ������Զ���Ч����Ҳ��
*			�����ض���ĸ��
*
*			Ϊ�˼�ǿģ�黯�����ǽ��齫�жϷ�������ɢ����Ӧ������ģ���ļ��С�����systick�жϷ������
*			���� bsp_timer.c �ļ��С�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2013-02-01 armfly  �װ�
*		v1.0    2013-06-20 armfly  ��Ӳ���쳣�жϷ�����������Ӵ�ӡ������Ϣ�Ĺ��ܡ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
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
*	�� �� ��: SDIO_IRQHandler
*	����˵��: This function handles WWDG interrupt request.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SDIO_IRQHandler(void)
{
  	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();

	SD_ProcessIRQSrc();

	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/*
*********************************************************************************************************
*	�� �� ��: SD_SDIO_DMA_IRQHANDLER
*	����˵��: This function handles WWDG interrupt request.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SD_SDIO_DMA_IRQHANDLER(void)
{
  	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();

	SD_ProcessDMAIRQ();

	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/*
*********************************************************************************************************
*	�� �� ��: SPI2_IRQHandler
*	����˵��: I2S���ݴ����жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
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

    /* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_Alarm_IRQHandler
*	����˵��: �����жϡ�
*	��    �Σ���
*	�� �� ֵ: ��
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
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

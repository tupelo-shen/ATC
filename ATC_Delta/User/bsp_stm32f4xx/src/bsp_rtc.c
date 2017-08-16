/*
*********************************************************************************************************
*
*	ģ������ : RTC
*	�ļ����� : bsp_rtc.c
*	��    �� : V1.0
*	˵    �� : RTC�ײ�����
*	�޸ļ�¼ :
*		�汾��   ����        ����     ˵��
*		V1.0    2013-12-11  armfly   ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


//#define RTC_Debug   /* ����ѡ�����ģʽ����ʽʹ�ÿ��Խ���ע�͵� */

/* ѡ��RTC��ʱ��Դ */
#define RTC_CLOCK_SOURCE_LSE       /* LSE */
//#define RTC_CLOCK_SOURCE_LSI     /* LSI */ 

RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef  RTC_DateStructure;

__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
uint8_t aShowTime[50] = {0};
/* 
*********************************************************************************************************
*  �� �� ��: bsp_InitRTC 
*  ����˵��: ��ʼ��RTC 
*  �� �Σ���  *  �� �� ֵ: ��   
*********************************************************************************************************
*/ 
void bsp_InitRTC(void) 
{ 
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* ���ڼ���Ƿ��Ѿ����ù�RTC��������ù��Ļ����������ý���ʱ 
	����RTC���ݼĴ���Ϊ0x32F2��������RTC���ݼĴ�������0x32F2   ��ô��ʾû�����ù�����Ҫ����RTC.   */ 
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2) 
	{ 
		/* RTC ���� */ 
		RTC_Config(); 
		/* ��ӡ������Ϣ */ 
		#ifdef RTC_Debug 
			printf("��һ��ʹ��RTC \n\r"); 
		#endif  
		/* ����ϵ縴λ��־�Ƿ����� */ 
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) 
		{ 
			/* �����ϵ縴λ */ 
			#ifdef RTC_Debug 
				printf("�����ϵ縴λ \n\r"); 
			#endif 
		} 
	} 
	else 
	{ 
		/* ��ӡ������Ϣ */ 
		#ifdef RTC_Debug 
			printf("��n��ʹ��RTC \n\r"); 
		#endif 
		/* ����ϵ縴λ��־�Ƿ����� */ 
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) 
		{ 
			/* �����ϵ縴λ */ 
			#ifdef RTC_Debug 
				printf("�����ϵ縴λ \n\r"); 
			#endif 
		} 
		/* ������Ÿ�λ��־�Ƿ����� */ 
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) 
		{ 
			/* �������Ÿ�λ */ 
			#ifdef RTC_Debug 
				printf("�������Ÿ�λ \n\r");     
			#endif    
		} 
		
		/* ʹ��PWRʱ�� */ 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
		/* �������RTC */ 
		PWR_BackupAccessCmd(ENABLE); 
		/* �ȴ� RTC APB �Ĵ���ͬ�� */ 
		RTC_WaitForSynchro(); 
		/* ���RTC���ӱ�־ */ 
		RTC_ClearFlag(RTC_FLAG_ALRAF); 
		/* ���RTC�����жϹ����־ */ 
		EXTI_ClearITPendingBit(EXTI_Line17); 
	} 
	
	/* RTC�����ж����� */ 
	/* EXTI ���� */ 
	EXTI_ClearITPendingBit(EXTI_Line17); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line17; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure); 
	
	/* ʹ��RTC�����ж� */ 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_Config
*	����˵��: 1. ѡ��ͬ��RTCʱ��ԴLSI����LSE��
*             2. ����RTCʱ�ӡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_Config(void)
{
	/* ʹ��PWRʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* �������RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* ѡ��LSI��Ϊʱ��Դ */
#if defined (RTC_CLOCK_SOURCE_LSI)  
	
	/* Enable the LSI OSC */ 
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* ѡ��RTCʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;
	
	/* ѡ��LSE��ΪRTCʱ�� */
#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* ʹ��LSE����  */
	RCC_LSEConfig(RCC_LSE_ON);

	/* �ȴ����� */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* ѡ��RTCʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;

#else
#error Please select the RTC Clock source inside the main.c file
#endif 

	/* ʹ��RTCʱ�� */
	RCC_RTCCLKCmd(ENABLE);

	/* �ȴ�RTC APB�Ĵ���ͬ�� */
	RTC_WaitForSynchro();

	/* ����RTC���ݼĴ����ͷ�Ƶ��  */
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* ���������պ����� */
	RTC_DateStructure.RTC_Year = 0x13;
	RTC_DateStructure.RTC_Month = RTC_Month_January;
	RTC_DateStructure.RTC_Date = 0x11;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	/* ����ʱ���룬�Լ���ʾ��ʽ */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x05;
	RTC_TimeStructure.RTC_Minutes = 0x20;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

	/* ���ñ��ݼĴ�������ʾ�Ѿ����ù�RTC */
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);

	/* �������� BCD��ʽ��05Сʱ��20���ӣ�30�� */
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x05;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x20;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x30;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	/* ���� RTC Alarm A �Ĵ��� */
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	/* ʹ�� RTC Alarm A �ж� */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* ʹ������ */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	/* ���RTC���ӱ�־ */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_TimeShow
*	����˵��: ��ʾʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_TimeShow(void)
{
	/* �õ���ǰʱ����ʱ�� */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	/* ʱ����ʾ��ʽ : hh:mm:ss */
	sprintf((char*)aShowTime,"%0.2d:%0.2d:%0.2d",RTC_TimeStructure.RTC_Hours, 
	                                           RTC_TimeStructure.RTC_Minutes, 
	                                           RTC_TimeStructure.RTC_Seconds);
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_DateShow
*	����˵��: ��ʾʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_DateShow(void)
{
	/* �õ���ǰʱ����ʱ�� */
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	/* ʱ����ʾ��ʽ : hh:mm:ss */
	sprintf((char*)aShowTime,"��%0.2d��%0.2d��%0.2d����%0.1d",RTC_DateStructure.RTC_Year, 
	                                                         RTC_DateStructure.RTC_Month, 
	                                                          RTC_DateStructure.RTC_Date,
											               RTC_DateStructure.RTC_WeekDay);
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_AlarmShow
*	����˵��: ��ʾ���õ�����ʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_AlarmShow(void)
{	
	uint8_t showalarm[50] = {0};

	/* ���õ�ǰ���� */
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	sprintf((char*)showalarm,"%0.2d:%0.2d:%0.2d", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, 
												RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, 
												RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_AlarmShow
*	����˵��: ��ʾ���õ�����ʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_TimeRegulate(void)
{
	uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

	printf("\n\r===================ʱ������=======================\n\r");
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	
	/*******************************************************/
	
	printf("  ������Сʱ:\n\r");
	while (tmp_hh == 0xFF)
	{
		tmp_hh = USART_Scanf(23);
		RTC_TimeStructure.RTC_Hours = tmp_hh;
	}
	printf("  %0.2d\n\r", tmp_hh);
	
    /*******************************************************/
	
	printf("  �����÷���:\n\r");
	while (tmp_mm == 0xFF)
	{
	tmp_mm = USART_Scanf(59);
	RTC_TimeStructure.RTC_Minutes = tmp_mm;
	}
	printf("  %0.2d\n\r", tmp_mm);

	/*******************************************************/
	
	printf("  ��������:\n\r");
	while (tmp_ss == 0xFF)
	{
	tmp_ss = USART_Scanf(59);
	RTC_TimeStructure.RTC_Seconds = tmp_ss;
	}
	printf("  %0.2d\n\r", tmp_ss);
	
	/*******************************************************/

	/* ����RTCʱ��Ĵ��� */
	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
	{
		printf("\n\r>> !! RTC ʱ������ʧ�� !! <<\n\r");
	} 
	else
	{
		printf("\n\r>> !! RTC ʱ�����óɹ� !! <<\n\r");
		RTC_TimeShow();
		/* ����RTC���ݼĴ�������˵���Ѿ����ù� */
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
	}
	
	/*******************************************************/

	tmp_hh = 0xFF;
	tmp_mm = 0xFF;
	tmp_ss = 0xFF;

	/* ���� Alarm A */
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	printf("\n\r==============Alarm A ����=========================\n\r");
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	
	/*******************************************************/
	
	printf("  ����������Сʱ:\n\r");
	while (tmp_hh == 0xFF)
	{
	tmp_hh = USART_Scanf(23);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = tmp_hh;
	}
	printf("  %0.2d\n\r", tmp_hh);

	/*******************************************************/
	
	printf("  ���������ӷ���:\n\r");
	while (tmp_mm == 0xFF)
	{
	tmp_mm = USART_Scanf(59);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = tmp_mm;
	}
	printf("  %0.2d\n\r", tmp_mm);
	
	/*******************************************************/

	printf("  ������������:\n\r");
	while (tmp_ss == 0xFF)
	{
	tmp_ss = USART_Scanf(59);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = tmp_ss;
	}
	printf("  %0.2d", tmp_ss);
	
	/*******************************************************/
	
	/* ���� Alarm A */
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	/* ���� RTC Alarm A �Ĵ��� */
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	printf("\n\r>> !! RTC ���� Alarm �ɹ�. !! <<\n\r");

	/* ʹ�� RTC Alarm A �ж� */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* ʹ�� alarm  A */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	
}

/*
*********************************************************************************************************
*	�� �� ��: USART_Scanf
*	����˵��: ���մ�������
*	��    �Σ�value ��ʾ�����������ֵ
*	�� �� ֵ: ���ش��ڽ��յ�����ֵ
*********************************************************************************************************
*/
uint8_t USART_Scanf(uint32_t value)
{
	uint32_t index = 0;
	uint32_t tmp[2] = {0, 0};

	while (index < 2)
	{
		/* �ȴ�ֱ�� RXNE = 1 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
		{}
		tmp[index++] = (USART_ReceiveData(USART1));
		if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
		{
			printf("\n\r ��Ч���ݱ����� 0 �� 9 ֱ��\n\r");
			index--;
		}
	}
	
	/* ���������ֽںϲ������ֵ */
	index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
	/* Checks */
	if (index > value)
	{
		printf("\n\r ��Ч���ݱ����� 0 �� %d ֱ��\n\r", value);
		return 0xFF;
	}
	
	return index;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


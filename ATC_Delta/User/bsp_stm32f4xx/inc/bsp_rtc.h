/*
*********************************************************************************************************
*
*	ģ������ : RTC
*	�ļ����� : bsp_rtc.c
*	��    �� : V1.0
*	˵    �� : RTC�ײ�����
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_RTC_H
#define __BSP_RTC_H

void RTC_TimeShow(void);
void RTC_AlarmShow(void);
void RTC_TimeRegulate(void);
uint8_t USART_Scanf(uint32_t value);
void RTC_DateShow(void);
void bsp_InitRTC(void);
void RTC_Config(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


/*
*********************************************************************************************************
*
*	ģ������ : DAC����ģ��
*	�ļ����� : bsp_dac.c
*	��    �� : V1.0
*	˵    �� : ʵ��DAC�������
*	�޸ļ�¼ :
*		�汾��    ����        ����     ˵��
*		V1.0    2015-01-05   Eric2013   ��ʽ����
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f4xx.h"

void bsp_InitADC(void);
void TIM8_MeasureTrigConfig(void);

extern uint16_t count;
extern void ADC_DMA_Open(void);
extern void ADC_DMA_Close(void);
extern uint16_t ADC1ConvertedValue[10240];
extern uint16_t ADC2ConvertedValue[10240];
extern uint16_t ADC3ConvertedValue[10240];

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

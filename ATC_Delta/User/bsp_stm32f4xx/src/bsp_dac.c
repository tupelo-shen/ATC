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
#include "bsp.h"


#define DAC_DHR12R1_ADDRESS    0x40007408  
uint16_t g_usWaveBuff[64];     /* ���λ��� */
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDAC
*	����˵��: DAC��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDAC(void)
{   
    uint8_t i;

	/* �û����һ�����ڵķ��� */
	for(i =0; i < 32; i++)
	{
		g_usWaveBuff[i] = 0;
	}
	
	for(i =0; i < 32; i++)
	{
		g_usWaveBuff[i+32] = 2048;
	}
	
	/* MDK�Ż��ȼ�Ϊ1��ʱ��DAC��ʼ��˳�򣬲�ͬ���Ż��ȼ���
	   ��ͬ�����˳�����ײ���DAC�޷�������Ρ�
	   �������Ҫ�����ҵ��ر�ע�⣡����
	*/
	DAC_Ch1_WaveConfig();
	DAC_GPIOConfig(); 
    TIM6_Config(); 
}

/*
*********************************************************************************************************
*	�� �� ��: TIM6_Config
*	����˵��: ���ö�ʱ��6�����ڴ���DAC��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM6_Config(void)
{
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    
     /* ʹ��TIM6ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    /* --------------------------------------------------------
    system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

    HCLK = SYSCLK / 1     (AHB1Periph)
    PCLK2 = HCLK / 2      (APB2Periph)
    PCLK1 = HCLK / 4      (APB1Periph)

    ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
          
    TIM6 ���������� = TIM6CLK / ��TIM_Period + 1��/��TIM_Prescaler + 1��
	�������µ����ã����������ǣ�
	TIM6CLK / ��TIM_Period + 1��/��TIM_Prescaler + 1��
	= 84MHz /��55+1��/��0+1��
	= 1.5MHz
    ----------------------------------------------------------- */
    
    /* ����TIM6 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 55;          
    TIM_TimeBaseStructure.TIM_Prescaler = 0;       
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* TIM6 TRGO ѡ�� */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* ʹ��TIM6 */
    TIM_Cmd(TIM6, ENABLE);   
}

/*
*********************************************************************************************************
*	�� �� ��: DAC_GPIOConfig
*	����˵��: DACͨ��һ��PA4��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DAC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    /*  ʹ������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    /* DACͨ��1����������ã�PA.4�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
}

/*
*********************************************************************************************************
*	�� �� ��: DAC_Ch1_WaveConfig
*	����˵��: DACͨ��1�������
*             ���溯��TIM6_Config�ѽ���TIM6��������ó�1.5MHz������DMA�Ļ����С��64,��ô���������Ƶ�ʾ���
*             �������� = TIM6��������/64 = 1.5MHz / 64 = 23437.5Hz
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DAC_Ch1_WaveConfig(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
      
    /* DAC ����ʱ��ʹ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* DMA1 ʱ��ʹ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
     
    /* DACͨ��1���� */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    
    /* DMA1_Stream5 ͨ��7���� **************************************/  
    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&g_usWaveBuff;
    DMA_InitStructure.DMA_BufferSize = 64;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);    

    /* ʹ�� DMA1_Stream5 */
    DMA_Cmd(DMA1_Stream5, ENABLE);

    /* ʹ��DACͨ�� */
    DAC_Cmd(DAC_Channel_1, ENABLE);

    /* ʹ��DAC��DMAͨ�� */
    DAC_DMACmd(DAC_Channel_1, ENABLE);    
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

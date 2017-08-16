/*
*********************************************************************************************************
*
*	ģ������ : ADC����ģ��
*	�ļ����� : bsp_adc.c
*	��    �� : V1.0
*	˵    �� : ʵ��3ADC���ٽ������
*	�޸ļ�¼ :
*		�汾��    ����        ����       ˵��
*		V1.0    2015-01-05   Eric2013   ��ʽ����
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


/*
*********************************************************************************************************
*	                                 ADC�Ĵ���
*********************************************************************************************************
*/
#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)
#define ADC3_DR_ADDRESS    ((uint32_t)0x4001224C)
#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)
#define ADC2_DR_ADDRESS    ((uint32_t)0x4001214C)

/*
*********************************************************************************************************
*	                           ADC1��ADC2��ADC3�����ݻ��棬��С��Ϊ10240
*********************************************************************************************************
*/
uint16_t ADC1ConvertedValue[10240];
uint16_t ADC2ConvertedValue[10240];
uint16_t ADC3ConvertedValue[10240];

/*
*********************************************************************************************************
*	                                   �����ͺ���                           
*********************************************************************************************************
*/
uint16_t g_usFirstTimeIRQFlag;  /* ��һ�ν��붨ʱ��8���жϱ�־ */
uint16_t g_usTrigCount = 0;     /* ��¼��ͨ�������뿴�Ź��ж�ʱ��DMA�����λ�� */
uint16_t g_usTrigTempFlag = 0;  /* ��ʱ����������ADC�ж� */
void Time8Recorder(void);

/*
*********************************************************************************************************
*	                                  �ⲿ����������
*********************************************************************************************************
*/
extern const uint32_t g_SampleFreqTable[][2];
extern int8_t  TimeBaseId;
extern uint16_t TriggerFlag;
extern uint16_t g_usTrigValue;
extern uint16_t g_usWaveBuf1[1024*6];

/*
*********************************************************************************************************
*	�� �� ��: TIM1_Config
*	����˵��: ���ö�ʱ��1�����ڴ���ADC1��ADC2��ADC3��
*             ���ⲿ�����źű�ѡΪADC�����ע��ת��ʱ��ֻ�����������ؿ�������ת����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TIM1_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
	/* ʹ�ܶ�ʱ��1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	
    
	/* �Ƚ��������� */
    TIM_Cmd(TIM1, DISABLE);	
	
    /*
     ********************************************************************************
    system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

    HCLK = SYSCLK / 1     (AHB1Periph)
    PCLK2 = HCLK / 2      (APB2Periph)
    PCLK1 = HCLK / 4      (APB1Periph)

    ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
          
    TIM1 ���������� = TIM1CLK / ��TIM_Period + 1��/��TIM_Prescaler + 1��
    ********************************************************************************
    */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 	                                   //��ʼ����ʱ��1�ļĴ���Ϊ��λֵ
    TIM_TimeBaseStructure.TIM_Period =  168000000/g_SampleFreqTable[TimeBaseId][0] - 1;    //ARR�Զ���װ�ؼĴ������ڵ�ֵ(��ʱʱ�䣩������Ƶ�ʺ���������»����ж�(Ҳ��˵��ʱʱ�䵽)
    TIM_TimeBaseStructure.TIM_Prescaler = g_SampleFreqTable[TimeBaseId][1]-1;   		   //PSCʱ��Ԥ��Ƶ�� ���磺ʱ��Ƶ��=TIM1CLK/(ʱ��Ԥ��Ƶ+1)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    	            //CR1->CKDʱ��ָ�ֵ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     //CR1->CMS[1:0]��DIR��ʱ��ģʽ ���ϼ���
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /**************ADC1�Ĵ���***********************************************/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				   //CCMR2�����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;      //CCER ���ʹ��          
    TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period/3;//CCR3ͬ������TIMx_CNT�ıȽϣ�����OC1�˿��ϲ�������ź� 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    	   //CCER�����������
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	 /**************ADC2�Ĵ���***********************************************/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;     
    TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period*2/3;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    	  
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	
	/**************ADC3�Ĵ���***********************************************/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;     
    TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period-1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    	  
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    //TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);		//CMR2 ����Ԥװ��ʹ��  �����¼�����ʱд����Ч
    //TIM_ARRPreloadConfig(TIM1, ENABLE);		   		 	//CR1  ����ARR�Զ���װ �����¼�����ʱд����Ч
    TIM_Cmd(TIM1, ENABLE);	
	
    /* ʹ��PWM��� */	
    TIM_CtrlPWMOutputs(TIM1, ENABLE);  		
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitADC
*	����˵��: ADC��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitADC(void)
{  
	ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
    /* ����ģ�⿴�Ź��ж�NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    /* ʹ�� ADC1, ADC2, DMA2 �� GPIO ʱ�� ****************************************/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | 
						   RCC_APB2Periph_ADC3, ENABLE);
	
	/* DMA2 Stream1 channel1 ��������ADC3 **************************************/
    DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024*10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream1, ENABLE);
	
    /* DMA2 Stream2 channel1 ��������ADC2 **************************************/
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC2_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC2ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024*10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream2, ENABLE);
	
	/* DMA2 Stream0 channel0 ��������ADC1 **************************************/
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024*10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* ����ADC����Ϊģ������ģʽ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*
	***************************************************************************   
	  PCLK2 = HCLK / 2 
	  ����ѡ�����2��Ƶ
	  ADCCLK = PCLK2 /2 = HCLK / 4 = 168 / 4 = 42M
      ADC����Ƶ�ʣ� Sampling Time + Conversion Time = 3 + 12 cycles = 15cyc
                    Conversion Time = 42MHz / 15cyc = 2.8Mbps.
	****************************************************************************
	*/
    
    /* ADC�������ֳ�ʼ��**********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_8Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInit(&ADC_CommonInitStructure); 
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////

	 /*ADC3������*****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
    
    /* ADC3 ����ͨ������ */
	ADC_Init(ADC3, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

    /* ʹ�� ADC3 DMA */
	ADC_DMACmd(ADC3, ENABLE);
	
	/* ����ģ�⿴�Ź��ķ�ֵ ע������÷��ˣ�Ҫ��һֱ�����ж� */
    ADC_AnalogWatchdogThresholdsConfig(ADC3, 4095, 0);
    
    /* ����ģ�⿴�Ź����ADC3��ͨ��10 */
    ADC_AnalogWatchdogSingleChannelConfig(ADC3, ADC_Channel_10);
    
    /* ʹ��һ������ͨ���Ŀ��Ź� */
    ADC_AnalogWatchdogCmd(ADC3, ADC_AnalogWatchdog_SingleRegEnable);

    /* ʹ��ģ�⿴�Ź��ж� */
    ADC_ITConfig(ADC3, ADC_IT_AWD, ENABLE);
    
	/* ʹ��DMA���� (��ADCģʽ) --------------------------------------------------*/
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

	/* Enable ADC1 --------------------------------------------------------------*/
	ADC_Cmd(ADC3, ENABLE);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
    /*ADC2������*****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC2;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
    
    /* ADC2 ����ͨ������ */
	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

    /* ʹ�� ADC2 DMA */
	ADC_DMACmd(ADC2, ENABLE);
    
	/* ʹ��DMA���� (��ADCģʽ) */
	ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);

	/* ʹ�� ADC2 */
	ADC_Cmd(ADC2, ENABLE);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*ADC1������******************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
    
    /* ADC1 ����ͨ������ -------------------------------------------------------*/
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

    /* ʹ�� ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* ʹ��DMA���� (��ADCģʽ) --------------------------------------------------*/
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 --------------------------------------------------------------*/
	ADC_Cmd(ADC1, ENABLE);

	/**��ʱ������********************************************************************/
	TIM1_Config();
}

/*
*********************************************************************************************************
*	�� �� ��: DMA_Open
*	����˵��: ʹ��ADC1��ADC2��ADC3��DMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ADC_DMA_Open(void)
{
    DMA_InitTypeDef       DMA_InitStructure;

	/* DMA2 Stream1 channel1 ��������ADC3 **************************************/
    DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024*10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream1, ENABLE);
	
	/* DMA2 Stream2 channel1 ��������ADC2 **************************************/
    DMA_DeInit(DMA2_Stream2);  /* ��DMA��DMA_Mode_Normalģʽ��һ��Ҫʹ�����������ѭ��ģʽ���Բ��� */
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC2_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC2ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024*10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream2, ENABLE);
	
	/* DMA2 Stream0 channel0 ��������ADC1 **************************************/
    DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1024*10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

	/* ʹ�� ADC3 --------------------------------------------------------------*/
	ADC_Cmd(ADC3, ENABLE);
	
	ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

	/* ʹ�� ADC2 --------------------------------------------------------------*/
	ADC_Cmd(ADC2, ENABLE);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

	/* ʹ�� ADC1 --------------------------------------------------------------*/
	ADC_Cmd(ADC1, ENABLE);

	/**��ʱ������,һ��Ҫ���µĳ�ʼ���Ӷ���֤ͬʱ���� **/
	TIM1_Config();

	/* ֻ������ͨ����ģʽ������TIM8�ļ�ʱ���� */
	if(TriggerFlag != 0)
	{
		Time8Recorder();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DMA_Close
*	����˵��: �ر�ADC1��ADC2��ADC3����DMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ADC_DMA_Close()
{
	TIM_Cmd(TIM1, DISABLE);
	
	DMA_Cmd(DMA2_Stream1, DISABLE);
	
	DMA_Cmd(DMA2_Stream0, DISABLE);
	
	DMA_Cmd(DMA2_Stream2, DISABLE);
	
	/* ��ֹ ADC1 ---------------------------------------------------------------------*/
	ADC_Cmd(ADC1, DISABLE);

	/* ��ֹ ADC2 ---------------------------------------------------------------------*/
	ADC_Cmd(ADC2, DISABLE);
	
	/* ��ֹ ADC3 ---------------------------------------------------------------------*/
	ADC_Cmd(ADC3, DISABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: ADC_IRQHandler
*	����˵��: ģ�⿴�Ź��жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ADC_IRQHandler()
{ 
	/* ��ȡDMAʣ��Ҫ�������Ŀ */
	g_usTrigCount = DMA2_Stream1->NDTR;

	/* ȷ���Ƿ���ADC3�Ŀ��Ź��ж� */
	if((ADC3->SR)&0x01)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    {
		/* ȡ����ֵ��ǰһ���㣬�鿴�Ƿ����ڷ�ֵ��Χ�ڣ��Ӷ��ж��������ػ����½��� */
		if(g_usTrigCount == 10240)
		{
			/* ʣ��10240��ʾ����ֵ��ADC3ConvertedValue[10239]�Ǵ���ֵ����ô��һ�������10238 */
			g_usTrigTempFlag = ADC3ConvertedValue[10238]; 
		}
		else if(g_usTrigCount == 10239)
		{
			/* ʣ��10239��ʾ����ֵ��ADC3ConvertedValue[0]�Ǵ���ֵ����ô��һ�������10239 */
			g_usTrigTempFlag = ADC3ConvertedValue[10239]; 
		}
		else
		{
			/* ʣ��10239��ʾ����ֵ��ADC3ConvertedValue[0]�Ǵ���ֵ����ô��һ�������10239 */
			g_usTrigTempFlag = ADC3ConvertedValue[10238 - g_usTrigCount]; 
		}
		
		/* �ж��Ƿ��������أ��ǵĻ�������ʱ����¼ADC���� */
		if(g_usTrigTempFlag <= g_usTrigValue)
		{
			/* �ر�ADC3�Ŀ��Ź��ж� */
			ADC3->CR1 &= 0xffffffbf;
			TriggerFlag = 1;
			
			/* ������ʱ������ */
			TIM8->CR1 |= 0x01;
		}
        /* ��������־ */
		ADC3->SR &= 0xfe;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: TIM8_MeasureTrigConfig
*	����˵��: ʹ��TIM8Ϊ��ͨ����ģʽ�����ݲɼ���ʱ����ʱ�ɼ�����ֵǰ���1024��ADC����
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
/* 
	ÿ�β���ɼ�����ֵǰ���1024��ADC����(��ͨ��)����ͨ���������1024*3��ADC���ݡ�
*/
const uint32_t g_TrigFreqTable[][2] =
{
	{60,   1024},    //2.8Msps  168000000/2800000 = 60  => 60 * 1024 
	{84,   1024},    //2Msps    168000000/2000000 = 84  => 84 * 1024 
	{168,  1024},    //1Msps    168000000/1000000 = 168 => 168 * 1024
	{336,  1024},    //500Ksps  168000000/500000  = 336 => 336 * 1024
	{840,  1024},    //200Ksps  168000000/200000  = 840 => 840 * 1024
	
	{1680,  1024},   //100Ksps 168000000/100000  = 1680  => 1680 * 1024
	{3360,  1024},   //50Ksps  168000000/50000   = 3360  => 3360 * 1024 
	{8400,  1024},   //20Ksps  168000000/20000   = 8400  => 8400 * 1024
	{16800, 1024},   //10Ksps  168000000/10000   = 16800 => 16800 * 1024 
	{33600, 1024},   //5Ksps   168000000/5000    = 33600 => 33600 * 1024 

	/* ����5�ֲ���Ƶ����ˢ�½�������Ϊ�ɼ�ǰ��1024��ADC��ʱ��ϳ� */
	{42000,    2048},    //2Ksps 168000000/2000  = 84000 => 84000 * 1024
	{42000,    4096},    //1Ksps 168000000/1000  = 168000 => 168000 * 1024 
	{42000,    8192},    //500sps 168000000/500  = 336000 => 336000 * 1024 
	{42000,    20480},   //200sps 168000000/200  = 840000 => 840000 * 1024 
	{42000,    40960},   //100sps 168000000/100  = 1680000 => 1680000 * 1024
	
	/* �����⼸�ֲ����ʲ�������֧�� */
	{42000,    40960},   //50sps 
	{42000,    40960},   //20sps
	{42000,    40960},   //10sps 
	{42000,    40960},   //5sps 
	{42000,    40960},   //2sps 
	{42000,    40960},   //1sps 	
};

void TIM8_MeasureTrigConfig(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* ����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	/* ���ϵ�һ�ν����жϵı�־�������жϺ�����1 */
	g_usFirstTimeIRQFlag = 0;

	/* ʹ�ܶ�ʱ��8�ж�  */
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* ��ʱ������ */
	TimeBaseId = 1;   /* ��������ADC��ͨ��2Msps�������� */
	TIM_DeInit(TIM8);  
	TIM_BaseInitStructure.TIM_Period = g_TrigFreqTable[TimeBaseId][0] - 1;
	TIM_BaseInitStructure.TIM_Prescaler = g_TrigFreqTable[TimeBaseId][1] - 1;
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_BaseInitStructure); 
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM8, DISABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: Time8Recorder
*	����˵��: ʹ��TIM8Ϊ��ͨ����ģʽ�����ݲɼ���ʱ����ʱ�ɼ�����ֵǰ���1024��ADC����
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void Time8Recorder(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
	
	/* ���ϵ�һ�ν����жϵı�־ */
	g_usFirstTimeIRQFlag = 0;
	
	TIM_DeInit(TIM8);  
	TIM_BaseInitStructure.TIM_Period = g_TrigFreqTable[TimeBaseId][0] - 1;
	TIM_BaseInitStructure.TIM_Prescaler = g_TrigFreqTable[TimeBaseId][1] - 1;
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_BaseInitStructure); 
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM8, DISABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: TIM8_UP_TIM13_IRQHandler
*	����˵��: ��ʱ���жϺ���
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void TIM8_UP_TIM13_IRQHandler(void)
{
	uint16_t i,j,k,usCurPos;
	
	/* ��һ�ν���TIM8�ж�ִ���˳���������Ϊ��ʱ�������ж��и�Сbug��
	   һ����ʼ���˶�ʱ������ʹ���жϾͻ�����������һ���жϵ������Ϊ�˷�ֹ���������
	   ���֣������²�����
	*/
	if(g_usFirstTimeIRQFlag == 0)
	{
		g_usFirstTimeIRQFlag = 1;
		//TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		
		/* ����ǵ�һ�ν�����˳� */
		return;
	}
	
	/* ��ʱ��ʱ���ж� */
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		/* �ȹرն�ʱ�� */
		TIM_Cmd(TIM8, DISABLE);
	
		/* ȷ�������Ķ�ʱ����ʱ�ɼ�ADC���� */
		if(TriggerFlag == 1)
		{
			/* ��ʱ���ɼ�ADC���ݽ��� */
			TriggerFlag = 2;
			
			/* ������ʱ������ǰDMA�������ݵ�λ�� */
			usCurPos = 10240 - g_usTrigCount;
			
			/*
				�����Ϊ���������ȡ6K������(��ͨ������ͨ������ǰ��1024��ADC����)��
			     1. ��ǰ��λ�� < 1024.
			     2. 1024 <= ��ǰ��λ�� <= 10240 - 1024(9216)
				 3. ��ǰλ�� > 10240 - 1024(9216)
			*/
			/* ��һ���������ǰ��λ�� < 1024. */
			if(usCurPos < 1024)
			{
				k = 1024 - usCurPos;
				usCurPos = 10240 - k;
				
				/* ǰ�������� */
				for(i = 0; i < k; i++)
				{
					j = i*3;
					g_usWaveBuf1[j] = ADC1ConvertedValue[i+usCurPos];
					g_usWaveBuf1[j+1] = ADC2ConvertedValue[i+usCurPos];
					g_usWaveBuf1[j+2] = ADC3ConvertedValue[i+usCurPos];
				}
				
				usCurPos = i*3;
				k = 10240 - g_usTrigCount + 1024;
				
				/* �󲿷����� */
				for(i = 0; i < k; i++)
				{
					j = i*3;
					g_usWaveBuf1[j + usCurPos] = ADC1ConvertedValue[i];
					g_usWaveBuf1[j+1+usCurPos] = ADC2ConvertedValue[i];
					g_usWaveBuf1[j+2+usCurPos] = ADC3ConvertedValue[i];
				}
			}
			/* �������������ǰλ�� > 10240 - 1024(9216) */
			else if(usCurPos > 9216)
			{
				usCurPos = usCurPos - 1024;
				
				/* �ɼ�ǰ1024+g_usTrigCount������ */
				for(i = 0; i < (g_usTrigCount + 1024); i++)
				{
					j = i*3;
					g_usWaveBuf1[j] = ADC1ConvertedValue[i+usCurPos];
					g_usWaveBuf1[j+1] = ADC2ConvertedValue[i+usCurPos];
					g_usWaveBuf1[j+2] = ADC3ConvertedValue[i+usCurPos];
				}
				
				k = i*3;
				usCurPos = 10240 - (1024 - g_usTrigCount);
				/* ʣ�����ݵĲɼ� */
				for(i = usCurPos; i < 10240; i++)
				{
					j = (i - usCurPos)*3;
					g_usWaveBuf1[j + k] = ADC1ConvertedValue[i];
					g_usWaveBuf1[j+1+k] = ADC2ConvertedValue[i];
					g_usWaveBuf1[j+2+k] = ADC3ConvertedValue[i];
				}
			}
			/* �ڶ��������1024 <= ��ǰ��λ�� <= 10240 - 1024(9216)  */
			else
			{
				usCurPos = usCurPos - 1024;
				for(i = 0; i < 2048; i++)
				{
					g_usWaveBuf1[i*3] = ADC1ConvertedValue[i+usCurPos];
					g_usWaveBuf1[i*3+1] = ADC2ConvertedValue[i+usCurPos];
					g_usWaveBuf1[i*3+2] = ADC3ConvertedValue[i+usCurPos];
				}
			}
			
		}
	
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

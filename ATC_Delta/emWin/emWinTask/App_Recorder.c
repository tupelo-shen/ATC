/*
*********************************************************************************************************
*	                                  
*	ģ������ : ¼���ͻط�
*	�ļ����� : App_Record.c
*	��    �� : V1.2
*	˵    �� : ¼���ͻط�
*	�޸ļ�¼ :
*		�汾��    ����          ����         ˵��
*		v1.0    2013-04-26    Eric2013      �׷�
*       v1.1    2014-05-23    Eric2013      1. ����������ͬʱ֧��4.3�磬5���7��������ʾ
*       v1.2    2014-07-19    Eric2013      1. ���������ʵ�ֿ��ٵı���ˢ��
*	  
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
#include <inttypes.h>


/*
*********************************************************************************************************
*                                         ����
*********************************************************************************************************
*/ 
uint8_t BBB = 0, flag = 0;
char *buf;

/*
*********************************************************************************************************
*                                        �ļ��ڲ�����
*********************************************************************************************************
*/ 
static void StartPlay(void);
static void StartRecord(void);

/*
	¼��ʱ���ݱ����ڰ�����STM32������������2MB SRAM�У�ȱʡʹ��8K�����ʣ�16bit��ʽ��1MB���Ա���64��¼����
	��������ͬ�ĸ�ʽ�����ⲿSRAM�ж�ȡ���ݡ�

	I2S_StartRecord() �� I2S_StartPlay()�������β�ֵ��
    �ӿڱ�׼֧�֣�
		#define I2S_Standard_Phillips           ((uint16_t)0x0000)
		#define I2S_Standard_MSB                ((uint16_t)0x0010)
		#define I2S_Standard_LSB                ((uint16_t)0x0020)
		#define I2S_Standard_PCMShort           ((uint16_t)0x0030)
		#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)

    �ֳ�֧�֣�
    	#define I2S_DataFormat_16b              ((uint16_t)0x0000)
		#define I2S_DataFormat_16bextended      ((uint16_t)0x0001)
		#define I2S_DataFormat_24b              ((uint16_t)0x0003)
		#define I2S_DataFormat_32b              ((uint16_t)0x0005)

	����Ƶ��֧�֣�
		#define I2S_AudioFreq_192k               ((uint32_t)192000)
		#define I2S_AudioFreq_96k                ((uint32_t)96000)
		#define I2S_AudioFreq_48k                ((uint32_t)48000)
		#define I2S_AudioFreq_44k                ((uint32_t)44100)
		#define I2S_AudioFreq_32k                ((uint32_t)32000)
		#define I2S_AudioFreq_22k                ((uint32_t)22050)
		#define I2S_AudioFreq_16k                ((uint32_t)16000)
		#define I2S_AudioFreq_11k                ((uint32_t)11025)
		#define I2S_AudioFreq_8k                 ((uint32_t)8000)
*/

/* ¼����״̬ */
enum
{
	STA_IDLE = 0,	/* ����״̬ */
	STA_RECORDING,	/* ¼��״̬ */
	STA_PLAYING,	/* ����״̬ */
	STA_STOP_I2S,	/* ��ʱ״̬��������֪ͨI2S�жϷ������׼��ֹͣ */
};

typedef struct
{
	uint8_t ucFmtIdx;			/* ��Ƶ��ʽ����׼��λ��������Ƶ�� */
	uint8_t ucVolume;			/* ��ǰ�������� */
	uint8_t ucMicGain;			/* ��ǰMIC���� */	
	int16_t *pAudio;			/* ����һ��ָ�룬���ڷ����ⲿSRAM */	
	uint32_t uiCursor;			/* ����λ�� */
	
	uint8_t ucStatus;			/* ¼����״̬��0��ʾ������1��ʾ¼���У�2��ʾ������ */	
}REC_T;

WM_HWIN hFrame;
REC_T g_tRec;

/* ��Ƶ��ʽ�л��б�(�����Զ���) */
#define FMT_COUNT	6		/* ��Ƶ��ʽ����Ԫ�ظ��� */
uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};


/*
*********************************************************************************************************
*							�Ի����ʼ��ѡ��		        
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRecorder[] = {
     { FRAMEWIN_CreateIndirect,  "WM8978 Recorder",               0,            	0,  0,  800, 444, 0, 0},
    { BUTTON_CreateIndirect,    "Recorder",          		 GUI_ID_BUTTON0,           15, 175, 60, 25, 0,0},
    { BUTTON_CreateIndirect,    "PlayBack",          		 GUI_ID_BUTTON1,           80, 175, 60, 25, 0,0},
    { TEXT_CreateIndirect,      "Audio Formats",     		 GUI_ID_TEXT0,             15, 3,   150, 16, 0,0},
    { TEXT_CreateIndirect,      "Recorder PlayBack&Volume",  GUI_ID_TEXT1,     		   15, 150, 400, 16, 0,0},
    { PROGBAR_CreateIndirect,    NULL,               		 GUI_ID_PROGBAR0,          160, 175, 250, 25, 0,0},
    { RADIO_CreateIndirect,      NULL,               		 GUI_ID_RADIO0,            15,  25, 213,120, 0,6},
    { RADIO_CreateIndirect,      NULL,               		 GUI_ID_RADIO1,            260, 25, 213,100, 0,2}, 	
    { SLIDER_CreateIndirect,     NULL,               		 GUI_ID_SLIDER0,           15,  210, 250,25, 0,0},
};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialogFile
*	����˵��: �ļ����������ػ�  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void PaintDialogRecorder(WM_MESSAGE * pMsg)
{
//    WM_HWIN hWin = pMsg->hWin;

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialogFile
*	����˵��: ��ʼ���Ի���  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void InitDialogRecorder(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
    //
    //FRAMEWIN
    //
	FRAMEWIN_SetFont(hWin, &GUI_Font24B_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_SetTitleHeight(hWin,28);


    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font16B_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font16B_ASCII);

    PROGBAR_SetMinMax(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 0, 1024);

    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"I2S_Standard_Phillips, 16b, 8k",   0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"I2S_Standard_Phillips, 16b, 16k",  1);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"I2S_Standard_Phillips, 16b, 22k",  2);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"I2S_Standard_Phillips, 16b, 44k",  3);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"I2S_Standard_Phillips, 16b, 96k",  4);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"I2S_Standard_Phillips, 16b, 192k", 5);
    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO0), &GUI_Font16_ASCII);

    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1),"Speaker",  0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1),"Ear", 1);
    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO1), &GUI_Font16_ASCII);

    SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), 0,63);
    SLIDER_SetNumTicks(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), 21);
    SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), 40);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSYS
*	����˵��: SYS�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
static void _cbCallbackRecorder(WM_MESSAGE * pMsg) 
{ 
	 
    int xSize, ySize; 
	int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hClient;

	hClient = WM_GetClientWindow(hWin);
	xSize = WM_GetWindowSizeX(hClient);
	ySize = WM_GetWindowSizeY(hClient);
   
    switch (pMsg->MsgId) 
    {
        case WM_TIMER:
	       PROGBAR_SetValue(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), g_tRec.uiCursor >> 10);
		   WM_RestartTimer(pMsg->Data.v, 100);
	       break;
		
        case WM_DELETE:
		    OnICON12 = 0;
			prevent_refresh = 0;	
		    break;
		
		case WM_PAINT:
			/* ������ˮƽ��ɫ�ݶ����ľ��� */
			GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  xSize - 1,	   /* ���½�X λ�� */
							  ySize - 1,       /* ���½�Y λ�� */
							  GUI_YELLOW,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_RED);        /* �������Ҳ�Ҫ���Ƶ���ɫ */

            GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  20,			   /* ���Ͻ�Y λ�� */
							  400,	           /* ���½�X λ�� */
							  21,              /* ���½�Y λ�� */
							  GUI_LIGHTBLUE,  /* ���������Ҫ���Ƶ���ɫ */
							  GUI_WHITE);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  170,			   /* ���Ͻ�Y λ�� */
							  400,	           /* ���½�X λ�� */
							  171,             /* ���½�Y λ�� */
							  GUI_LIGHTBLUE,  /* ���������Ҫ���Ƶ���ɫ */
							  GUI_WHITE);     /* �������Ҳ�Ҫ���Ƶ���ɫ */


            PaintDialogRecorder(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogRecorder(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_DROPDOWN0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_SEL_CHANGED:
							
                            break;
                    }
                    break;
				case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;

                /* ��ʼ��USB HOST */
				case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                           StartRecord();
                           break;
                    }
                    break;

                /*  �ر�USB HOST */
                case GUI_ID_BUTTON1:
                
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            StartPlay(); 
                            break;
                    }
                    break;
                
                 case GUI_ID_RADIO0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            g_tRec.ucFmtIdx = RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO0));
                            break;
                    }
                    break;

                case GUI_ID_RADIO1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1)) == 0)
                            {
                                wm8978_CfgAudioPath(DAC_ON, SPK_ON);    
                            }
                            else
                            {   
                                /* ����WM8978оƬ������ΪDAC�����Ϊ���� */
	                            wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);   
                            }
                            break;
                    }
                    break;
                case GUI_ID_SLIDER0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            g_tRec.ucVolume = SLIDER_GetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0));
                            wm8978_SetSpkVolume(g_tRec.ucVolume);
                            /* ����������������ͬ���� */
                        	wm8978_SetEarVolume(g_tRec.ucVolume);
                            break;
                    }
                    break;
            }
            break;
    }

	WM_DefaultProc(pMsg);	
}



void WM8978_Init()
{   
    wm8978_Init();
    /* ��ʼ��ȫ�ֱ��� */
	g_tRec.ucVolume = 40;		    /* ȱʡ����    */
	g_tRec.ucMicGain = 34;			/* ȱʡPGA���� */
    
    I2S_CODEC_Init();

    /* ����WM8978оƬ������ΪDAC�����Ϊ���� */
	wm8978_CfgAudioPath(DAC_ON, SPK_ON);

    wm8978_SetSpkVolume(g_tRec.ucVolume);
    /* ����������������ͬ���� */
	wm8978_SetEarVolume(g_tRec.ucVolume);

	g_tRec.ucFmtIdx = 0;		/* ȱʡ��Ƶ��ʽ(16Bit, 16KHz) */
	g_tRec.pAudio = (int16_t *)EXT_SRAM_ADDR;	/* ͨ�� init16_t * ��ָ������ⲿSRAM */

	/* ����¼��������. SRAM����2M�ֽ� */
	{
		int i;

		for (i = 0 ; i < 1 * 1024 * 1024; i++)
		{
			g_tRec.pAudio[i] = 0;
		}
	}
                                               
    g_tRec.ucStatus = STA_IDLE;		/* ���Ƚ������״̬ */       

}

/*
*********************************************************************************************************
*	�� �� ��: StartPlay
*	����˵��: ����WM8978��STM32��I2S��ʼ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void StartPlay(void)
{
	/* ����Ѿ���¼���ͷ���״̬������Ҫ��ֹͣ�ٿ��� */
	g_tRec.ucStatus = STA_STOP_I2S;
//  BSP_OS_TimeDlyMs(10); /* �ӳ�һ��ʱ�䣬�ȴ�I2S�жϽ��� */
	wm8978_Init();		  /* ��λWM8978����λ״̬ */
	I2S_Stop();			  /* ֹͣI2S¼���ͷ��� */

	g_tRec.ucStatus = STA_PLAYING;		/* ����״̬ */

	g_tRec.uiCursor = 0;		/* ����ָ�븴λΪ0����ͷ��ʼ���� */
    
    /* ���ŵ�ʱ�����һ��Ҫ����һ�£�Ҫ���᲻��� */
    if(RADIO_GetValue(WM_GetDialogItem(hFrame, GUI_ID_RADIO1)) == 0)
    {
        wm8978_CfgAudioPath(DAC_ON, SPK_ON);    
    }
    else
    {   
        /* ����WM8978оƬ������ΪDAC�����Ϊ���� */
        wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);   
    }

	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	/* ����STM32��I2S��Ƶ�ӿ�(��������ֱ�׼I2S�ӿڣ�16bit�� 8K������), ��ʼ����*/
	I2S_StartPlay(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
}

/*
*********************************************************************************************************
*	�� �� ��: StartRecord
*	����˵��: ����WM8978��STM32��I2S��ʼ¼����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void StartRecord(void)
{
	/* ����Ѿ���¼���ͷ���״̬������Ҫ��ֹͣ�ٿ��� */
	g_tRec.ucStatus = STA_STOP_I2S;
//    BSP_OS_TimeDlyMs(10); 	
//	bsp_DelayMS(10);	/* �ӳ�һ��ʱ�䣬�ȴ�I2S�жϽ��� */
	wm8978_Init();		/* ��λWM8978����λ״̬ */
	I2S_Stop();			/* ֹͣI2S¼���ͷ��� */

	g_tRec.ucStatus = STA_RECORDING;		/* ¼��״̬ */

	g_tRec.uiCursor = 0;	/* ����ָ�븴λΪ0����ͷ��ʼ¼�� */

	/* ����WM8978оƬ������ΪMic�����Ϊ���� */
	//wm8978_CfgAudioPath(MIC_LEFT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	//wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* ���ڷ���������������ͬ���� */
	wm8978_SetEarVolume(g_tRec.ucVolume);

	/* ����MICͨ������ */
	wm8978_SetMicGain(g_tRec.ucMicGain);

	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	/* ����STM32��I2S��Ƶ�ӿ�(��������ֱ�׼I2S�ӿڣ�16bit�� 8K������), ��ʼ¼�� */
	I2S_StartRecord(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
}

/*
*********************************************************************************************************
*	�� �� ��: I2S_CODEC_DataTransfer
*	����˵��: I2S���ݴ��亯��, ��SPI2 (I2S)�жϷ��������á�¼���ͷ������ڴ˺�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void I2S_CODEC_DataTransfer(void)
{
	uint16_t usData;

	/* ֻ¼������(������STM32������MIC��װ��������), ����ʱ����������ͬ */
	if (g_tRec.ucStatus == STA_RECORDING)	/* ¼��״̬ */
	{	
			
		if (SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE) == SET)
		{
			usData = SPI_I2S_ReceiveData(I2S2ext);
			#if 1		/* ¼˫ͨ�� */
				g_tRec.pAudio[g_tRec.uiCursor] = usData;	/* ������Ƶ���� */
				if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
				{
					g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
					/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
					SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
					/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
					SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
                    

                    g_tRec.ucStatus = STA_IDLE;		/* ����״̬ */
					I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
					wm8978_Init();	/* ��λWM8978����λ״̬ */

				}
			#else	/* ѡ��һ���������м�¼ */
				if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE) != SET)	/* �ж��ǲ��������� */
				{
					g_tRec.pAudio[g_tRec.uiCursor] = usData;	/* ������Ƶ���� */
					if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
					{
						g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
						/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
						SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
						/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
						SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

						g_tRec.ucStatus = STA_IDLE;		/* ����״̬ */
					    I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
					    wm8978_Init();	/* ��λWM8978����λ״̬ */
					}
				}
			#endif			
		}

		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)		
		{
			SPI_I2S_SendData(SPI2, 0);
		}
	}
	else if (g_tRec.ucStatus == STA_PLAYING)	/* ����״̬����������ֵ���������� */
	{
		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
		{
			#if 1		/* ˫ͨ������ */			
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* ��ȡ��Ƶ���� */
				if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
				{
					g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
					/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
					SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
					/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
					SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

					g_tRec.ucStatus = STA_IDLE;		/* ����״̬ */
					I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
					wm8978_Init();	/* ��λWM8978����λ״̬ */
				}
				SPI_I2S_SendData(SPI2, usData); 
				
			#else	/* ���Խ�����1��ͨ�����������Ƶ�����1��ͨ��(����������¼����������¼��) */
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* ��ȡ��Ƶ���� */
				if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE) != SET)	/* �ж��ǲ��������� */
				{
					if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
					{
						g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
						/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
						SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
						/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
						SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

					    g_tRec.ucStatus = STA_IDLE;		/* ����״̬ */
					    I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
					    wm8978_Init();	/* ��λWM8978����λ״̬ */
					}
				}
				SPI_I2S_SendData(SPI2, usData);				
			#endif
		}		
	}
	else 	/* ��ʱ״̬ STA_STOP_I2S������ֹͣ�ж� */
	{
		/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
		SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
		/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: OnICON0Clicked
*	����˵��: �ļ�ϵͳ���������� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OnICON12Clicked(void)
{ 		
    WM8978_Init();
    hFrame = GUI_CreateDialogBox(_aDialogCreateRecorder, GUI_COUNTOF(_aDialogCreateRecorder), &_cbCallbackRecorder, 0, 0, 0);  	
	WM_CreateTimer(WM_GetClientWindow(hFrame),  /* ������Ϣ�Ĵ��ڵľ�� */
               0, 	                            /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
			   100,                             /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
			   0);	                            /* ��������ʹ�ã�ӦΪ0 */
	while(OnICON12)
	{					 				
        GUI_Delay(10);  				
	}
	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

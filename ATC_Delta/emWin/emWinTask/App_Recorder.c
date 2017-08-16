/*
*********************************************************************************************************
*	                                  
*	模块名称 : 录音和回放
*	文件名称 : App_Record.c
*	版    本 : V1.2
*	说    明 : 录音和回放
*	修改记录 :
*		版本号    日期          作者         说明
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      1. 更改主界面同时支持4.3寸，5寸和7寸屏的显示
*       v1.2    2014-07-19    Eric2013      1. 配合主界面实现快速的背景刷新
*	  
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
#include <inttypes.h>


/*
*********************************************************************************************************
*                                         变量
*********************************************************************************************************
*/ 
uint8_t BBB = 0, flag = 0;
char *buf;

/*
*********************************************************************************************************
*                                        文件内部函数
*********************************************************************************************************
*/ 
static void StartPlay(void);
static void StartRecord(void);

/*
	录音时数据保存在安富莱STM32开发板外扩的2MB SRAM中，缺省使用8K采样率，16bit格式。1MB可以保存64秒录音。
	放音按相同的格式，从外部SRAM中读取数据。

	I2S_StartRecord() 和 I2S_StartPlay()函数的形参值域
    接口标准支持：
		#define I2S_Standard_Phillips           ((uint16_t)0x0000)
		#define I2S_Standard_MSB                ((uint16_t)0x0010)
		#define I2S_Standard_LSB                ((uint16_t)0x0020)
		#define I2S_Standard_PCMShort           ((uint16_t)0x0030)
		#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)

    字长支持：
    	#define I2S_DataFormat_16b              ((uint16_t)0x0000)
		#define I2S_DataFormat_16bextended      ((uint16_t)0x0001)
		#define I2S_DataFormat_24b              ((uint16_t)0x0003)
		#define I2S_DataFormat_32b              ((uint16_t)0x0005)

	采样频率支持：
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

/* 录音机状态 */
enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_RECORDING,	/* 录音状态 */
	STA_PLAYING,	/* 放音状态 */
	STA_STOP_I2S,	/* 临时状态，主程序通知I2S中断服务程序准备停止 */
};

typedef struct
{
	uint8_t ucFmtIdx;			/* 音频格式：标准、位长、采样频率 */
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucMicGain;			/* 当前MIC增益 */	
	int16_t *pAudio;			/* 定义一个指针，用于访问外部SRAM */	
	uint32_t uiCursor;			/* 播放位置 */
	
	uint8_t ucStatus;			/* 录音机状态，0表示待机，1表示录音中，2表示播放中 */	
}REC_T;

WM_HWIN hFrame;
REC_T g_tRec;

/* 音频格式切换列表(可以自定义) */
#define FMT_COUNT	6		/* 音频格式数组元素个数 */
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
*							对话框初始化选项		        
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
*	函 数 名: PaintDialogFile
*	功能说明: 文件管理界面的重绘  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void PaintDialogRecorder(WM_MESSAGE * pMsg)
{
//    WM_HWIN hWin = pMsg->hWin;

}

/*
*********************************************************************************************************
*	函 数 名: InitDialogFile
*	功能说明: 初始化对话框  
*	形    参：pMsg
*	返 回 值: 无		        
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
*	函 数 名: _cbCallbackSYS
*	功能说明: SYS对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无		        
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
			/* 绘制用水平颜色梯度填充的矩形 */
			GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  xSize - 1,	   /* 右下角X 位置 */
							  ySize - 1,       /* 右下角Y 位置 */
							  GUI_YELLOW,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_RED);        /* 矩形最右侧要绘制的颜色 */

            GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  20,			   /* 左上角Y 位置 */
							  400,	           /* 右下角X 位置 */
							  21,              /* 右下角Y 位置 */
							  GUI_LIGHTBLUE,  /* 矩形最左侧要绘制的颜色 */
							  GUI_WHITE);     /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  170,			   /* 左上角Y 位置 */
							  400,	           /* 右下角X 位置 */
							  171,             /* 右下角Y 位置 */
							  GUI_LIGHTBLUE,  /* 矩形最左侧要绘制的颜色 */
							  GUI_WHITE);     /* 矩形最右侧要绘制的颜色 */


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

                /* 初始化USB HOST */
				case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                           StartRecord();
                           break;
                    }
                    break;

                /*  关闭USB HOST */
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
                                /* 配置WM8978芯片，输入为DAC，输出为耳机 */
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
                            /* 调节音量，左右相同音量 */
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
    /* 初始化全局变量 */
	g_tRec.ucVolume = 40;		    /* 缺省音量    */
	g_tRec.ucMicGain = 34;			/* 缺省PGA增益 */
    
    I2S_CODEC_Init();

    /* 配置WM8978芯片，输入为DAC，输出为喇叭 */
	wm8978_CfgAudioPath(DAC_ON, SPK_ON);

    wm8978_SetSpkVolume(g_tRec.ucVolume);
    /* 调节音量，左右相同音量 */
	wm8978_SetEarVolume(g_tRec.ucVolume);

	g_tRec.ucFmtIdx = 0;		/* 缺省音频格式(16Bit, 16KHz) */
	g_tRec.pAudio = (int16_t *)EXT_SRAM_ADDR;	/* 通过 init16_t * 型指针访问外部SRAM */

	/* 清零录音缓冲区. SRAM容量2M字节 */
	{
		int i;

		for (i = 0 ; i < 1 * 1024 * 1024; i++)
		{
			g_tRec.pAudio[i] = 0;
		}
	}
                                               
    g_tRec.ucStatus = STA_IDLE;		/* 首先进入空闲状态 */       

}

/*
*********************************************************************************************************
*	函 数 名: StartPlay
*	功能说明: 配置WM8978和STM32的I2S开始放音。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void StartPlay(void)
{
	/* 如果已经再录音和放音状态，则需要先停止再开启 */
	g_tRec.ucStatus = STA_STOP_I2S;
//  BSP_OS_TimeDlyMs(10); /* 延迟一段时间，等待I2S中断结束 */
	wm8978_Init();		  /* 复位WM8978到复位状态 */
	I2S_Stop();			  /* 停止I2S录音和放音 */

	g_tRec.ucStatus = STA_PLAYING;		/* 放音状态 */

	g_tRec.uiCursor = 0;		/* 数据指针复位为0，从头开始放音 */
    
    /* 播放的时候，这个一定要设置一下，要不会不输出 */
    if(RADIO_GetValue(WM_GetDialogItem(hFrame, GUI_ID_RADIO1)) == 0)
    {
        wm8978_CfgAudioPath(DAC_ON, SPK_ON);    
    }
    else
    {   
        /* 配置WM8978芯片，输入为DAC，输出为耳机 */
        wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);   
    }

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	/* 配置STM32的I2S音频接口(比如飞利浦标准I2S接口，16bit， 8K采样率), 开始放音*/
	I2S_StartPlay(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
}

/*
*********************************************************************************************************
*	函 数 名: StartRecord
*	功能说明: 配置WM8978和STM32的I2S开始录音。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void StartRecord(void)
{
	/* 如果已经再录音和放音状态，则需要先停止再开启 */
	g_tRec.ucStatus = STA_STOP_I2S;
//    BSP_OS_TimeDlyMs(10); 	
//	bsp_DelayMS(10);	/* 延迟一段时间，等待I2S中断结束 */
	wm8978_Init();		/* 复位WM8978到复位状态 */
	I2S_Stop();			/* 停止I2S录音和放音 */

	g_tRec.ucStatus = STA_RECORDING;		/* 录音状态 */

	g_tRec.uiCursor = 0;	/* 数据指针复位为0，从头开始录音 */

	/* 配置WM8978芯片，输入为Mic，输出为耳机 */
	//wm8978_CfgAudioPath(MIC_LEFT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	//wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* 调节放音音量，左右相同音量 */
	wm8978_SetEarVolume(g_tRec.ucVolume);

	/* 设置MIC通道增益 */
	wm8978_SetMicGain(g_tRec.ucMicGain);

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	/* 配置STM32的I2S音频接口(比如飞利浦标准I2S接口，16bit， 8K采样率), 开始录音 */
	I2S_StartRecord(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
}

/*
*********************************************************************************************************
*	函 数 名: I2S_CODEC_DataTransfer
*	功能说明: I2S数据传输函数, 被SPI2 (I2S)中断服务程序调用。录音和放音均在此函数处理。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void I2S_CODEC_DataTransfer(void)
{
	uint16_t usData;

	/* 只录左声道(安富莱STM32开发板MIC安装在左声道), 放音时左右声道相同 */
	if (g_tRec.ucStatus == STA_RECORDING)	/* 录音状态 */
	{	
			
		if (SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE) == SET)
		{
			usData = SPI_I2S_ReceiveData(I2S2ext);
			#if 1		/* 录双通道 */
				g_tRec.pAudio[g_tRec.uiCursor] = usData;	/* 保存音频数据 */
				if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
				{
					g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
					/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
					SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
					/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
					SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
                    

                    g_tRec.ucStatus = STA_IDLE;		/* 待机状态 */
					I2S_Stop();		/* 停止I2S录音和放音 */
					wm8978_Init();	/* 复位WM8978到复位状态 */

				}
			#else	/* 选择一个声道进行记录 */
				if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE) != SET)	/* 判断是不是左声道 */
				{
					g_tRec.pAudio[g_tRec.uiCursor] = usData;	/* 保存音频数据 */
					if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
					{
						g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
						/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
						SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
						/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
						SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

						g_tRec.ucStatus = STA_IDLE;		/* 待机状态 */
					    I2S_Stop();		/* 停止I2S录音和放音 */
					    wm8978_Init();	/* 复位WM8978到复位状态 */
					}
				}
			#endif			
		}

		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)		
		{
			SPI_I2S_SendData(SPI2, 0);
		}
	}
	else if (g_tRec.ucStatus == STA_PLAYING)	/* 放音状态，右声道的值等于左声道 */
	{
		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
		{
			#if 1		/* 双通道放音 */			
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* 读取音频数据 */
				if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
				{
					g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
					/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
					SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
					/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
					SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

					g_tRec.ucStatus = STA_IDLE;		/* 待机状态 */
					I2S_Stop();		/* 停止I2S录音和放音 */
					wm8978_Init();	/* 复位WM8978到复位状态 */
				}
				SPI_I2S_SendData(SPI2, usData); 
				
			#else	/* 可以将其中1个通道的声音复制到另外1个通道(用于左声道录音或右声道录音) */
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* 读取音频数据 */
				if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE) != SET)	/* 判断是不是左声道 */
				{
					if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
					{
						g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
						/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
						SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
						/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
						SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

					    g_tRec.ucStatus = STA_IDLE;		/* 待机状态 */
					    I2S_Stop();		/* 停止I2S录音和放音 */
					    wm8978_Init();	/* 复位WM8978到复位状态 */
					}
				}
				SPI_I2S_SendData(SPI2, usData);				
			#endif
		}		
	}
	else 	/* 临时状态 STA_STOP_I2S，用于停止中断 */
	{
		/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
		SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
		/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
	}
}

/*
*********************************************************************************************************
*	函 数 名: OnICON0Clicked
*	功能说明: 文件系统管理主函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void OnICON12Clicked(void)
{ 		
    WM8978_Init();
    hFrame = GUI_CreateDialogBox(_aDialogCreateRecorder, GUI_COUNTOF(_aDialogCreateRecorder), &_cbCallbackRecorder, 0, 0, 0);  	
	WM_CreateTimer(WM_GetClientWindow(hFrame),  /* 接受信息的窗口的句柄 */
               0, 	                            /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
			   100,                             /* 周期，此周期过后指定窗口应收到消息*/
			   0);	                            /* 留待将来使用，应为0 */
	while(OnICON12)
	{					 				
        GUI_Delay(10);  				
	}
	
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

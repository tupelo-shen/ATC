/*
*********************************************************************************************************
*	                                  
*	模块名称 : 收音机
*	文件名称 : App_Radio.c
*	版    本 : V1.2
*	说    明 : 收音机
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


static const char *TaskProfile[2]={" "," "};

/*
AM873，FM88.4，武汉人民广播电台（新闻综合频率）



	武汉地区FM电台频率表：
	FM89.6 武汉广播电台交通台
	FM91.6 楚天广播电台卫星台
	FM92.7 楚天广播电台交通体育台
	FM93.6 武汉广播电台中波台
	FM95.6 中央广播电台中国之声
	FM96.6 湖北广播电台生活频道
	FM97.8 中央广播电台经济之声
	FM99.8 湖北广播电台经济频道
	FM100.6 长江经济广播电台
	FM101.8 武汉广播电台文艺台
	FM102.6 湖北广播电台妇女儿童频道
	FM103.8 湖北广播电台音乐频道
	FM104.6 湖北广播电台中波频道
	FM105.8 楚天广播电台音乐台
	FM107.8 湖北广播电台交通频道
*/

const uint16_t g_InitListFM[] = {
	8840,
	8960,
	9160,
	9270,
	9360,
	9560,
	9660,
	9780,
	9980,
	10060,
	10180,
	10260,
	10380,
	10480,
	10490,
	10500
};

const uint16_t g_InitListAM[] = {
	531,
	540,
	549,
	558,
	603,
	639,
	855,
	873,
	882,
	900,
	909,
	918,
	927,
    0,
    0,
    0
};

#define FM_RX		0
#define AM_RX		1

typedef struct
{
	uint8_t ucMode;		/* AM 或 FM */

	uint8_t ucFMCount;	/* FM 电台个数 */
	uint8_t ucIndexFM;	/* 当前电台索引 */

	uint8_t ucAMCount;	/* FM 电台个数 */
	uint8_t ucIndexAM;	/* 当前电台索引 */

	uint32_t usFMFreq;	/* 当前电台频率 */
    uint32_t usAMFreq;	/* 当前电台频率 */

	uint8_t ucVolume;	/* 音量 */
}RADIO_T;


RADIO_T g_tRadio;

/*
*********************************************************************************************************
*							对话框初始化选项		        
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRadio[] = {
    { FRAMEWIN_CreateIndirect,  "Si4730 FM/AM",               0,            0,  0,  800, 444, 0, 0},
    { TEXT_CreateIndirect,      "",              GUI_ID_TEXT0,            15, 0, 240, 16, 0,0},
    { TEXT_CreateIndirect,      "",              GUI_ID_TEXT1,            15, 16, 240, 16, 0,0},
    { TEXT_CreateIndirect,      "",              GUI_ID_TEXT2,            15, 32, 240, 16, 0,0},
    { TEXT_CreateIndirect,      "",              GUI_ID_TEXT3,            15, 48, 240, 16, 0,0},
    { LISTVIEW_CreateIndirect,   NULL,           GUI_ID_LISTVIEW0,        250,  5,  180,220,0,0},
    { RADIO_CreateIndirect,      NULL,           GUI_ID_RADIO0,           15,  70, 100, 40,0,2},
    { RADIO_CreateIndirect,      NULL,           GUI_ID_RADIO1,           15,  110, 100, 40,0,2},
    { BUTTON_CreateIndirect,    "Pre",           GUI_ID_BUTTON0,          15, 160, 100, 30, 0,0},
    { BUTTON_CreateIndirect,    "Next",          GUI_ID_BUTTON1,          135, 160, 100, 30, 0,0},
    { SLIDER_CreateIndirect,     NULL,           GUI_ID_SLIDER0,          5,  200, 190,30, 0,0}, 
};

/*
*********************************************************************************************************
*	函 数 名: PaintDialogFile
*	功能说明: 文件管理界面的重绘  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void PaintDialogRadio(WM_MESSAGE * pMsg)
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
void InitDialogRadio(WM_MESSAGE * pMsg)
{
    char buf[20];
    uint8_t Buf[7];
    WM_HWIN hWin = pMsg->hWin;
    unsigned char i;
    HEADER_Handle hHeader;
    SCROLLBAR_Handle hScrollbar;

	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
    //
    //FRAMEWIN
    //
	FRAMEWIN_SetFont(hWin, &GUI_Font24B_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_SetTitleHeight(hWin,28);

    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO0),&GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0), "FM",   0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0), "AM",   1);

    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO1),&GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "Spek",   0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "Ear",   1);
    //
    //GUI_ID_LISTVIEW0
    //
	hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0));
	HEADER_SetFont(hHeader,&GUI_Font16B_ASCII);
	HEADER_SetHeight(hHeader,16);
    hScrollbar = SCROLLBAR_CreateAttached(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),SCROLLBAR_CF_VERTICAL);
	SCROLLBAR_SetWidth(hScrollbar,18);

    LISTVIEW_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),0,0x000000);
    LISTVIEW_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),0,0x00ff00);
    LISTVIEW_SetFont(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),&GUI_Font16B_ASCII);

    LISTVIEW_SetAutoScrollV(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),1);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"FM",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"AM",GUI_TA_VCENTER|GUI_TA_LEFT);
  
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 0, 80);
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 1, 80);

	
	for(i = 0; i < 16; i++)
	{
		LISTVIEW_AddRow(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), (GUI_ConstString *)TaskProfile);
		
        sprintf(buf, "%d.%dMHz", g_InitListFM[i]/100, g_InitListFM[i]%100);
        LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 0, i, buf);
        sprintf(buf, "%dKHz",  g_InitListAM[i]);		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 1, i, buf);				
	}

    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0), &GUI_Font16B_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font16B_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2), &GUI_Font16B_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font16B_ASCII);

	if (SI4730_GetAMTuneStatus(Buf))
	{
		if (Buf[0] == 0)
		{
			
            TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), "Invalid Frequency");
		}
		else
		{   
            uint32_t ulFreq;
            ulFreq = (Buf[1] << 8) + Buf[2];	/* 单位 10kHz */
            sprintf(buf, "Frequency = %d.%dMHz\r\n", ulFreq / 100, (ulFreq % 100));	/* 当前调谐频率 */
			TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), buf);
		}

		sprintf(buf, "RSSI = %ddBμV\r\n", Buf[3]);
        TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1), buf);

		sprintf(buf, "SNR = %ddB\r\n", Buf[4]);	
        TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT2), buf);
	}

    sprintf(buf, "FM = %d.%dMHz", g_tRadio.usFMFreq/100, g_tRadio.usFMFreq%100);	
    TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3), buf);

    SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), 0,63);
    SLIDER_SetNumTicks(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), 21);
    SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), g_tRadio.ucVolume);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackSYS
*	功能说明: SYS对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void _cbCallbackRadio(WM_MESSAGE * pMsg) 
{ 
    char buf[30];
    uint8_t Buf[7];
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

           if (SI4730_GetAMTuneStatus(Buf))
        	{
        		if (Buf[0] == 0)
        		{
        			
                    TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), "Invalid Frequency");
        		}
        		else
        		{   
                    uint32_t ulFreq;
                    ulFreq = (Buf[1] << 8) + Buf[2];	/* 单位 10kHz */
                    sprintf(buf, "Frequency = %d.%dMHz\r\n", ulFreq / 100, (ulFreq % 100));	/* 当前调谐频率 */
        			TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), buf);
        		}
        
        		sprintf(buf, "RSSI = %ddBμV\r\n", Buf[3]);
                TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1), buf);
        
        		sprintf(buf, "SNR = %ddB\r\n", Buf[4]);	
                TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT2), buf);
           }
           else
           {
                sprintf(buf, "FM = %d.%dMHz", g_tRadio.usFMFreq/100, g_tRadio.usFMFreq%100);	
                TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3), buf);    
           }

           WM_RestartTimer(pMsg->Data.v, 100);
	       break;
		   
        case WM_DELETE:
		    OnICON8 = 0;
			prevent_refresh = 0;
		    break;
		
		case WM_PAINT:

            GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  xSize - 1,	   /* 右下角X 位置 */
							  ySize - 1,       /* 右下角Y 位置 */
							  GUI_YELLOW,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_RED);        /* 矩形最右侧要绘制的颜色 */
            
//             GUI_DrawGradientH(0,			   /* 左上角X 位置 */
// 							  200,			   /* 左上角Y 位置 */
// 							  300,	           /* 右下角X 位置 */
// 							  201,             /* 右下角Y 位置 */
// 							  GUI_LIGHTBLUE,  /* 矩形最左侧要绘制的颜色 */
// 							  GUI_WHITE);     /* 矩形最右侧要绘制的颜色 */


            PaintDialogRadio(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogRadio(pMsg);
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
                case GUI_ID_LISTVIEW0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_SEL_CHANGED:
						    if(g_tRadio.ucMode == FM_RX)
                            {
                                g_tRadio.usFMFreq = g_InitListFM[LISTVIEW_GetSel(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0))];
    						    SI4730_SetFMFreq(g_tRadio.usFMFreq );
                            }
                            else
                            {
                                g_tRadio.usAMFreq = g_InitListAM[LISTVIEW_GetSel(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0))];
    						    SI4730_SetAMFreq(g_tRadio.usAMFreq );    
                            }
                            break;
                    }
                    break;
                case GUI_ID_RADIO0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO0)) == 0)
                            {
                                g_tRadio.ucMode = FM_RX;
                                SI4730_PowerUp_FM_Revice();    
                            }
                            else
                            {   
                                g_tRadio.ucMode = AM_RX;
                                SI4730_PowerUp_AM_Revice(); 
                            }	 
                        	GUI_X_Delay(10);
                            break;
                    }
                    break;

                case GUI_ID_RADIO1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1)) == 0)
                            {
                                wm8978_CfgAudioPath(AUX_ON, SPK_ON);    
                            }
                            else
                            {   
                                /* 配置WM8978芯片，输入为DAC，输出为耳机 */
	                            wm8978_CfgAudioPath(AUX_ON, EAR_LEFT_ON | EAR_RIGHT_ON);   
                            }
                            break;
                    }
                    break;

				case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                           
                           if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO0)) == 0)
                           {
                               g_tRadio.usFMFreq -= 10; 
    						   SI4730_SetFMFreq(g_tRadio.usFMFreq );
                           }
                           else
                           {
                               g_tRadio.usAMFreq -= 10; 
    						   SI4730_SetFMFreq(g_tRadio.usAMFreq );     
                           }
                           break;
                    }
                    break;
                 
				case GUI_ID_BUTTON1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                           if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO0)) == 0)
                           {
                               g_tRadio.usFMFreq += 10; 
    						   SI4730_SetFMFreq(g_tRadio.usFMFreq );
                           }
                           else
                           {
                               g_tRadio.usAMFreq += 10; 
    						   SI4730_SetFMFreq(g_tRadio.usAMFreq );     
                           }
                           break;
                    }
                    break;

               case GUI_ID_SLIDER0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            g_tRadio.ucVolume = SLIDER_GetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0));
                            wm8978_SetSpkVolume(g_tRadio.ucVolume);
                            /* 调节音量，左右相同音量 */
                        	wm8978_SetEarVolume(g_tRadio.ucVolume);
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
            }
            break;
    }

	WM_DefaultProc(pMsg);	
}

/*
*********************************************************************************************************
*	函 数 名: OnICON8Clicked
*	功能说明: 主函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void OnICON8Clicked(void)
{ 	
    WM_HWIN hWin;

    wm8978_Init();

    /* 配置WM8978芯片，输入为AUX接口(收音机)，输出为喇叭 */
	wm8978_CfgAudioPath(AUX_ON, SPK_ON); 

	/* 调节音量，左右相同音量 */
	g_tRadio.ucVolume = 30;
	wm8978_SetEarVolume(g_tRadio.ucVolume);
	wm8978_SetSpkVolume(g_tRadio.ucVolume);

    SI4730_PowerUp_FM_Revice();
//    SI4730_PowerUp_AM_Revice();
	GUI_Delay(10);
	g_tRadio.ucMode = FM_RX;	/* 缺省是FM接收 */
	//g_tRadio.ucMode = AM_RX;	/* 缺省是FM接收 */

    if (g_tRadio.ucMode == FM_RX)
	{
		g_tRadio.usFMFreq = 10430;
        SI4730_SetFMFreq(g_tRadio.usFMFreq);
	}
	else
	{
		g_tRadio.usAMFreq = 520;
        SI4730_SetAMFreq(g_tRadio.usAMFreq);
	}

    hWin = GUI_CreateDialogBox(_aDialogCreateRadio, 
                                 GUI_COUNTOF(_aDialogCreateRadio), 
                                 &_cbCallbackRadio, 
                                 0, 
                                 0, 
                                 0);
    
    WM_CreateTimer(WM_GetClientWindow(hWin),  /* 接受信息的窗口的句柄 */
                   0, 	                        /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
			       100,                         /* 周期，此周期过后指定窗口应收到消息*/
			       0);	                        /* 留待将来使用，应为0 */ 
                                                 	
	while(OnICON8)
	{					 				
        GUI_Delay(10); 				
	}
	
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

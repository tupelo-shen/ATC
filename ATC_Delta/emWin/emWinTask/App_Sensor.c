/*
*********************************************************************************************************
*	                                  
*	模块名称 : 传感器
*	文件名称 : App_Sensor.c
*	版    本 : V1.2
*	说    明 : 传感器
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

/*
*********************************************************************************************************
*							对话框初始化选项		        
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSensor[] = {
    { FRAMEWIN_CreateIndirect,  "BH1750FVI",               0,            0,  0,  800, 444, 0, 0},
    { TEXT_CreateIndirect,      "BH1750FVI",           GUI_ID_TEXT0,            18, 20, 240, 24, 0,0},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO0,             15,  70, 300, 60,0,3},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO1,             15,  140, 300, 60,0,3}, 
};

/*
*********************************************************************************************************
*	函 数 名: PaintDialogFile
*	功能说明: 文件管理界面的重绘  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void PaintDialogSensor(WM_MESSAGE * pMsg)
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
void InitDialogSensor(WM_MESSAGE * pMsg)
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


    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0), &GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT1
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font24B_ASCII);

    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO0),&GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0), "Mode1  RES: 11ux    Test Time: 120ms-180ms",   0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0), "Mode2  RES: 0.51ux  Test Time: 120ms-180ms",   1);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0), "Mode3  RES: 41ux    Test Time: 16ms-24ms",     2);

    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO1),&GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "Sensitivity 1.85 lx or 0.93 lx	MT = 31",  0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "Sensitivity 0.23 lx or 0.11 lx	MT = 254", 1);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "Sensitivity 1.20 lx or 0.60 lx	MT = 69",  2);
//    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0), "",6);

}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackSYS
*	功能说明: SYS对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void _cbCallbackSensor(WM_MESSAGE * pMsg) 
{ 
    char buf[30];
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

           sprintf(buf, "BH1750FVI=%6.2f lux", BH1750_GetLux());
           TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), buf);

           WM_RestartTimer(pMsg->Data.v, 500);

	       break;
        case WM_DELETE:
		    OnICON13 = 0;
			prevent_refresh = 0;
		    break;
		
		case WM_PAINT:
            GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  xSize - 1,	   /* 右下角X 位置 */
							  ySize - 1,       /* 右下角Y 位置 */
							  GUI_YELLOW,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_RED);        /* 矩形最右侧要绘制的颜色 */
            
            GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  45,			   /* 左上角Y 位置 */
							  500,	           /* 右下角X 位置 */
							  46,             /* 右下角Y 位置 */
							  GUI_LIGHTBLUE,  /* 矩形最左侧要绘制的颜色 */
							  GUI_WHITE);     /* 矩形最右侧要绘制的颜色 */


            PaintDialogSensor(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogSensor(pMsg);
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

                 case GUI_ID_RADIO0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1)) == 0)
                            {
                                BH1750_AdjustSensitivity(31);
                            }
                            else if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1)) == 1)
                            {
                                BH1750_AdjustSensitivity(69);
                            }
                            else if(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1)) == 2)
                            {
                                BH1750_AdjustSensitivity(254);
                            }
                            break;
                    }
                    break;
                case GUI_ID_RADIO1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            BH1750_ChageMode(RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1)) + 1);
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
*	函 数 名: OnICON13Clicked
*	功能说明: 主函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void OnICON13Clicked(void)
{ 	
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreateSensor, 
                                 GUI_COUNTOF(_aDialogCreateSensor), 
                                 &_cbCallbackSensor, 
                                 0, 
                                 0, 
                                 0);
    
    WM_CreateTimer(WM_GetClientWindow(hWin),  /* 接受信息的窗口的句柄 */
                   0, 	                        /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
			       500,                         /* 周期，此周期过后指定窗口应收到消息*/
			       0);	                        /* 留待将来使用，应为0 */ 
                                                 	
	while(OnICON13)
	{					 				
        GUI_Delay(10); 				
	}
	
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������
*	�ļ����� : App_Sensor.c
*	��    �� : V1.2
*	˵    �� : ������
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

/*
*********************************************************************************************************
*							�Ի����ʼ��ѡ��		        
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
*	�� �� ��: PaintDialogFile
*	����˵��: �ļ����������ػ�  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void PaintDialogSensor(WM_MESSAGE * pMsg)
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
*	�� �� ��: _cbCallbackSYS
*	����˵��: SYS�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
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
            GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  xSize - 1,	   /* ���½�X λ�� */
							  ySize - 1,       /* ���½�Y λ�� */
							  GUI_YELLOW,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_RED);        /* �������Ҳ�Ҫ���Ƶ���ɫ */
            
            GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  45,			   /* ���Ͻ�Y λ�� */
							  500,	           /* ���½�X λ�� */
							  46,             /* ���½�Y λ�� */
							  GUI_LIGHTBLUE,  /* ���������Ҫ���Ƶ���ɫ */
							  GUI_WHITE);     /* �������Ҳ�Ҫ���Ƶ���ɫ */


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
*	�� �� ��: OnICON13Clicked
*	����˵��: ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
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
    
    WM_CreateTimer(WM_GetClientWindow(hWin),  /* ������Ϣ�Ĵ��ڵľ�� */
                   0, 	                        /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
			       500,                         /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
			       0);	                        /* ��������ʹ�ã�ӦΪ0 */ 
                                                 	
	while(OnICON13)
	{					 				
        GUI_Delay(10); 				
	}
	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

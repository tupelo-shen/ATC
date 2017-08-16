/*
*********************************************************************************************************
*	                                  
*	ģ������ : ADC
*	�ļ����� : App_AD7606.c
*	��    �� : V1.2
*	˵    �� : AD7606
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
static const GUI_WIDGET_CREATE_INFO _aDialogCreateAD7606[] = {
    { FRAMEWIN_CreateIndirect,  "AD7606",               0,            0,  0,  800, 444, 0, 0},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO0,           10, 16, 92, 162,0,8},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO1,           99, 16, 94, 139,0,7},
    { RADIO_CreateIndirect,      NULL,               GUI_ID_RADIO2,           199,16, 84, 80, 0,2},
    { TEXT_CreateIndirect,      "Chanel1",           GUI_ID_TEXT0,            270, 16,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel2",           GUI_ID_TEXT1,            270, 36,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel3",           GUI_ID_TEXT2,            270, 56,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel4",           GUI_ID_TEXT3,            270, 76,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel5",           GUI_ID_TEXT4,            270, 96,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel6",           GUI_ID_TEXT5,            270, 116,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel7",           GUI_ID_TEXT6,            270, 136,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Chanel8",           GUI_ID_TEXT7,            270, 156,48, 16, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT0,            330, 16,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            330, 36,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            330, 56,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT3,            330, 76,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT4,            330, 96,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT5,            330, 116,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT6,            330, 136,100,18, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT7,            330, 156,100,18, 0,0},
    //{ GRAPH_CreateIndirect,      0,                  GUI_ID_GRAPH0,           300,  10, 470, 385, 0, 0},
};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialogFile
*	����˵��: �ļ����������ػ�  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void PaintDialogAD7606(WM_MESSAGE * pMsg)
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
void InitDialogAD7606(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
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
    //GUI_ID_RADIO0
    //
    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO0), &GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel1",0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel2",1);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel3",2);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel4",3);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel5",4);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel6",5);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel7",6);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO0),"Chanel8",7);

    //
    //GUI_ID_RADIO1
    //
    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO1),&GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_NO",  0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_X2",  1);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_X4",  2);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_X8",  3);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_X16", 4);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_X32", 5);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO1), "AD_OS_X64", 6);

    //
    //GUI_ID_RADIO2
    //
    RADIO_SetFont(WM_GetDialogItem(hWin,GUI_ID_RADIO2),&GUI_Font16_ASCII);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO2)," +-5V",0);
    RADIO_SetText(WM_GetDialogItem(hWin,GUI_ID_RADIO2)," +-10V",1);

    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT5),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT6),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT7),&GUI_Font16_ASCII);


    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT0), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT1), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT2), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT3), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT4), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT5), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT6), &GUI_Font16_ASCII);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT7), &GUI_Font16_ASCII); 

    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT0), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT1), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT2), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT3), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT4), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT5), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT6), 0, 0.0001, 10, 4, 0);
    EDIT_SetFloatMode(WM_GetDialogItem(hWin,GUI_ID_EDIT7), 0, 0.0001, 10, 4, 0);


    hItem = WM_GetDialogItem(hWin, GUI_ID_GRAPH0);

	/* Creates a data object of type GRAPH_DATA_YT */
	/* �����ɹ��Ļ����������ݾ�� */
	/* ��ʾ��������ɫ��������ʾ��������ݸ���������ָ�룬Ҫ��ӵ����ݸ��� */
	ahData1[0] = GRAPH_DATA_YT_Create(_aColor[0], 500, 0, 0);
	
	/* Attaches a data object to an existing graph widget. */
	/* Ϊ��ͼ�ؼ�������ݶ��� */
	GRAPH_AttachData(hItem, ahData1[0]);

	GRAPH_SetGridDistY(hItem, 40);
	/* Sets the visibility of the grid lines */
	/* դ���Ƿ�ɼ� */
    GRAPH_SetGridVis(hItem, 1);
	
	/* Fixes the grid in X-axis */ 
	/* �̶�X���դ�� */
    GRAPH_SetGridFixedX(hItem, 1);

	/* Create and add vertical scale */
	/* ���������Ӵ�ֱ��Χ�߶ȱ�ǩ  */
	/* ����ߵĳ߶�λ�ã����뷽ʽ����ֱ��ˮƽ��ǩ����ǩ�ļ�� */
    hScaleV = GRAPH_SCALE_Create( 16, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
	/* �������ñ������� */
	GRAPH_SCALE_SetFactor(hScaleV, 0.05);
	/* ��������С�����������λ�� */
//	GRAPH_SCALE_SetNumDecs(hScaleV,1);
	/* ���ñ�ǩ������ɫ */
    GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);
	/* ����ǩ��ӵ���ֱ���� */
    GRAPH_AttachScale(hItem, hScaleV);

    /* ���������������±߽� */
	GRAPH_SetBorder(hItem,16,0,0,0);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSYS
*	����˵��: SYS�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
static void _cbCallbackAD7606(WM_MESSAGE * pMsg) 
{ 
//    char buf[30];
    int xSize, ySize; 
	int NCode, Id;
    static char Rang = 5, chanel = 0;
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hClient;

	hClient = WM_GetClientWindow(hWin);
	xSize = WM_GetWindowSizeX(hClient);
	ySize = WM_GetWindowSizeY(hClient);
   
    switch (pMsg->MsgId) 
    {
        case WM_TIMER:

           AD7606_ReadNowAdc();	
		   AD7606_StartConvst();

           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT0), (float)(g_tAD7606.sNowAdc[0] * Rang) / 32768 );
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT1), (float)(g_tAD7606.sNowAdc[1] * Rang) / 32768);
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT2), (float)(g_tAD7606.sNowAdc[2] * Rang) / 32768);
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT3), (float)(g_tAD7606.sNowAdc[3] * Rang) / 32768);
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT4), (float)(g_tAD7606.sNowAdc[4] * Rang) / 32768);
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT5), (float)(g_tAD7606.sNowAdc[5] * Rang) / 32768);
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT6), (float)(g_tAD7606.sNowAdc[6] * Rang) / 32768);
           EDIT_SetFloatValue(WM_GetDialogItem(hWin,GUI_ID_EDIT7), (float)(g_tAD7606.sNowAdc[7] * Rang) / 32768);
           
           GRAPH_DATA_YT_AddValue(ahData1[0], (float)(g_tAD7606.sNowAdc[chanel] * Rang * 20) / 32768 + Rang);
           
           WM_RestartTimer(pMsg->Data.v, 100);
	       break;
		
        case WM_DELETE:
		    OnICON2 = 0;
			prevent_refresh = 0;
		    break;
		
		case WM_PAINT:
            GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  xSize - 1,	   /* ���½�X λ�� */
							  ySize - 1,       /* ���½�Y λ�� */
							  GUI_YELLOW,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_RED);        /* �������Ҳ�Ҫ���Ƶ���ɫ */

            PaintDialogAD7606(pMsg);
            break;
		
        case WM_INIT_DIALOG:
            InitDialogAD7606(pMsg);
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
                            chanel = WM_GetDialogItem(hWin,GUI_ID_RADIO0); 
                            break;
                    }
                    break;

                case GUI_ID_RADIO1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            g_tAD7606.ucOS = RADIO_GetValue(WM_GetDialogItem(hWin,GUI_ID_RADIO1));
                            AD7606_SetOS(g_tAD7606.ucOS);
                            break;
                    }
                    break;

                case GUI_ID_RADIO2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                           if(WM_GetDialogItem(hWin,GUI_ID_RADIO2) == 0)
                           {
                                Rang = 5;
                                AD7606_SetInputRange(0);    
                           }
                           else
                           {
                                Rang = 10;
                                AD7606_SetInputRange(1);     
                           }
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
*	�� �� ��: OnICON2Clicked
*	����˵��: ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OnICON2Clicked(void)
{ 	
    WM_HWIN hWin;

    bsp_InitAD7606();
	AD7606_SetOS(AD_OS_NO);	
	AD7606_SetInputRange(0);
	AD7606_StartConvst();	
    	

    hWin = GUI_CreateDialogBox(_aDialogCreateAD7606, 
                                 GUI_COUNTOF(_aDialogCreateAD7606), 
                                 &_cbCallbackAD7606, 
                                 0, 
                                 0, 
                                 0);
    
    WM_CreateTimer(WM_GetClientWindow(hWin),    /* ������Ϣ�Ĵ��ڵľ�� */
                   0, 	                        /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
			       100,                         /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
			       0);	                        /* ��������ʹ�ã�ӦΪ0 */ 
                                                 	
	while(OnICON2)
	{					 				
        GUI_Delay(10); 				
	}
	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


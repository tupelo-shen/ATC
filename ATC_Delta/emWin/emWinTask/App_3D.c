/*
*********************************************************************************************************
*	                                  
*	ģ������ : ����
*	�ļ����� : App_3D.c
*	��    �� : V1.2
*	˵    �� : ���������Ǻ����������
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

#define GUI_ID_TEXT10  GUI_ID_USER + 1

GRAPH_SCALE_Handle hScaleV;         /* Handle of vertical scale */
GRAPH_SCALE_Handle hScaleH;         /* Handle of horizontal scale */

GRAPH_DATA_Handle  ahData1[3];       /* Array of handles for the GRAPH_DATA objects */
GRAPH_DATA_Handle  ahData2[3];       /* Array of handles for the GRAPH_DATA objects */

GUI_COLOR _aColor[] = {GUI_BLUE, 0xff00ff, GUI_GREEN};
/*
*********************************************************************************************************
*							�Ի����ʼ��ѡ��		        
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate3D[] = {
    { FRAMEWIN_CreateIndirect,  "MPU6050 HMC5883L",               0,            0,  0,  800, 444, 0, 0},

    { TEXT_CreateIndirect,      "MPU6050",           GUI_ID_TEXT0,            18, 20, 94, 24, 0,0},
    { TEXT_CreateIndirect,      "Accel_X",           GUI_ID_TEXT1,            18, 71, 64, 16, 0,0},
    { TEXT_CreateIndirect,      "GYRO_Z",            GUI_ID_TEXT6,            18, 206,56, 16, 0,0},
    { TEXT_CreateIndirect,      "GYRO_Y",            GUI_ID_TEXT5,            18, 179,56, 16, 0,0},
    { TEXT_CreateIndirect,      "GYRO_X",            GUI_ID_TEXT4,            18, 152,56, 16, 0,0},
    { TEXT_CreateIndirect,      "Accel_Z",           GUI_ID_TEXT3,            18, 125,64, 16, 0,0},
    { TEXT_CreateIndirect,      "Accel_Y",           GUI_ID_TEXT2,            18, 98, 64, 16, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT0,            89, 69, 100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT5,            89, 204,100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT4,            89, 177,100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT3,            89, 150,100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            89, 123,100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            89, 96, 100,21, 0,0},

    { TEXT_CreateIndirect,      "HMC5883L",          GUI_ID_TEXT7,            250, 20,106,24, 0,0},
    { TEXT_CreateIndirect,      "Mag_X",             GUI_ID_TEXT8,            250, 70,48, 16, 0,0},
    { TEXT_CreateIndirect,      "Mag_Y",             GUI_ID_TEXT9,            250, 98,48, 16, 0,0},    
    { TEXT_CreateIndirect,      "Mag_Z",             GUI_ID_TEXT10,           250, 125,48, 16, 0,0},

    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT6,            320, 70,100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT7,            320, 98,100,21, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT8,            320, 125,100,21, 0,0},

};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialogFile
*	����˵��: �ļ����������ػ�  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void PaintDialog3D(WM_MESSAGE * pMsg)
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
void InitDialog3D(WM_MESSAGE * pMsg)
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
    TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT1), GUI_BLUE);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font16B_ASCII);

    //
    //GUI_ID_TEXT2
    //
    TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT2), 0xff00ff);
    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT2), &GUI_Font16B_ASCII);
    //
    //GUI_ID_TEXT3
    //
    TEXT_SetTextColor(WM_GetDialogItem(hWin, GUI_ID_TEXT3), GUI_GREEN);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font16B_ASCII);
    //
    //GUI_ID_TEXT4
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4),&GUI_Font16B_ASCII);
    //
    //GUI_ID_TEXT5
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT5),&GUI_Font16B_ASCII);
    //
    //GUI_ID_TEXT6
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT6),&GUI_Font16B_ASCII);
    //
    //GUI_ID_TEXT7
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT7),&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT8
    //
    TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT8), GUI_BLUE);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT8),&GUI_Font16B_ASCII);

    //
    //GUI_ID_TEXT9
    //
    TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT9), 0xff00ff);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT9),&GUI_Font16B_ASCII);
    //
    //GUI_ID_TEXT10
    //
    TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT10), GUI_GREEN);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT10),&GUI_Font16B_ASCII);

    //
    //GUI_ID_EDIT0
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT0),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT0),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT0),0,0,65536,0,0);

    //
    //GUI_ID_EDIT1
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT1),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT1),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT1),0,0,65536,0,0);

    //
    //GUI_ID_EDIT2
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT2),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT2),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT2),0,0,65536,0,0);
    //
    //GUI_ID_EDIT3
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT3),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT3),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT3),0,0,65536,0,0);

    //
    //GUI_ID_EDIT4
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT4),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT4),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT4),0,0,65536,0,0);

    //
    //GUI_ID_EDIT5
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT5),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT5),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT5),0,0,65536,0,0);

    //
    //GUI_ID_EDIT6
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT6),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT6),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT6),0,0,65536,0,0);

    //
    //GUI_ID_EDIT7
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT7),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT7),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT7),0,0,65536,0,0);

    //
    //GUI_ID_EDIT8
    //
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT8),&GUI_Font16_ASCII);
    EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT8),GUI_TA_VCENTER|GUI_TA_CENTER);
    EDIT_SetDecMode(WM_GetDialogItem(hWin,GUI_ID_EDIT8),0,0,65536,0,0);

}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSYS
*	����˵��: SYS�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
static void _cbCallback3D(WM_MESSAGE * pMsg) 
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
	       HMC5883L_ReadData();		/* ��ȡ MPU-6050�����ݵ�ȫ�ֱ��� g_tMPU6050 */
//           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT6), g_tHMC5883L.Mag_X);
//           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT7), g_tHMC5883L.Mag_Y);
//           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT8), g_tHMC5883L.Mag_Z);

           MPU6050_ReadData();		/* ��ȡ MPU-6050�����ݵ�ȫ�ֱ��� g_tMPU6050 */
           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT0), g_tMPU6050.Accel_X);
           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT1), g_tMPU6050.Accel_Y);
           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT2), g_tMPU6050.Accel_Z);
           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT3), g_tMPU6050.GYRO_X);
           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT4), g_tMPU6050.GYRO_Y);
           EDIT_SetValue(WM_GetDialogItem(hWin,GUI_ID_EDIT5), g_tMPU6050.GYRO_Z);

           WM_RestartTimer(pMsg->Data.v, 100);
	       break;
        case WM_DELETE:
		    OnICON4 = 0;
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
							  400,	           /* ���½�X λ�� */
							  46,             /* ���½�Y λ�� */
							  GUI_LIGHTBLUE,  /* ���������Ҫ���Ƶ���ɫ */
							  GUI_WHITE);     /* �������Ҳ�Ҫ���Ƶ���ɫ */


            PaintDialog3D(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog3D(pMsg);
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
            }
            break;
    }

	WM_DefaultProc(pMsg);	
}

/*
*********************************************************************************************************
*	�� �� ��: OnICON0Clicked
*	����˵��: �ļ�ϵͳ���������� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OnICON4Clicked(void)
{ 	
    WM_HWIN hWin ;

    hWin = GUI_CreateDialogBox(_aDialogCreate3D, 
                                 GUI_COUNTOF(_aDialogCreate3D), 
                                 &_cbCallback3D, 
                                 0, 
                                 0, 
                                 0); 

    WM_CreateTimer(WM_GetClientWindow(hWin),  /* ������Ϣ�Ĵ��ڵľ�� */
                   0, 	                        /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
			       300,                         /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
			       0);	                        /* ��������ʹ�ã�ӦΪ0 */        	

	while(OnICON4)
	{					 				
        GUI_Delay(10); 				
	}
	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

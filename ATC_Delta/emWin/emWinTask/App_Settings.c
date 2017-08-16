/*
*********************************************************************************************************
*	                                  
*	ģ������ : ϵͳ��Ϣ
*	�ļ����� : MainTask.c
*	��    �� : V1.2
*	˵    �� : ������ʾϵͳ��Ϣ
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
**************************************************************************
*                  				������
**************************************************************************
*/
static const char *TaskProfile1[7]={" "," "," "," "," "," "," "};


/*
*********************************************************************************************************
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSetting[] = {
    { FRAMEWIN_CreateIndirect,  "Task Manage",           0,                       0,  0,  800, 444,0,0},

};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialog
*	����˵��: �ۺ�ʵ��һ�ػغ��� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PaintDialogSetting(WM_MESSAGE * pMsg)
{

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitDialogSetting(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
    //
    //FRAMEWIN
    //
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
	
	FRAMEWIN_SetTextColor(hWin,0x0000ff);
	FRAMEWIN_SetFont(hWin,&GUI_Font24B_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_SetTitleHeight(hWin,28);

}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallback
*	����˵��: �ۺ�ʵ��һ�ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackSetting(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_DELETE:
		    OnICON1 = 0;
			prevent_refresh = 0;
		    break;
		
        case WM_PAINT:
            PaintDialogSetting(pMsg);
            break;
		
        case WM_INIT_DIALOG:
            InitDialogSetting(pMsg);
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
        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*				                         ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateTask[] = {
  { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 800, 444, FRAMEWIN_CF_MOVEABLE },
  { LISTVIEW_CreateIndirect,   NULL,               GUI_ID_LISTVIEW0,        0,  0,  794,421,0,0}
};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialog
*	����˵��: �ۺ�ʵ��һ�ػغ��� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PaintDialogTask(WM_MESSAGE * pMsg)
{

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitDialogTask(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	unsigned char i;
	HEADER_Handle hHeader;
	
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 
				WM_GetWindowOrgX(hWin),  
				WM_GetWindowOrgY(hWin),  
				LCD_GetXSize(),
				LCD_GetYSize());
    //
    //GUI_ID_LISTVIEW0
    //
	hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0));
	HEADER_SetFont(hHeader,&GUI_FontHZ_SimSun_1616);
	HEADER_SetHeight(hHeader,28);

    LISTVIEW_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),0,0x000000);
    LISTVIEW_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),0,0x00ff00);
    LISTVIEW_SetFont(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),&GUI_FontHZ_SimSun_1616);

    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"���ȼ�",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),70,"��ջʹ��",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),70,"��ջʣ��",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),70,"��ջ�ֱ�",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"CPU",GUI_TA_VCENTER|GUI_TA_LEFT);

    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),120,"��������",GUI_TA_VCENTER|GUI_TA_LEFT);
    
// 	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 0, 110);
// 	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 1, 110);
// 	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 2, 110);
// 	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 3, 110);
// 	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 4, 100);
// 	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 5, 240);
	
	for(i = 0; i < OSTaskQty; i++)
	{
		LISTVIEW_AddRow(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), (GUI_ConstString *)TaskProfile1);
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 0, i, "1");		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 1, i, "2");
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 2, i, "3");		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 3, i, "4");
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 4, i, "5");		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 5, i, "6");					
	}
	
//	SCROLLBAR_SetWidth(SCROLLBAR_CreateAttached(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),SCROLLBAR_CF_VERTICAL),16);
//  	LISTVIEW_SetAutoScrollH(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),1);
//     LISTVIEW_SetAutoScrollV(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),1);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallback
*	����˵��: �ۺ�ʵ��һ�ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackTask(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialogTask(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogTask(pMsg);
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
		case WM_DELETE:

		    break;
        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCPU[] = {
   { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 800, 444, FRAMEWIN_CF_MOVEABLE },
   { GRAPH_CreateIndirect,     0,                 GUI_ID_GRAPH0,       0,   0, 794, 421, 0, 0}
};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialog
*	����˵��: �ۺ�ʵ��һ�ػغ��� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PaintDialogCPU(WM_MESSAGE * pMsg)
{

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitDialogCPU(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	unsigned char i;

	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_GRAPH0), 
				WM_GetWindowOrgX(hWin),  
				WM_GetWindowOrgY(hWin),  
				LCD_GetXSize(),
				LCD_GetYSize()-55);
	
  	hItem = WM_GetDialogItem(hWin, GUI_ID_GRAPH0);

	/* Add graphs */
	for (i = 0; i < 1; i++)
	{
		/* Creates a data object of type GRAPH_DATA_YT */
		/* �����ɹ��Ļ����������ݾ�� */
		/* ��ʾ��������ɫ��������ʾ��������ݸ���������ָ�룬Ҫ��ӵ����ݸ��� */
		ahData1[i] = GRAPH_DATA_YT_Create(_aColor[i], 800, 0, 0);
		
		/* Attaches a data object to an existing graph widget. */
		/* Ϊ��ͼ�ؼ�������ݶ��� */
		GRAPH_AttachData(hItem, ahData1[i]);
	}

	GRAPH_SetGridDistY(hItem, 20);
	/* Sets the visibility of the grid lines */
	/* դ���Ƿ�ɼ� */
    GRAPH_SetGridVis(hItem, 1);
	
	/* Fixes the grid in X-axis */ 
	/* �̶�X���դ�� */
    GRAPH_SetGridFixedX(hItem, 1);

	/* Create and add vertical scale */
	/* ���������Ӵ�ֱ��Χ�߶ȱ�ǩ  */
	/* ����ߵĳ߶�λ�ã����뷽ʽ����ֱ��ˮƽ��ǩ����ǩ�ļ�� */
    hScaleV = GRAPH_SCALE_Create( 20, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
	/* �������ñ������� */
	GRAPH_SCALE_SetFactor(hScaleV, 0.5);
	/* ��������С�����������λ�� */
//	GRAPH_SCALE_SetNumDecs(hScaleV,1);
	/* ���ñ�ǩ������ɫ */
    GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);
	/* ����ǩ��ӵ���ֱ���� */
    GRAPH_AttachScale(hItem, hScaleV);
	
//    /* Create and add horizontal scale */
//	/* ����������ˮƽ��Χ�߶ȱ�ǩ */
//    hScaleH = GRAPH_SCALE_Create(212, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 25);
//	/* ����������ɫ */
//    GRAPH_SCALE_SetTextColor(hScaleH, GUI_DARKGREEN);
//    /* ��ӵ�ˮƽ���� */
//    GRAPH_AttachScale(hItem, hScaleH);

    /* ���������������±߽� */
	GRAPH_SetBorder(hItem,25,0,0,10);

}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallback
*	����˵��: �ۺ�ʵ��һ�ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackCPU(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialogCPU(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogCPU(pMsg);
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
		case WM_DELETE:

		    break;
        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateTarget[] = {
   { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 800, 444, FRAMEWIN_CF_MOVEABLE },
   { MULTIEDIT_CreateIndirect, "MULTIEDIT",       GUI_ID_MULTIEDIT0,   0,   0,  794,421,0,0},
};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialog
*	����˵��: �ۺ�ʵ��һ�ػغ��� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PaintDialogTarget(WM_MESSAGE * pMsg)
{

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitDialogTarget(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 
				WM_GetWindowOrgX(hWin),  
				WM_GetWindowOrgY(hWin),  
				LCD_GetXSize(),
				LCD_GetYSize());

	MULTIEDIT_SetAutoScrollV(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1);
	MULTIEDIT_SetAutoScrollH(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1);   
	MULTIEDIT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1,0x000000);
    MULTIEDIT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1,0x00ff00);
    MULTIEDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),&GUI_FontHZ_SimSun_1616);
	MULTIEDIT_SetWrapWord(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0));
	MULTIEDIT_SetReadOnly(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1);
	MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0), "���ܽ������ȼ���ջʹ��ʣ�������ʵ��һĿ��");
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallback
*	����˵��: �ۺ�ʵ��һ�ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackTarget(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialogTarget(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogTarget(pMsg);
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
		case WM_DELETE:
            OnICON1 = 0;
		    break;
        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: MainTask
*	����˵��: ������ 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OnICON1Clicked(void)
{ 
    WM_HWIN  hDialog; 
	WM_HWIN hMultiPage, hDialogTemp;
	OS_TCB      *p_tcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	float CPU;
	char buf[20],i;
	CPU_SR_ALLOC();

    hDialog = GUI_CreateDialogBox(_aDialogCreateSetting, 
                                  GUI_COUNTOF(_aDialogCreateSetting), 
                                  &_cbCallbackSetting, 
                                  0, 
                                  0, 
                                  0);

	hMultiPage = MULTIPAGE_CreateEx(0, 0, WM_GetWindowSizeX(WM_GetClientWindow(hDialog)), WM_GetWindowSizeY(WM_GetClientWindow(hDialog)), WM_GetClientWindow(hDialog), WM_CF_SHOW|WM_CF_MEMDEV, 0, 0);

	MULTIPAGE_SetFont(hMultiPage, &GUI_FontHZ_SimSun_1616);		

	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateTask, GUI_COUNTOF(_aDialogCreateTask), &_cbCallbackTask, WM_UNATTACHED, 0, 0);   		
	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "����");

	
	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateCPU, GUI_COUNTOF(_aDialogCreateCPU), &_cbCallbackCPU, WM_UNATTACHED, 0, 0);  		
	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "����");	
	

	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateTarget, GUI_COUNTOF(_aDialogCreateTarget), &_cbCallbackTarget, WM_UNATTACHED, 0, 0);  		
	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "ʵ��Ŀ��");	

	
	while(OnICON1)
	{
		if(MULTIPAGE_GetSelection(hMultiPage) == 0)
		{
			CPU_CRITICAL_ENTER();
			p_tcb = OSTaskDbgListPtr;
			i = 0;
			/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
			while (p_tcb != (OS_TCB *)0) 
			{
				CPU = (float)p_tcb->CPUUsage / 100;
				sprintf(buf, "%d", p_tcb->Prio);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 0), GUI_ID_LISTVIEW0), 0, i, buf);
				sprintf(buf, "%d", p_tcb->StkUsed);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 0), GUI_ID_LISTVIEW0), 1, i, buf);
				sprintf(buf, "%d", p_tcb->StkFree);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 0), GUI_ID_LISTVIEW0), 2, i, buf);
				sprintf(buf, "%d%%", (p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree));
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 0), GUI_ID_LISTVIEW0), 3, i, buf);
				sprintf(buf, "%5.2f%%", CPU);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 0), GUI_ID_LISTVIEW0), 4, i, buf);
				sprintf(buf, "%s", p_tcb->NamePtr);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 0), GUI_ID_LISTVIEW0), 5, i, buf);
											 	
			    p_tcb = p_tcb->DbgNextPtr;
				i++;
			}
			CPU_CRITICAL_EXIT();	
		}
		else if(MULTIPAGE_GetSelection(hMultiPage) == 1)
		{
			GRAPH_DATA_YT_AddValue(ahData1[0], OSStatTaskCPUUsage/20);
		}
							
		GUI_Delay(200);	
	}

}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

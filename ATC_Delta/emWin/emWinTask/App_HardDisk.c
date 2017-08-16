/*
*********************************************************************************************************
*	                                  
*	ģ������ : �ļ�����
*	�ļ����� : App_FileManage.c
*	��    �� : V1.2
*	˵    �� : �ļ�ϵͳ����֧��SD, NANDFLASH, U��
*	�޸ļ�¼ :
*		�汾��    ����          ����         ˵��
*		v1.0    2013-04-26    Eric2013      �׷�
*       v1.1    2014-05-23    Eric2013      1. ����������ͬʱ֧��4.3�磬5���7��������ʾ
*                                           2. ɾ��MENU�ؼ�
*       v1.2    2014-07-19    Eric2013      1. ���������ʵ�ֿ��ٵı���ˢ��
*                                            
*	
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"

/*
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/ 
#define File_DropDownHeight    30   /* �ؼ�dropdown�ĸ߶� */
#define File_ProgBarStartX     55   /* �ؼ�progbar����ʼx */
#define File_ProgBarStartY     70   /* �ؼ�progbar����ʼy */
#define File_ProgBarWidth      170  /* �ؼ�progbar�Ŀ��  */
#define File_ProgBarHeight     25   /* �ؼ�progbar�ĸ߶�  */
#define File_ProgBarCap        30   /* �ؼ�progbar�ļ��  */

#define Imag_StartX  11   /* ͼ�����ʼX */
#define Imag_StartY  35   /* ͼ�����ʼY */
#define Imag_YCap    30   /* ͼ��ļ�� */

/*
*********************************************************************************************************
*	�� �� ��: Ext_GetFree
*	����˵��: �õ�����ʣ������
*	��    �Σ� drv    ���̱��("0:"/"1:")
*              total  ����������λKB��
*              free   ʣ������	 ����λKB��
*	�� �� ֵ: 0,����.����,�������		        
*********************************************************************************************************
*/
uint8_t Ext_GetFree(uint8_t *drv, uint32_t *total, uint32_t *free)
{
	FATFS *FS;
	uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv, (DWORD *)&fre_clust, &FS);
    if(res==0)
	{											   
	    tot_sect = (FS->n_fatent - 2)* FS->csize;	//�õ���������
	    fre_sect = fre_clust * FS->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				    //������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect *= FS->ssize / 512;
		fre_sect *= FS->ssize / 512;
#endif	  
		*total = tot_sect>>1;	//��λΪKB
		*free = fre_sect>>1;	//��λΪKB 
 	}
	return res;
}

/*
*********************************************************************************************************
*							�Ի����ʼ��ѡ��		        
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateFile[] = {
    { FRAMEWIN_CreateIndirect,  "File Manage",               0,               0,  0,  800, 444, 0, 0},
	{ DROPDOWN_CreateIndirect,   NULL,               GUI_ID_DROPDOWN0,        0,  0,  790, 90, 0,0},
	{ TEXT_CreateIndirect,      "HardDisk",          GUI_ID_TEXT0,            15, 36, 106,24, 0,0},
    { PROGBAR_CreateIndirect,    NULL,               GUI_ID_PROGBAR0,         0, 0, 0, 0, 0, 0},
    { PROGBAR_CreateIndirect,    NULL,               GUI_ID_PROGBAR1,         0, 0, 0, 0, 0, 0},
    { PROGBAR_CreateIndirect,    NULL,               GUI_ID_PROGBAR2,         0, 0, 0, 0, 0, 0},

 	{ TEXT_CreateIndirect,      "OPEN USB HOST",     GUI_ID_TEXT1,        15, 160, 200,24, 0,0},
    { BUTTON_CreateIndirect,    "OPEN HOST",     GUI_ID_BUTTON0,          10, 200,100,28, 0,0},
	{ BUTTON_CreateIndirect,    "CLOSE HOST",    GUI_ID_BUTTON1,          120,200,100,28, 0,0},
	{ BUTTON_CreateIndirect,    "OPEN DEVICE",   GUI_ID_BUTTON2,          230,200,100,28, 0,0},
    { BUTTON_CreateIndirect,    "CLOSE DEVICE",  GUI_ID_BUTTON3,          340,200,100,28, 0,0},
};

/*
*********************************************************************************************************
*	�� �� ��: PaintDialogFile
*	����˵��: �ļ����������ػ�  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void PaintDialogFile(WM_MESSAGE * pMsg)
{

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialogFile
*	����˵��: ��ʼ���Ի���  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void InitDialogFile(WM_MESSAGE * pMsg)
{
    char buf[25];
    WM_HWIN hWin = pMsg->hWin;
	uint32_t DiskFree, DiskTotal;
	WM_HWIN hClient;
	
	/* �������öԻ������ʾλ�úʹ�С */
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
	
	
	hClient = WM_GetClientWindow(hWin);
	
	/* ���ÿؼ�DROPDOWN0��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), 
					WM_GetWindowOrgX(hClient),  
					WM_GetWindowOrgY(hClient),  
					LCD_GetXSize() - 10,
					File_DropDownHeight);
	
	/* ���ÿؼ�PROGBAR0��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 
					WM_GetWindowOrgX(hClient) + File_ProgBarStartX,  
					WM_GetWindowOrgY(hClient) + File_ProgBarStartY,  
					File_ProgBarWidth,
					File_ProgBarHeight);
					
	/* ���ÿؼ�PROGBAR1��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR1), 
					WM_GetWindowOrgX(hClient) + File_ProgBarStartX,  
					WM_GetWindowOrgY(hClient) + File_ProgBarStartY + File_ProgBarCap,  
					File_ProgBarWidth,
					File_ProgBarHeight);
					
	/* ���ÿؼ�PROGBAR2��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR2), 
					WM_GetWindowOrgX(hClient) + File_ProgBarStartX,  
					WM_GetWindowOrgY(hClient) + File_ProgBarStartY + File_ProgBarCap*2,  
					File_ProgBarWidth,
					File_ProgBarHeight);
    //
    //FRAMEWIN
    //
	FRAMEWIN_SetFont(hWin, &GUI_Font24B_ASCII);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
	
	//
    //GUI_ID_DROPDOWN0
    //
	DROPDOWN_SetFont(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), &GUI_Font20_ASCII);
    DROPDOWN_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0),0,0xffffff);
    DROPDOWN_SetAutoScroll(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0),1);
    DROPDOWN_AddString(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), "Open none");
    DROPDOWN_AddString(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), "Open sd flash");
    DROPDOWN_AddString(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), "Open nand flash");
    DROPDOWN_AddString(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), "Open u flash");

	//
    //GUI_ID_PROGBAR0  NANDFLASH
    //
    Ext_GetFree("1:", &DiskTotal, &DiskFree);
	sprintf(buf, "Used: %dMB  Total: %dMB", (DiskTotal - DiskFree)>>10, DiskTotal>>10);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 0, DiskTotal>>10);
	PROGBAR_SetText(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), buf);
    PROGBAR_SetValue(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), (DiskTotal - DiskFree)>>10);
    //
    //GUI_ID_PROGBAR1  USB
    //
    Ext_GetFree("2:", &DiskTotal, &DiskFree);
	sprintf(buf, "Used: %dMB  Total: %dMB", (DiskTotal - DiskFree)>>10, DiskTotal>>10);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin,GUI_ID_PROGBAR1), 0, DiskTotal>>10);
	PROGBAR_SetText(WM_GetDialogItem(hWin,GUI_ID_PROGBAR1), buf);
    PROGBAR_SetValue(WM_GetDialogItem(hWin,GUI_ID_PROGBAR1), (DiskTotal - DiskFree)>>10);
    
	//
    //GUI_ID_PROGBAR0  SD
    //
	Ext_GetFree("0:", &DiskTotal, &DiskFree);
	sprintf(buf, "Used: %dMB  Total: %dMB", (DiskTotal - DiskFree)>>10, DiskTotal>>10);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin,GUI_ID_PROGBAR2), 0, DiskTotal>>10);
	PROGBAR_SetText(WM_GetDialogItem(hWin,GUI_ID_PROGBAR2), buf);
    PROGBAR_SetValue(WM_GetDialogItem(hWin,GUI_ID_PROGBAR2), (DiskTotal - DiskFree)>>10);

	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font24B_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font24B_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font24B_ASCII);
	
	
	BUTTON_SetFocussable(WM_GetDialogItem(hWin,GUI_ID_BUTTON0), 0);
	BUTTON_SetFocussable(WM_GetDialogItem(hWin,GUI_ID_BUTTON1), 0);
	BUTTON_SetFocussable(WM_GetDialogItem(hWin,GUI_ID_BUTTON2), 0);
	BUTTON_SetFocussable(WM_GetDialogItem(hWin,GUI_ID_BUTTON3), 0);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSYS
*	����˵��: SYS�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
static void _cbCallbackFile(WM_MESSAGE * pMsg) 
{
	uint8_t ucSelID = 0;
	int xSize, ySize; 
	int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hClient;
	hClient = WM_GetClientWindow(hWin);
	xSize = WM_GetWindowSizeX(hClient);
	ySize = WM_GetWindowSizeY(hClient);
   
    switch (pMsg->MsgId) 
    {
        case WM_DELETE:
		    OnICON0 = 0;
			prevent_refresh = 0;
		    break;
		
		case WM_MOVE:
			prevent_refresh = 0;
			break;
		
		case WM_PAINT:
			/* ������ˮƽ��ɫ�ݶ����ľ��� */
			GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  xSize - 1,	   /* ���½�X λ�� */
							  ySize - 1,  	   /* ���½�Y λ�� */
							  GUI_WHITE,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_LIGHTBLUE);  /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  60,			   /* ���Ͻ�Y λ�� */
							  400,	           /* ���½�X λ�� */
							  61,              /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  185,			   /* ���Ͻ�Y λ�� */
							  400,	           /* ���½�X λ�� */
							  186,             /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawBitmap(&bmfiledisk, Imag_StartX, WM_GetWindowOrgY(hClient) + Imag_StartY);
			GUI_DrawBitmap(&bmfileusb,  Imag_StartX, WM_GetWindowOrgY(hClient) + Imag_StartY + Imag_YCap);
 			GUI_DrawBitmap(&bmfilesd,   Imag_StartX, WM_GetWindowOrgY(hClient) + Imag_StartY + Imag_YCap*2);
            break;
			
        case WM_INIT_DIALOG:
            InitDialogFile(pMsg);
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
							ucSelID = DROPDOWN_GetSel(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0));
							OnICON00 = 1;
							if(ucSelID != 0)
							{
								OnICON00Clicked(ucSelID);	
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
					
                /* ��ʼ��USB HOST */
				case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            #ifdef USE_USB_OTG_FS
                        		USBH_Init(&USB_OTG_Core,
                        			USB_OTG_FS_CORE_ID,
                                    &USB_Host,
                                    &USBH_MSC_cb,
                                    &USR_USBH_MSC_cb);
                        	#else
                        		USBH_Init(&USB_OTG_Core,
                        			USB_OTG_HS_CORE_ID,
                                    &USB_Host,
                                    &USBH_MSC_cb,
                                    &USR_USBH_MSC_cb);
                        	#endif
                            g_ucState = USB_HOST;
                            TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3), "OPEN USB HOST");
                            break;
                    }
                    break;
					
                /*  �ر�USB HOST */
                case GUI_ID_BUTTON1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                             USBH_DeInit(&USB_OTG_Core, &USB_Host);
                             USB_OTG_StopHost(&USB_OTG_Core);
                      
                             /* Manage User disconnect operations*/
                             USB_Host.usr_cb->DeviceDisconnected();                                          
                              
                             /* Re-Initilaize Host for new Enumeration */
                             USBH_DeInit(&USB_OTG_Core, &USB_Host);
                    
                             USB_Host.usr_cb->DeInit();
                             USB_Host.class_cb->DeInit(&USB_OTG_Core, &USB_Host.device_prop);
                             g_ucState = USB_IDLE;
                            TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3), "CLOSE USB HOST");
                            break;
                    }
                    break;
					
                /* ��ʼ��USB DEVICE */
                case GUI_ID_BUTTON2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            USBD_Init(&USB_OTG_Core,
                            #ifdef USE_USB_OTG_FS
                                   USB_OTG_FS_CORE_ID,
                            #elif defined USE_USB_OTG_HS
                                   USB_OTG_HS_CORE_ID,
                            #endif 
                                   &USR_desc,
                                   &USBD_MSC_cb, 
                                   &USR_cb);
                            g_ucState = USB_DEVICE;
                            TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3), "OPEN USB DEVICE");
                            break;
                    }
                    break;
					
                /* �ر�USB DEVICE */
                case GUI_ID_BUTTON3:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            DCD_DevDisconnect (&USB_OTG_Core);
                            USB_OTG_StopDevice(&USB_OTG_Core);
                            TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3), "CLOSE USB DEVICE");
                            break;
                    }
                    break;
					
			    case GUI_ID_SLIDER0:
                   switch(NCode)
                   {
                       case WM_NOTIFICATION_VALUE_CHANGED:
                          
						   break;
                   }
                   break;
            }
            break;
    }

	WM_DefaultProc(pMsg);	
}

/*
*********************************************************************************************************
*	�� �� ��: OnICON0Clicked
*	����˵��: ������
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OnICON0Clicked(void)
{ 	
	DROPDOWN_SetDefaultFont(&GUI_Font16_ASCII);
	GUI_CreateDialogBox(_aDialogCreateFile, GUI_COUNTOF(_aDialogCreateFile), &_cbCallbackFile, 0, 0, 0); 
	
	while(OnICON0)
	{
		GUI_Delay(10);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

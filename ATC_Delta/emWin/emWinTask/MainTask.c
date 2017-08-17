/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI����������
*	�ļ����� : MainTask.c
*	��    �� : V1.2d
*	˵    �� : �ۺ�ʵ�����������
*	�޸ļ�¼ :
*		�汾��    ����          ����        ˵��
*		v1.0    2013-04-26    Eric2013      �׷�
*       v1.1    2014-05-23    Eric2013      1. ����������ͬʱ֧��4.3�磬5���7��������ʾ
*                                           2. ��ʱɾ����̬LOGO����ʾ������汾�ټ���
*       V1.2    2014-07-19    Eric2013      1. ���ICON�ؼ���Ҫ������βſ��Խ�������⡣
*                                           2. ɾ��FLASH�Ľ���ʹ���У׼����
*       V1.2b   2014-12-05    Eric2013      1. ��ʱɾ������ͼƬ����ʾ
*                                           2. �����ĵ㴥��У׼
*       V1.2c   2015-05-10 	  Eric2013      1. �����̼��⵽V1.5.0
*       								    2. ����BSP�弶֧�ְ� 
*      									    3. ����fatfs��0.11
*       								    4. ����STemWin��5.26
*       								    5. ���7��800*480�ֱ��ʵ�����֧�֣����3.5��480*320��ILI9488֧�֡�
*      									    6. ��Ӧ��App_FileBrowse��ͨ������CHOOSEFILE_SetButtonText�޸İ�����С���Ա���ݴ��������Ե������
*	    V1.2d  2015-12-18  Eric2013         1. ����BSP�弶֧�ְ� 
*                                           2. ����STemWin��5.28
*                                           3. ���4.3���5�������֧�֡�       
*	
*	Copyright (C), 2015-2020, ���������� www.armfly.com
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
#define ICONVIEW_TBorder   10   /* �ؼ�ICONVIEW���ϱ߾� */
#define ICONVIEW_LBorder   10   /* �ؼ�ICONVIEW����߾� */
#define ICONVIEW_RBorder   10   /* �ؼ�ICONVIEW���ұ߾� */
#define ICONVIEW_BBorder   36   /* �ؼ�ICONVIEW���±߾� */   
#define ICONVIEW_Width     70   /* �ؼ�ICONVIEW�Ŀ� */  
#define ICONVIEW_Height    85   /* �ؼ�ICONVIEW�ĸߣ�����ͼ���Y������ */  
#define ICONVIEW_YSpace    5    /* �ؼ�ICONVIEW��Y������ */  
#define ICONVIEW_ImagNum   20   /* �ؼ�ICONVIEW�е�ͼ����� */ 

#define Imag_LBorder     2   /* ͼ�����߾� */
#define Imag_TBorder     2   /* ͼ����ϱ߾� */
#define Time_Width       90  /* ��ʾʱ��Ŀ�� */
#define ImagStart_Width  40  /* ��ʼͼ��Ŀ�� */
#define ImagSpeak_Width  40  /* ����ͼ��Ŀ�� */
#define ImagSignal_Width 40  /* �ź�ͼ��Ŀ�� */
#define ImagTime_Width   210 /* ���濪ʼͼ�꣬����ͼ�꣬�ź�ͼ���Լ�ʱ����ʾ���ܿ��40+40+40+90=210*/

#define Time_Line1   1   /* ��һ��ʱ����ʾ��Y���� */
#define Time_Line2   17  /* �ڶ���ʱ����ʾ��Y���� */
#define Time_Height  16  /* ʱ����ʾ�ĸ߶� */

/*
*********************************************************************************************************
*                                      ����
*********************************************************************************************************
*/ 
WM_HWIN hWin;           /* ICONVIEW�ľ�� */
WM_HWIN hWinTaskBar;    /* ��������� */
uint8_t OnICON0  = 0;   /* ICONVIEW�ؼ����µı�־��0��ʾδ���£�1��ʾ���� */
uint8_t OnICON00 = 0;
uint8_t OnICON1  = 0;
uint8_t OnICON2  = 0;
uint8_t OnICON3  = 0;
uint8_t OnICON4  = 0;
uint8_t OnICON5  = 0;
uint8_t OnICON6  = 0;
uint8_t OnICON7  = 0;
uint8_t OnICON8  = 0;
uint8_t OnICON9  = 0;
uint8_t OnICON10  = 0;
uint8_t OnICON11  = 0;
uint8_t OnICON12  = 0;
uint8_t OnICON13  = 0;
uint8_t OnICON14  = 0;
uint8_t OnICON15  = 0;
uint8_t OnICON16  = 0;
uint8_t OnICON17  = 0;
uint8_t OnICON18  = 0;
uint8_t OnICON19  = 0;

uint8_t g_ucState = USB_HOST;            /* Ĭ��ѡ��USB HOSTģʽ       */

/* �������ڵ���ʾ */
const char ucWeekDay[7][3] =
{
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

/* ��������ICONVIEWͼ��Ĵ��� */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

/* ��������ICONVIEWͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem[] = {
  {&bmComputer, "Computer"},
  {&bmSettings,  "Settings"},
  {&bmAD7606,    "AD7606" },
  {&bmPicture,   "Picture" },

  {&bm3D,        "3D"},
  {&bmBluetooth,  "Bluetooth"},
  {&bmCamera,    "Camera" },
  {&bmClock,   "Clock" },

  {&bmFMAM, "FM/AM"},
  {&bmGPS,  "GPS"},
  {&bmMP3,    "MP3" },
  {&bmNet,   "Net" },

  {&bmRecorder, "Recorder"},
  {&bmSensor,  "Sensor"},
  {&bmText,    "Text" },
  {&bmUSB,   "USB" },

  {&bmVedio, "Video"},
  {&bmWIFI,  "WIFI"},
  {&bmWireless,    "Wireless" },
  {&bmSignal,   "Signal" },
};

/*
*********************************************************************************************************
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMain[] = {
    { WINDOW_CreateIndirect,     "",      0,                       0, 0, 0, 0, 0},
	{ PROGBAR_CreateIndirect,    NULL,    GUI_ID_PROGBAR0,         0, 0, 0, 0, 0, 0},
	{ TEXT_CreateIndirect,       "",      GUI_ID_TEXT0,            0, 0, 0, 0, 0, 0},
    { TEXT_CreateIndirect,       "",      GUI_ID_TEXT1,            0, 0, 0, 0, 0, 0},
};


/*
*********************************************************************************************************
*	�� �� ��: File_Init
*	����˵��: �ļ�ϵͳ��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void File_Init(void)
{
	/* �����Ժ��޸�ͼ���λ�� */
	 /* ��ʼ��U�� */
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
		
    result = f_mount(&fs_nand, "1:/", 0);
	if (result != FR_OK)
	{
		return;
	}

	result = f_mount(&fs_usb, "2:/", 0);
	if (result != FR_OK)
	{
		return;
	}
    
	result = f_mount(&fs, "0:/", 0);
	if (result != FR_OK)
	{
		return;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Caculate_RTC
*	����˵��: ��ʾRTCʱ��
*	��    �Σ�pMsg ָ�����            
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
static void Caculate_RTC(WM_MESSAGE * pMsg)
{
    char buf[30];
    WM_HWIN hWin = pMsg->hWin;

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

    sprintf(buf, 
          "%0.2d:%0.2d:%0.2d %0.3s", 
          RTC_TimeStructure.RTC_Hours, 
          RTC_TimeStructure.RTC_Minutes,
          RTC_TimeStructure.RTC_Seconds, 
          ucWeekDay[RTC_DateStructure.RTC_WeekDay-1]);
    TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1), buf);

            
    sprintf(buf, 
          "20%0.2d/%0.2d/%0.2d", 
          RTC_DateStructure.RTC_Year, 
          RTC_DateStructure.RTC_Month, 
          RTC_DateStructure.RTC_Date);
    TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), buf); 			  	          
}

/*
*********************************************************************************************************
*	�� �� ��: PaintDialogMain
*	����˵��: ���������ػ溯�� 
*	��    �Σ�pMsg ָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PaintDialogMain(WM_MESSAGE * pMsg)
{
	GUI_SetBkColor(GUI_WHITE); 
    GUI_Clear(); 
	
	/* ��������Сͼ�� */
	GUI_DrawBitmap(&bmStart, 
	               Imag_LBorder, 
	               Imag_TBorder);
	
	/* ��������Сͼ�� */
	GUI_DrawBitmap(&bmSpeaker, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width + Imag_LBorder, 
	               Imag_TBorder);
	
	/* �����ź�Сͼ�� */
	GUI_DrawBitmap(&bmsignal, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width - ImagSignal_Width  + Imag_LBorder, 
	               Imag_TBorder);	
}

/*
*********************************************************************************************************
*	�� �� ��: InitDialogMain
*	����˵��: �Ի����ʼ��
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitDialogMain(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin; 
	WM_HWIN hClient;
	
	hClient = WM_GetClientWindow(hWin);
	
	/* ����������ʾλ�� */
	WM_SetWindowPos(hWin, 
	                0, 
	                LCD_GetYSize() - ICONVIEW_BBorder, 
               	    LCD_GetXSize(),
	                ICONVIEW_BBorder);
	
	/* ���ÿؼ�GUI_ID_PROGBAR0��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 
					WM_GetWindowOrgX(hClient)+ImagStart_Width,  
					WM_GetWindowOrgY(hClient),  
					LCD_GetXSize()-ImagTime_Width,
					ICONVIEW_BBorder);
	
	/* ���ÿؼ�GUI_ID_TEXT0��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_TEXT0), 
					WM_GetWindowOrgX(hClient)+LCD_GetXSize()-Time_Width,  
					WM_GetWindowOrgY(hClient)+Time_Line1,  
					Time_Width,
					Time_Height);
	
	/* ���ÿؼ�GUI_ID_TEXT1��λ�� */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_TEXT1), 
					WM_GetWindowOrgX(hClient)+LCD_GetXSize()-Time_Width,  
					WM_GetWindowOrgY(hClient)+Time_Line2,  
					Time_Width,
					Time_Height);
	
    //
    //	FRAMEWIN
    //
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0),0,10000);
	//
    //	GUI_ID_TEXT0
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),0xffffff);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font16B_ASCII);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT0),GUI_TA_VCENTER|GUI_TA_CENTER);
    //
    //	GUI_ID_TEXT0
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT1),0xffffff);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font16B_ASCII);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT1),GUI_TA_VCENTER|GUI_TA_CENTER);

	Caculate_RTC(pMsg);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackMain
*	����˵��: �ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackMain(WM_MESSAGE * pMsg) {
  WM_HWIN hWin;
  int NCode, Id;
  float CPU;
  char buf[20];

  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	case WM_TIMER:
	       PROGBAR_SetValue(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), OSStatTaskCPUUsage);
		   CPU = (float)OSStatTaskCPUUsage / 100;
	       sprintf(buf, "CPU:%5.2f%%", CPU);
	       PROGBAR_SetText(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), buf);
		   Caculate_RTC(pMsg);
		   WM_RestartTimer(pMsg->Data.v, 1000);
	       break;
    case WM_PAINT:
            PaintDialogMain(pMsg);
            break;
    case WM_INIT_DIALOG:
        InitDialogMain(pMsg);
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
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڵĻص����� 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
__IO uint8_t prevent_refresh = 0;  /* ��������Ƚ���Ҫ�����ڷ�ֹICON�ؼ������ʱ�ʹ����ص�������WM_PAINT��Ϣ*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
		Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
		NCode = pMsg->Data.v;                 /* Notification code */
		switch (Id) 
		{
			case GUI_ID_ICONVIEW0:
				switch (NCode) 
				{
					/* ICON�ؼ������Ϣ */
					case WM_NOTIFICATION_CLICKED:
						prevent_refresh = 1;
						break;
					
					case WM_NOTIFICATION_SCROLL_CHANGED:
						prevent_refresh = 0;
						break;
					
					/* ICON�ؼ��ͷ���Ϣ */
					case WM_NOTIFICATION_RELEASED: 
						
					    /* ����Ӧѡ�� */
						switch(ICONVIEW_GetSel(pMsg->hWinSrc))
						{
							/* �ҵĵ��Խ���*******************************************************************/
                            case 0:	
								OnICON0 = 1;
								OnICON0Clicked();			
								break;	
                            
                            /* ���ý���***********************************************************************/
                            case 1:
                                OnICON1 = 1;
								OnICON1Clicked();
								break;
                            
                            /* AD7606����*********************************************************************/
                            case 2:
							    OnICON2 = 1;
								OnICON2Clicked();	
								break;
                            
                            /* ͼƬ�������********************************************************************/
                            case 3:
                                WM_HideWindow(hWin);
                                WM_HideWindow(hWinTaskBar);
								GUI_SetBkColor(GUI_BLUE); 
								GUI_Clear(); 
							
								OnICON3 = 1;
								OnICON3Clicked();
								prevent_refresh = 0;
							
                                WM_SetCallback(WM_HBKWIN, _cbBkWindow);
                                WM_ShowWindow(hWinTaskBar);
								WM_ShowWindow(hWin);
								break;
                            
                            /* 3D����**********************************************************************/
                            case 4:
                                OnICON4 = 1;
								OnICON4Clicked();
								break;
                            
                            /* ��������********************************************************************/
                            case 5:
								break;
                            
                            /* ����ͷ����******************************************************************/
                            case 6:      
							    WM_HideWindow(hWin);
                                WM_HideWindow(hWinTaskBar);
								GUI_SetBkColor(GUI_BLUE); 
								GUI_Clear(); 
							
								OnICON6 = 1;
								OnICON6Clicked();
								prevent_refresh = 0;
							
                                WM_SetCallback(WM_HBKWIN, _cbBkWindow);
								WM_ShowWindow(hWin);
                                WM_ShowWindow(hWinTaskBar);	
								break;
                            
							/* ʱ�ӽ���*******************************************************************/
                            case 7:
								break;
                            
                            /* FM/AM����******************************************************************/
                            case 8:
                                OnICON8 = 1;
								OnICON8Clicked();
								break;
                             
                             /* GPS����******************************************************************/
                            case 9:
								break;
                             
                             /* MP3����******************************************************************/
                            case 10:
								break;
                             
                            /* NET����*******************************************************************/
                            case 11:
								OnICON11 = 1;
								OnICON11Clicked();
								break;
                            
							/* ¼����*******************************************************************/
                            case 12:
                                OnICON12 = 1;
								OnICON12Clicked();
								break;
                            
							/* ����������***************************************************************/
                            case 13:
                                OnICON13 = 1;
								OnICON13Clicked();
								break;

                            /* �ı�����****************************************************************/
                            case 14:
								break;
                            
                            /* USB********************************************************************/
                            case 15:
								break;
                            
							/* Video����**************************************************************/
                            case 16:
								break;
                            
							/* WIFI����***************************************************************/
                            case 17:
								break;
                            
                            /* wireless����**********************************************************/
                            case 18:
								break;
							
                            /* signal����************************************************************/
                            case 19:
								break;								
						}	
					 break;
				}
			break;
		}
		break;
		
		/* �ػ���Ϣ*/
		case WM_PAINT:
			if(g_LcdWidth == 800)
			{
				if(prevent_refresh == 0)
				{ 
					GUI_SetBkColor(GUI_BLUE);
					GUI_Clear();
					GUI_SetFont(&GUI_FontHZ_SimSun_16);
					GUI_SetColor(GUI_WHITE);
	 				GUI_DispStringHCenterAt("����K2���ڴ���У׼,����������У׼", LCD_GetXSize()/2, LCD_GetYSize() - 54);
					prevent_refresh = 1;
				}	
			}
			else
			{
				GUI_SetBkColor(GUI_BLUE);
				GUI_Clear();
				GUI_SetFont(&GUI_FontHZ_SimSun_16);
				GUI_SetColor(GUI_WHITE);
				GUI_DispStringHCenterAt("����K2���ڴ���У׼,����������У׼", LCD_GetXSize()/2, LCD_GetYSize() - 54);
			}
			
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}
							   
/*
*********************************************************************************************************
*	�� �� ��: MainTask
*	����˵��: GUI������ 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MainTask(void) 
{	
	uint8_t i;
	
	OS_ERR      err;
	CPU_TS		ts;
	OS_MSG_SIZE	msg_size;
	uint8_t	   *p_msg;
	
	uint16_t ICONVIEW_HNum = 0;  /* ICONVIEW��ˮƽ�����ͼ���� */
	uint16_t ICONVIEW_VNum = 0;  /* ICONVIEW�ڴ�ֱ�����ͼ���� */
	
	prevent_refresh = 0; 

	/* �ļ�ϵͳ��ʼ�� *********************************************************/
    File_Init();

	/* ����Ƥ��ɫ *************************************************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	
	/* 
	 * �����еĴ������Զ���ʹ���ڴ��豸����GUI_Init����ò�����
	 * �������ڣ����洰�ڣ���֮ǰ���òŰ����������� 
	 */
 	WM_SetCreateFlags(WM_CF_MEMDEV);			
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	
	/* ����ICONVIEW����ʾλ�� ********************************************************************/
	ICONVIEW_VNum = (LCD_GetYSize() - ICONVIEW_TBorder - ICONVIEW_BBorder) / ICONVIEW_Height;
	ICONVIEW_HNum = (LCD_GetXSize() - ICONVIEW_LBorder - ICONVIEW_RBorder) / ICONVIEW_Width;
	
	/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
	hWin = ICONVIEW_CreateEx(ICONVIEW_TBorder, 					/* С���ߵ��������أ��ڸ������У�*/
						     ICONVIEW_LBorder, 					/* С���ߵ��������أ��ڸ������У�*/
							 ICONVIEW_HNum * ICONVIEW_Width,    /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
							 ICONVIEW_VNum * ICONVIEW_Height, 	/* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
	                         WM_HBKWIN, 				        /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
	                         0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
							 GUI_ID_ICONVIEW0, 			        /* С���ߵĴ���ID */
							 ICONVIEW_Width, 				    /* ͼ���ˮƽ�ߴ� */
							 ICONVIEW_Height - ICONVIEW_YSpace);/* ͼ��Ĵ�ֱ�ߴ� */
	
	
    /* �����ʾ����װ������ô���ͼ�꣬������������ */
	if(ICONVIEW_VNum * ICONVIEW_HNum < ICONVIEW_ImagNum )
	{
		SCROLLBAR_SetWidth(SCROLLBAR_CreateAttached(hWin,SCROLLBAR_CF_VERTICAL),16);
	}
	
	/* ��ICONVIEW С���������ͼ�� */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) 
	{
		
		ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[i].pBitmap, _aBitmapItem[i].pText);
	}
	
	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(hWin, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);
	
	/* �������� */
	ICONVIEW_SetFont(hWin, &GUI_Font16B_ASCII);
	
	/* ����ͼ����x ��y �����ϵļ�ࡣ*/
	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
	
	/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
	ICONVIEW_SetIconAlign(hWin, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);

	/* ����һ���Ի���,����XPϵͳ�������� */
	hWinTaskBar = GUI_CreateDialogBox(_aDialogCreateMain, 
	                            GUI_COUNTOF(_aDialogCreateMain), 
								&_cbCallbackMain, 
								0, 
								0, 
								0);

	/* 
	 * ������ʱ�����书���Ǿ���ָ�����ں���ָ�����ڷ�����Ϣ��
	 * �ö�ʱ����ָ������������� 
	 */
	WM_CreateTimer(hWinTaskBar,  /* ������Ϣ�Ĵ��ڵľ�� */
	               0, 	         /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
				   1000,         /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
				   0);	         /* ��������ʹ�ã�ӦΪ0 */

	while (1) 
	{
		/* ���ڽ��հ�����������յ���ִ�д���У׼ */
		p_msg = OSTaskQPend(10,
						    OS_OPT_PEND_BLOCKING,
						    &msg_size,
						    &ts,
						    &err);
		
		if(p_msg[0] == 1)
		{
			/* ����������Ĵ��� */
			WM_HideWindow(hWin);
			WM_HideWindow(hWinTaskBar);
			GUI_SetBkColor(GUI_BLUE); 
			GUI_Clear(); 
			WM_Exec();

			/* ִ�д���У׼ */
			TOUCH_Calibration();
			
			/* ��ʾ�����洰�� */
			WM_ShowWindow(hWin);
			WM_ShowWindow(hWinTaskBar);
			GUI_SetBkColor(GUI_BLUE);
			GUI_Clear();
			GUI_SetFont(&GUI_FontHZ_SimSun_16);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringHCenterAt("����K2���ڴ���У׼,����������У׼", LCD_GetXSize()/2, LCD_GetYSize() - 54);
			GUI_Exec();
		}
		else
		{
			GUI_Delay(2);
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

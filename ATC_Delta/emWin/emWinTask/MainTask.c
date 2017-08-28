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
 * tupelo added
 */
extern GUI_CONST_STORAGE GUI_BITMAP bmLogo_armflySmall;

#define MAIN_BKCOLOR              0xD0D0D0
#define MAIN_TEXTCOLOR            0x000000
#define MAIN_FONT                 (&GUI_FontHZ_SimSun_16)
#define MAIN_BORDER               10
#define MAIN_TITLE_HEIGHT         30
#define MAIN_LOGO_BITMAP          (&bmLogo_armflySmall)
#define MAIN_LOGO_OFFSET_X        0
#define MAIN_LOGO_OFFSET_Y        0

#define FRAME_BKCOLOR             0xB0B0B0
#define FRAME_TEXTCOLOR           0x000000
#define FRAME_FONT                (&GUI_FontHZ_SimSun_16)
#define FRAME_EFFECT              (&WIDGET_Effect_3D2L)
#define FRAME_BORDER              FRAME_EFFECT->EffectSize
#define FRAME_WIDTH               (LCD_GetXSize() - (FRAME_BORDER * 2) - (MAIN_BORDER * 2))
#define FRAME_HEIGHT              (LCD_GetYSize() - ICONVIEW_BBorder - (FRAME_BORDER * 2) - (MAIN_BORDER + MAIN_TITLE_HEIGHT))

#define LOGO_FRAME_OFFSET_Y       5
#define LOGO_FRAME_SIZE_X         116
#define LOGO_FRAME_SIZE_Y         92
#define LOGO_FRAME_DIST_X         4
#define LOGO_FRAME_BKCOLOR        0xFFFFFF
#define LOGO_FRAME_EFFECT         (&WIDGET_Effect_3D2L)

extern WM_HWIN    _hLastFrame;
static WM_HWIN    _hTitle;
static WM_HWIN    _hTitlePic;
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
*	����˵��: ��������ʼ��
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
static void _cbCallbackMain(WM_MESSAGE * pMsg) 
{
    WM_HWIN hWin;
    int NCode, Id;
    float CPU;
    char buf[20];

    hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
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
*	�� �� ��: _DrawDownRectEx
*	����˵��: ��ʾ���ݵľ��ο�
*	��    �Σ�pEffect  �ؼ���ʾЧ��
*             pRect    GUI_RECT���ͱ�����ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _DrawDownRectEx(const WIDGET_EFFECT* pEffect, const GUI_RECT* pRect) 
{
	WM_LOCK();
	pEffect->pfDrawDownRect(pRect);
	WM_UNLOCK();
}
/*
*********************************************************************************************************
*	�� �� ��: _DrawDownRect
*	����˵��: ��ʾ���ݵľ��ο�
*	��    �Σ�pEffect  �ؼ���ʾЧ��
*             x0       ��ʼx������
*             y0       ��ʼy������
*             x1       ����x������
*             y1       ����y������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _DrawDownRect(const WIDGET_EFFECT* pEffect, int x0, int y0, int x1, int y1) 
{
	GUI_RECT r;
	
	r.x0 = x0;
	r.y0 = y0;
	r.x1 = x1;
	r.y1 = y1;
	_DrawDownRectEx(pEffect, &r);
}

/*
*********************************************************************************************************
*	�� �� ��: _DrawLogoBox
*	����˵��: С���̵Ļص�����
*	��    �Σ�Index     Ҫ��ʾ��λͼ���
*             pBitmap   λͼ��ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _DrawLogoBox(int Index, const GUI_BITMAP GUI_UNI_PTR* pBitmap) 
{
	int x, y, w, h;
	
	x  = (FRAME_WIDTH - (5 * LOGO_FRAME_SIZE_X) - (4 * LOGO_FRAME_DIST_X)) >> 1;
	y  = FRAME_HEIGHT - LOGO_FRAME_OFFSET_Y - LOGO_FRAME_SIZE_Y;
	w  = LOGO_FRAME_SIZE_X;
	h  = LOGO_FRAME_SIZE_Y;
	x += Index * (w + LOGO_FRAME_DIST_X);
	GUI_SetBkColor(LOGO_FRAME_BKCOLOR);
	GUI_ClearRect(x, y, x + w - 1, y + h - 1);
	_DrawDownRect(LOGO_FRAME_EFFECT, x, y, x + w - 1, y + h - 1);
	x += (w - pBitmap->XSize) >> 1;
	y += (h - pBitmap->YSize) >> 1;
	GUI_DrawBitmap(pBitmap, x, y);
	GUI_SetBkColor(FRAME_BKCOLOR);
	GUI_SetColor(FRAME_TEXTCOLOR);
	GUI_SetFont(FRAME_FONT);
}
/*
*********************************************************************************************************
*	�� �� ��: _PaintFrame
*	����˵��: ��ܴ��ڵ��ػ溯��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _PaintFrame(void) 
{
	GUI_RECT r;
	WM_GetClientRect(&r);
	GUI_SetBkColor(FRAME_BKCOLOR);
	GUI_SetColor(FRAME_TEXTCOLOR);
	GUI_SetFont(FRAME_FONT);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_ClearRectEx(&r);
}
/*
*********************************************************************************************************
*	�� �� ��: _CreateFrame
*	����˵��: ������ܴ���
*	��    �Σ�cb  �ص�������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static WM_HWIN _CreateFrame(WM_CALLBACK* cb) 
{
	int x = 0;
	int y = 0;
	x = FRAME_BORDER + MAIN_BORDER;
	y = FRAME_BORDER + MAIN_TITLE_HEIGHT;
	_hLastFrame = WM_CreateWindowAsChild(x, y, FRAME_WIDTH, FRAME_HEIGHT, WM_HBKWIN, WM_CF_SHOW, cb, 0);
	return _hLastFrame;
}
/*
*********************************************************************************************************
*	�� �� ��: _DeleteFrame
*	����˵��: ɾ�������Ŀ�ܴ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _DeleteFrame(void) 
{
	WM_DeleteWindow(_hLastFrame);
	_hLastFrame = 0;
}
/*
*********************************************************************************************************
*	�� �� ��: _cbInsertCard
*	����˵��: �ڶ������棬�ȴ��û��������п�
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbInsertCard(WM_MESSAGE* pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hWin);
			break;
		case WM_KEY:
            // nothing
            break;
		case WM_PAINT:
			_PaintFrame();
            OnICON3Clicked();
			break;
		case WM_TOUCH:
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
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
		break;
		case WM_KEY:
        break;
		/* �ػ���Ϣ*/
		case WM_PAINT:
            {
				int x, y, w, h;
				
				GUI_SetBkColor(MAIN_BKCOLOR);
				GUI_SetColor(MAIN_TEXTCOLOR);
				GUI_SetFont(MAIN_FONT);
				GUI_Clear();
				x = MAIN_LOGO_OFFSET_X + MAIN_BORDER;
				y = MAIN_LOGO_OFFSET_Y + ((MAIN_TITLE_HEIGHT - MAIN_LOGO_BITMAP->YSize) >> 1);
				GUI_DrawBitmap(MAIN_LOGO_BITMAP, x, y);
                
				x = MAIN_BORDER;
				y = MAIN_TITLE_HEIGHT;
				w = LCD_GetXSize() - (MAIN_BORDER * 2);
				h = LCD_GetYSize()  - (MAIN_BORDER + MAIN_TITLE_HEIGHT);
				_DrawDownRect(FRAME_EFFECT, x, y, x + w - 1, y + h - 1);
			}		
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}
/*
*********************************************************************************************************
*	�� �� ��: _cbLanguage
*	����˵��: ��һ�����棬�������ĺ�Ӣ���ѡ��
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbLanguage(WM_MESSAGE* pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hWin);
        
            _hTitle = TEXT_CreateEx(0, 5, LCD_GetXSize(), 32, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "�Զ�������");
            TEXT_SetTextAlign(_hTitle, GUI_TA_HCENTER);
            TEXT_SetFont(_hTitle, MAIN_FONT);
			break;
		 case WM_KEY:
//             switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
//             { 
// 				case GUI_KEY_ESCAPE:
//                     GUI_EndDialog(hWin, 1);
//                     break;
// 				case GUI_KEY_TAB:
// 					WM_SetFocusOnNextChild(hWin);
// 					break;
//             }
            break;
		case WM_PAINT:
			_PaintFrame(); 

		    GUI_DispStringHCenterAt("��ѡ������", FRAME_WIDTH >> 1, 5);
			GUI_DispStringHCenterAt("Please select your language", FRAME_WIDTH >> 1, 32);
			break;
		case WM_NOTIFY_PARENT:
// 			if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) 
// 			{
// 				int Id = WM_GetId(pMsg->hWinSrc);
// 				switch (Id) 
// 				{
// 					case GUI_ID_BUTTON0:
// 						_Language = 0;
// 						break;
// 					case GUI_ID_BUTTON1:
// 						_Language = 1;
// 						break;
// 				}
// 				/* �������⣬������ʾ */
// 				_hTitle = TEXT_CreateEx(0, 0, LCD_GetXSize(), 32, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, _GetLang(TEXT_ID_GELDAUTOMAT));
// 				TEXT_SetTextAlign(_hTitle, GUI_TA_HCENTER);
// 				TEXT_SetFont(_hTitle, MAIN_FONT);
// 				/* ɾ����������Ľ��� */
// 				_DeleteFrame();
// 				_CreateFrame(&_cbInsertCard);
// 			}
            /* �������⣬������ʾ */
            _hTitlePic = TEXT_CreateEx(0, 5, LCD_GetXSize(), 32, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "�����ļ���ʾ");
            TEXT_SetTextAlign(_hTitlePic, GUI_TA_HCENTER);
            TEXT_SetFont(_hTitlePic, MAIN_FONT);
            /* ɾ����������Ľ��� */
            _DeleteFrame();
            OnICON3Clicked();
            _CreateFrame(&_cbBkWindow2);
			break;
		default:
		WM_DefaultProc(pMsg);
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
	OS_ERR      err;
	CPU_TS		ts;
	OS_MSG_SIZE	msg_size;
	uint8_t	   *p_msg;
	
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

    /* ���������� */
	_CreateFrame(&_cbLanguage);
    
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

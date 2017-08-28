/*
*********************************************************************************************************
*	                                  
*	Ä£¿éÃû³Æ : GUI½çÃæÖ÷º¯Êı
*	ÎÄ¼şÃû³Æ : MainTask.c
*	°E   ±¾ : V1.2d
*	Ëµ    ÃE: ×ÛºÏÊµÀıÖ÷½çÃæÉè¼Æ
*	ĞŞ¸Ä¼ÇÂ¼ :
*		°æ±¾ºÅ    ÈÕÆÚ          ×÷Õß        ËµÃE
*		v1.0    2013-04-26    Eric2013      Ê×·¢
*       v1.1    2014-05-23    Eric2013      1. ¸EÄÖ÷½çÃæÍ¬Ê±Ö§³Ö4.3´ç£¬5´çºÍ7´çÆÁµÄÏÔÊ¾
*                                           2. ÔİÊ±É¾³ı¶¯Ì¬LOGOµÄÏÔÊ¾£¬ºóÃæ°æ±¾ÔÙ¼ÓÈE
*       V1.2    2014-07-19    Eric2013      1. ½â¾öICON¿Ø¼şĞèÒªµã»÷Á½´Î²Å¿ÉÒÔ½øÈEÄÎÊÌâ¡£
*                                           2. É¾³ıFLASHµÄ½çÃæºÍ´¥ÃşĞ£×¼½çÃE
*       V1.2b   2014-12-05    Eric2013      1. ÔİÊ±É¾³ı±³¾°Í¼Æ¬µÄÏÔÊ¾
*                                           2. ¼ÓÈEÄµã´¥ÃşĞ£×¼
*       V1.2c   2015-05-10 	  Eric2013      1. Éı¼¶¹Ì¼ş¿âµ½V1.5.0
*       								    2. Éı¼¶BSP°å¼¶Ö§³Ö°E
*      									    3. Éı¼¶fatfsµ½0.11
*       								    4. Éı¼¶STemWinµ½5.26
*       								    5. ÌúØÓ7´E00*480·Ö±æÂÊµçÈİÆÁÖ§³Ö£¬ÌúØÓ3.5´E80*320µÄILI9488Ö§³Ö¡£
*      									    6. ÔÚÓ¦ÓÃApp_FileBrowseÖĞÍ¨¹ıº¯ÊıCHOOSEFILE_SetButtonTextĞŞ¸Ä°´¼EóĞ¡£¬ÒÔ±ãµçÈİ´¥ÃşÆÁ¿ÉÒÔµã»÷µ½¡£
*	    V1.2d  2015-12-18  Eric2013         1. Éı¼¶BSP°å¼¶Ö§³Ö°E
*                                           2. Éı¼¶STemWinµ½5.28
*                                           3. ÌúØÓ4.3´çºÍ5´çµçÈİÆÁÖ§³Ö¡£       
*	
*	Copyright (C), 2015-2020, °²¸»À³µç×Ó www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"

/*
*********************************************************************************************************
*                                         ºE¨ÒE
*********************************************************************************************************
*/ 
#define ICONVIEW_TBorder   10   /* ¿Ø¼şICONVIEWµÄÉÏ±ß¾E*/
#define ICONVIEW_LBorder   10   /* ¿Ø¼şICONVIEWµÄ×ó±ß¾E*/
#define ICONVIEW_RBorder   10   /* ¿Ø¼şICONVIEWµÄÓÒ±ß¾E*/
#define ICONVIEW_BBorder   36   /* ¿Ø¼şICONVIEWµÄÏÂ±ß¾E*/   
#define ICONVIEW_Width     70   /* ¿Ø¼şICONVIEWµÄ¿E*/  
#define ICONVIEW_Height    85   /* ¿Ø¼şICONVIEWµÄ¸ß£¬°E¨Í¼±EÄY·½Ïò¼ä¾E*/  
#define ICONVIEW_YSpace    5    /* ¿Ø¼şICONVIEWµÄY·½Ïò¼ä¾E*/  
#define ICONVIEW_ImagNum   20   /* ¿Ø¼şICONVIEWÖĞµÄÍ¼±EöÊı */ 

#define Imag_LBorder     2   /* Í¼±EÄ×ó±ß¾E*/
#define Imag_TBorder     2   /* Í¼±EÄÉÏ±ß¾E*/
#define Time_Width       90  /* ÏÔÊ¾Ê±¼äµÄ¿úÒÈ */
#define ImagStart_Width  40  /* ¿ªÊ¼Í¼±EÄ¿úÒÈ */
#define ImagSpeak_Width  40  /* ÒôÁ¿Í¼±EÄ¿úÒÈ */
#define ImagSignal_Width 40  /* ĞÅºÅÍ¼±EÄ¿úÒÈ */
#define ImagTime_Width   210 /* ÉÏÃæ¿ªÊ¼Í¼±ê£¬ÒôÁ¿Í¼±ê£¬ĞÅºÅÍ¼±EÔ¼°Ê±¼äÏÔÊ¾µÄ×Ü¿úÒÈ40+40+40+90=210*/

#define Time_Line1   1   /* µÚÒ»ĞĞÊ±¼äÏÔÊ¾µÄY×ø±E*/
#define Time_Line2   17  /* µÚ¶şĞĞÊ±¼äÏÔÊ¾µÄY×ø±E*/
#define Time_Height  16  /* Ê±¼äÏÔÊ¾µÄ¸ß¶È */
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
*                                      ±äÁ¿
*********************************************************************************************************
*/ 
WM_HWIN hWin;           /* ICONVIEWµÄ¾ä±E*/
WM_HWIN hWinTaskBar;    /* ÈÎÎñÀ¸¾ä±E*/
uint8_t OnICON0  = 0;   /* ICONVIEW¿Ø¼ş°´ÏÂµÄ±E¾£¬0±úæ¾Î´°´ÏÂ£¬1±úæ¾°´ÏÂ */
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

uint8_t g_ucState = USB_HOST;            /* Ä¬ÈÏÑ¡ÔñUSB HOSTÄ£Ê½       */

/* ÓÃÓÚĞÇÆÚµÄÏÔÊ¾ */
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

/* ÓÃÓÚ×ÀÃæICONVIEWÍ¼±EÄ´´½¨ */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

/* ÓÃÓÚ×ÀÃæICONVIEWÍ¼±EÄ´´½¨ */
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
*				                         ¶Ô»°¿ò³õÊ¼»¯Ñ¡ÏE
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
*	º¯ Êı ÃE File_Init
*	¹¦ÄÜËµÃE ÎÄ¼şÏµÍ³³õÊ¼»¯
*	ĞÎ    ²Î£ºÎŞ
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
static void File_Init(void)
{
	/* ·½±ãÒÔºóĞŞ¸ÄÍ¼±EÄÎ»ÖÃ */
	 /* ³õÊ¼»¯UÅÌ */
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
*	º¯ Êı ÃE Caculate_RTC
*	¹¦ÄÜËµÃE ÏÔÊ¾RTCÊ±¼E
*	ĞÎ    ²Î£ºpMsg Ö¸ÕEÎÊı            
*	·µ »Ø Öµ: ÎŞ
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
*	º¯ Êı ÃE PaintDialogMain
*	¹¦ÄÜËµÃE ÈÎÎñÀ¸µÄÖØ»æº¯Êı 
*	ĞÎ    ²Î£ºpMsg Ö¸ÕEÎÊı
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
void PaintDialogMain(WM_MESSAGE * pMsg)
{
	GUI_SetBkColor(GUI_WHITE); 
    GUI_Clear(); 
	
	/* »æÖÆÆô¶¯Ğ¡Í¼±E*/
	GUI_DrawBitmap(&bmStart, 
	               Imag_LBorder, 
	               Imag_TBorder);
	
	/* »æÖÆÀ®°ÈĞ¡Í¼±E*/
	GUI_DrawBitmap(&bmSpeaker, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width + Imag_LBorder, 
	               Imag_TBorder);
	
	/* »æÖÆĞÅºÅĞ¡Í¼±E*/
	GUI_DrawBitmap(&bmsignal, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width - ImagSignal_Width  + Imag_LBorder, 
	               Imag_TBorder);	
}

/*
*********************************************************************************************************
*	º¯ Êı ÃE InitDialogMain
*	¹¦ÄÜËµÃE ¶Ô»°¿ò³õÊ¼»¯
*	ĞÎ    ²Î£ºpMsg
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
void InitDialogMain(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin; 
	WM_HWIN hClient;
    
	hClient = WM_GetClientWindow(hWin);
	
	/* ÖØĞÂÅäÖÃÏÔÊ¾Î»ÖÃ */
	WM_SetWindowPos(hWin, 
	                0, 
	                LCD_GetYSize() - ICONVIEW_BBorder, 
               	    LCD_GetXSize(),
	                ICONVIEW_BBorder);
    
	/* ÉèÖÃ¿Ø¼şGUI_ID_PROGBAR0µÄÎ»ÖÃ */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 
					WM_GetWindowOrgX(hClient)+ImagStart_Width,  
					WM_GetWindowOrgY(hClient),  
					LCD_GetXSize()-ImagTime_Width,
					ICONVIEW_BBorder);
	
	/* ÉèÖÃ¿Ø¼şGUI_ID_TEXT0µÄÎ»ÖÃ */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_TEXT0), 
					WM_GetWindowOrgX(hClient)+LCD_GetXSize()-Time_Width,  
					WM_GetWindowOrgY(hClient)+Time_Line1,  
					Time_Width,
					Time_Height);
	
	/* ÉèÖÃ¿Ø¼şGUI_ID_TEXT1µÄÎ»ÖÃ */
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
*	º¯ Êı ÃE _cbCallbackMain
*	¹¦ÄÜËµÃE »Øµ÷º¯Êı 
*	ĞÎ    ²Î£ºpMsg
*	·µ »Ø Öµ: ÎŞ
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
*	º¯ Êı ÃE _DrawDownRectEx
*	¹¦ÄÜËµÃE ÏÔÊ¾°¼ÏİµÄ¾ØĞÎ¿E
*	ĞÎ    ²Î£ºpEffect  ¿Ø¼şÏÔÊ¾Ğ§¹E
*             pRect    GUI_RECTÀàĞÍ±äÁ¿µØÖ·
*	·µ »Ø Öµ: ÎŞ
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
*	º¯ Êı ÃE _DrawDownRect
*	¹¦ÄÜËµÃE ÏÔÊ¾°¼ÏİµÄ¾ØĞÎ¿E
*	ĞÎ    ²Î£ºpEffect  ¿Ø¼şÏÔÊ¾Ğ§¹E
*             x0       ÆğÊ¼xÖá×ø±E
*             y0       ÆğÊ¼yÖá×ø±E
*             x1       ½áÊøxÖá×ø±E
*             y1       ½áÊøyÖá×ø±E
*	·µ »Ø Öµ: ÎŞ
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
*	º¯ Êı ÃE _DrawLogoBox
*	¹¦ÄÜËµÃE Ğ¡¼EÌµÄ»Øµ÷º¯Êı
*	ĞÎ    ²Î£ºIndex     ÒªÏÔÊ¾µÄÎ»Í¼ĞòºÅ
*             pBitmap   Î»Í¼µØÖ·
*	·µ »Ø Öµ: ÎŞ
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
*	º¯ Êı ÃE _PaintFrame
*	¹¦ÄÜËµÃE ¿ò¼Ü´°¿ÚµÄÖØ»æº¯Êı
*	ĞÎ    ²Î£ºÎŞ
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
static void _PaintFrame(void) 
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
*	º¯ Êı ÃE _CreateFrame
*	¹¦ÄÜËµÃE ´´½¨¿ò¼Ü´°¿Ú
*	ĞÎ    ²Î£ºcb  »Øµ÷º¯ÊıµØÖ·
*	·µ »Ø Öµ: ÎŞ
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
*	º¯ Êı ÃE _DeleteFrame
*	¹¦ÄÜËµÃE É¾³ı´´½¨µÄ¿ò¼Ü´°¿Ú
*	ĞÎ    ²Î£ºÎŞ
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
static void _DeleteFrame(void) 
{
	WM_DeleteWindow(_hLastFrame);
	_hLastFrame = 0;
}
/*
*********************************************************************************************************
*	º¯ Êı ÃE _cbInsertCard
*	¹¦ÄÜËµÃE µÚ¶ş¸ö½çÃæ£¬µÈ´ıÓÃ»§²åÈEøĞĞ¿¨
*	ĞÎ    ²Î£ºpMsg  ²ÎÊıÖ¸ÕE
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
static void _cbInsertCard(WM_MESSAGE* pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ÉèÖÃ¾Û½¹ */
			WM_SetFocus(hWin);
			break;
		case WM_KEY:
            // nothing
            break;
		case WM_PAINT:
			_PaintFrame();
			_DrawLogoBox(0, &bmLogo_armflySmall);
			_DrawLogoBox(1, &bmLogo_armflySmall);
			_DrawLogoBox(2, &bmLogo_armflySmall);
			_DrawLogoBox(3, &bmLogo_armflySmall);
			_DrawLogoBox(4, &bmLogo_armflySmall);
			GUI_DispStringHCenterAt("123456", FRAME_WIDTH >> 1, 15);
			break;
		case WM_TOUCH:
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	º¯ Êı ÃE _cbBkWindow
*	¹¦ÄÜËµÃE ×ÀÃæ´°¿ÚµÄ»Øµ÷º¯Êı 
*	ĞÎ    ²Î£ºWM_MESSAGE * pMsg
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
__IO uint8_t prevent_refresh = 0;  /* Õâ¸ö²ÎÊı±È½ÏÖØÒª£¬ÓÃÓÚ·ÀÖ¹ICON¿Ø¼şµã»÷ÏÂÊ±¾Í´¥·¢»Øµ÷º¯ÊıµÄWM_PAINTÏûÏ¢*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
// 		Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
// 		NCode = pMsg->Data.v;                 /* Notification code */
// 		switch (Id) 
// 		{

// 		}
		break;
		case WM_KEY:
//             GUI_SetBkColor(GUI_BLUE);
//             GUI_Clear();
//             GUI_SetFont(&GUI_FontHZ_SimSun_16);
//             GUI_SetColor(GUI_WHITE);
//             GUI_DispStringHCenterAt("°´¼üK2ÓÃÓÚ´¥ÃşĞ£×¼,µçÈİÆÁÎŞĞèĞ£×¼", LCD_GetXSize()/2, LCD_GetYSize() - 100);
        break;
		/* ÖØ»æÏûÏ¢*/
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
	}
}
/*
*********************************************************************************************************
*	º¯ Êı ÃE  _cbMainGUI
*	¹¦ÄÜËµÃE  Ö÷½çÃæ£¬ÓÃÓÚÏÔÊ¾¹¤³§³µ¼äĞÅÏ¢£¬Á÷Ë®ÏßĞÅÏ¢£¬¶©µ¥ĞÅÏ¢£¬ºÍ¹¤ÈËĞÅÏ¢
*	ĞÎ    ²Î£º pMsg  ²ÎÊıÖ¸ÕE
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
static void _cbMainGUI(WM_MESSAGE* pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ÉèÖÃ¾Û½¹ */
			WM_SetFocus(hWin);
        
            _hTitle = TEXT_CreateEx(0, 5, LCD_GetXSize(), 32, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "×Ô¶¯¼ÆÊıÆE);
            TEXT_SetTextAlign(_hTitle, GUI_TA_HCENTER);
            TEXT_SetFont(_hTitle, MAIN_FONT);
			break;
		case WM_KEY:
            // nothing;
        break;
		case WM_PAINT:
			_PaintFrame(); 
            GUI_DispStringHCenterAt("ÇE¡ÔñÓEÔ", FRAME_WIDTH >> 1, 5);
			GUI_DispStringHCenterAt("Please select your language", FRAME_WIDTH >> 1, 32);
			break;
		case WM_NOTIFY_PARENT:
            /* É¾³ıÕâ¸ö´´½¨µÄ½çÃæ£¬½øÈE¤ÒÕÎÄ¼şÏÔÊ¾½çÃE*/
            _DeleteFrame();
            _CreateFrame(&_cbInsertCard);
            break;
		default:
		WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	º¯ Êı ÃE MainTask
*	¹¦ÄÜËµÃE GUIÖ÷º¯Êı 
*	ĞÎ    ²Î£ºÎŞ
*	·µ »Ø Öµ: ÎŞ
*********************************************************************************************************
*/
void MainTask(void) 
{	
	uint8_t i;
	
	OS_ERR      err;
	CPU_TS		ts;
	OS_MSG_SIZE	msg_size;
	uint8_t	   *p_msg;
	
	prevent_refresh = 0; 

	/* ÎÄ¼şÏµÍ³³õÊ¼»¯ *********************************************************/
    File_Init();

	/* ÉèÖÃÆ¤·ôÉ« *************************************************************/
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
	 * ÔÚËùÓĞµÄ´°¿ÚÉÏ×Ô¶¯µÄÊ¹ÓÃÄÚ´æÉè±¸£¬ÔÚGUI_Initºóµ÷ÓÃ²»°E¨
	 * ±³¾°´°¿Ú£¨×ÀÃæ´°¿Ú£©£¬Ö®Ç°µ÷ÓÃ²Å°E¨±³¾°´°¿Ú 
	 */
 	WM_SetCreateFlags(WM_CF_MEMDEV);			
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

    /* ½øÈE÷½çÃE*/
	_CreateFrame(&_cbMainGUI);
    
	/* ´´½¨Ò»¸ö¶Ô»°¿EÀàĞÍXPÏµÍ³µÄÈÎÎñÀ¸ */
	hWinTaskBar = GUI_CreateDialogBox(_aDialogCreateMain, 
	                            GUI_COUNTOF(_aDialogCreateMain), 
								&_cbCallbackMain, 
								0, 
								0, 
								0);

	/* 
	 * ´´½¨¶¨Ê±Æ÷£¬Æä¹¦ÄÜÊÇ¾­¹ıÖ¸¶¨ÖÜÆÚºó£¬ÏòÖ¸¶¨´°¿Ú·¢ËÍÏûÏ¢¡£
	 * ¸Ã¶¨Ê±Æ÷ÓE¸¶¨´°¿ÚÏà¹ØÁª¡£ 
	 */
	WM_CreateTimer(hWinTaskBar,  /* ½ÓÊÜĞÅÏ¢µÄ´°¿ÚµÄ¾ä±E*/
	               0, 	         /* ÓÃ»§¶¨ÒåµÄId¡£Èç¹û²»¶ÔÍ¬Ò»´°¿ÚÊ¹ÓÃ¶à¸ö¶¨Ê±Æ÷£¬´ËÖµ¿ÉÒÔÉèÖÃÎªÁã¡£ */
				   1000,         /* ÖÜÆÚ£¬´ËÖÜÆÚ¹ıºóÖ¸¶¨´°¿ÚÓ¦ÊÕµ½ÏûÏ¢*/
				   0);	         /* Áô´ı½«À´Ê¹ÓÃ£¬Ó¦Îª0 */

	while (1) 
	{
		/* ÓÃÓÚ½ÓÊÕ°´¼E¬Èç¹û½ÓÊÕµ½£¬Ö´ĞĞ´¥ÃşĞ£×¼ */
		p_msg = OSTaskQPend(10,
						    OS_OPT_PEND_BLOCKING,
						    &msg_size,
						    &ts,
						    &err);
		
		if(p_msg[0] == 1)
		{
			/* Òş²ØÖ÷½çÃæµÄ´°¿Ú */
			WM_HideWindow(hWin);
			WM_HideWindow(hWinTaskBar);
			GUI_SetBkColor(GUI_BLUE); 
			GUI_Clear(); 
			WM_Exec();

			/* Ö´ĞĞ´¥ÃşĞ£×¼ */
			TOUCH_Calibration();
			
			/* ÏÔÊ¾Ö÷½çÃæ´°¿Ú */
			WM_ShowWindow(hWin);
			WM_ShowWindow(hWinTaskBar);
			GUI_SetBkColor(GUI_BLUE);
			GUI_Clear();
			GUI_SetFont(&GUI_FontHZ_SimSun_16);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringHCenterAt("°´¼üK2ÓÃÓÚ´¥ÃşĞ£×¼,µçÈİÆÁÎŞĞèĞ£×¼", LCD_GetXSize()/2, LCD_GetYSize() - 54);
			GUI_Exec();
		}
		else
		{
			GUI_Delay(2);
		}
	}
}

/***************************** °²¸»À³µç×Ó www.armfly.com (END OF FILE) *********************************/

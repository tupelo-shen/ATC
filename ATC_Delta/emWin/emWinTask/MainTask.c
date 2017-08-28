/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	皝E   本 : V1.2d
*	说    明: 综合实例主界面设计
*	修改记录 :
*		版本号    日期          作者        说脕E
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      1. 竵E闹鹘缑嫱敝С�4.3寸，5寸和7寸屏的显示
*                                           2. 暂时删除动态LOGO的显示，后面版本再加葋E
*       V1.2    2014-07-19    Eric2013      1. 解决ICON控件需要点击两次才可以进葋E奈侍狻�
*                                           2. 删除FLASH的界面和触摸校准界脕E
*       V1.2b   2014-12-05    Eric2013      1. 暂时删除背景图片的显示
*                                           2. 加葋E牡愦ッＷ�
*       V1.2c   2015-05-10 	  Eric2013      1. 升级固件库到V1.5.0
*       								    2. 升级BSP板级支持皝E
*      									    3. 升级fatfs到0.11
*       								    4. 升级STemWin到5.26
*       								    5. 铁赜7磥E00*480分辨率电容屏支持，铁赜3.5磥E80*320的ILI9488支持。
*      									    6. 在应用App_FileBrowse中通过函数CHOOSEFILE_SetButtonText修改按紒E笮。员愕缛荽ッ量梢缘慊鞯健�
*	    V1.2d  2015-12-18  Eric2013         1. 升级BSP板级支持皝E
*                                           2. 升级STemWin到5.28
*                                           3. 铁赜4.3寸和5寸电容屏支持。       
*	
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"

/*
*********************************************************************************************************
*                                         簛Eㄒ丒
*********************************************************************************************************
*/ 
#define ICONVIEW_TBorder   10   /* 控件ICONVIEW的上边緛E*/
#define ICONVIEW_LBorder   10   /* 控件ICONVIEW的左边緛E*/
#define ICONVIEW_RBorder   10   /* 控件ICONVIEW的右边緛E*/
#define ICONVIEW_BBorder   36   /* 控件ICONVIEW的下边緛E*/   
#define ICONVIEW_Width     70   /* 控件ICONVIEW的縼E*/  
#define ICONVIEW_Height    85   /* 控件ICONVIEW的高，皝Eㄍ急丒腨方向间緛E*/  
#define ICONVIEW_YSpace    5    /* 控件ICONVIEW的Y方向间緛E*/  
#define ICONVIEW_ImagNum   20   /* 控件ICONVIEW中的图眮E鍪� */ 

#define Imag_LBorder     2   /* 图眮E淖蟊呔丒*/
#define Imag_TBorder     2   /* 图眮E纳媳呔丒*/
#define Time_Width       90  /* 显示时间的窥胰 */
#define ImagStart_Width  40  /* 开始图眮E目� */
#define ImagSpeak_Width  40  /* 音量图眮E目� */
#define ImagSignal_Width 40  /* 信号图眮E目� */
#define ImagTime_Width   210 /* 上面开始图标，音量图标，信号图眮E约笆奔湎允镜淖芸�40+40+40+90=210*/

#define Time_Line1   1   /* 第一行时间显示的Y坐眮E*/
#define Time_Line2   17  /* 第二行时间显示的Y坐眮E*/
#define Time_Height  16  /* 时间显示的高度 */
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
/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
WM_HWIN hWin;           /* ICONVIEW的句眮E*/
WM_HWIN hWinTaskBar;    /* 任务栏句眮E*/
uint8_t OnICON0  = 0;   /* ICONVIEW控件按下的眮E荆�0柄婢未按下，1柄婢按下 */
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

uint8_t g_ucState = USB_HOST;            /* 默认选择USB HOST模式       */

/* 用于星期的显示 */
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

/* 用于桌面ICONVIEW图眮E拇唇� */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

/*
*********************************************************************************************************
*				                         对话框初始化选蟻E
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
*	函 数 名: File_Init
*	功能说脕E 文件系统初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void File_Init(void)
{
	/* 方便以后修改图眮E奈恢� */
	 /* 初始化U盘 */
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
*	函 数 名: Caculate_RTC
*	功能说脕E 显示RTC时紒E
*	形    参：pMsg 指諄E问�            
*	返 回 值: 无
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
*	函 数 名: PaintDialogMain
*	功能说脕E 任务栏的重绘函数 
*	形    参：pMsg 指諄E问�
*	返 回 值: 无
*********************************************************************************************************
*/
void PaintDialogMain(WM_MESSAGE * pMsg)
{
	GUI_SetBkColor(GUI_WHITE); 
    GUI_Clear(); 
	
	/* 绘制启动小图标*/
	GUI_DrawBitmap(&bmStart, 
	               Imag_LBorder, 
	               Imag_TBorder);
	
	/* 绘制喇叭小图标*/
	GUI_DrawBitmap(&bmSpeaker, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width + Imag_LBorder, 
	               Imag_TBorder);
	
	/* 绘制信号小图标*/
	GUI_DrawBitmap(&bmsignal, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width - ImagSignal_Width  + Imag_LBorder, 
	               Imag_TBorder);	
}

/*
*********************************************************************************************************
*	函 数 名: InitDialogMain
*	功能说明: 任务栏初始化
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void InitDialogMain(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin; 
	WM_HWIN hClient;
    
	hClient = WM_GetClientWindow(hWin);
	
	/* 重新配置显示位置 */
	WM_SetWindowPos(hWin, 
	                0, 
	                LCD_GetYSize() - ICONVIEW_BBorder, 
               	    LCD_GetXSize(),
	                ICONVIEW_BBorder);
    
	/* 设置控件GUI_ID_PROGBAR0的位置 */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 
					WM_GetWindowOrgX(hClient)+ImagStart_Width,  
					WM_GetWindowOrgY(hClient),  
					LCD_GetXSize()-ImagTime_Width,
					ICONVIEW_BBorder);
	
	/* 设置控件GUI_ID_TEXT0的位置 */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_TEXT0), 
					WM_GetWindowOrgX(hClient)+LCD_GetXSize()-Time_Width,  
					WM_GetWindowOrgY(hClient)+Time_Line1,  
					Time_Width,
					Time_Height);
	
	/* 设置控件GUI_ID_TEXT1的位置 */
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
*	函 数 名: _cbCallbackMain
*	功能说明: 回调函数 
*	形    参: pMsg
*	返 回 值: 无
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
*	函 数 ��: _DrawDownRectEx
*	功能说脕E 显示凹陷的矩形縼E
*	形    参：pEffect  控件显示效箒E
*             pRect    GUI_RECT类型变量地址
*	返 回 值: 无
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
*	函 数 名: _DrawDownRect
*	功能说明：显示凹陷的矩形縼E
*	形    参：pEffect  控件显示效果
*             x0       起始x轴坐标
*             y0       起始y轴坐标
*             x1       结束x轴坐标
*             y1       结束y轴坐标
*	返 回 值: 无
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
*	函 数 名: _DrawLogoBox
*	功能说脕E 小紒E痰幕氐骱�
*	形    参：Index     要显示的位图序号
*             pBitmap   位图地址
*	返 回 值: 无
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
*	函 数 名: _PaintFrame
*	功能说脕E 框架窗口的重绘函数
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: _CreateFrame
*	功能说脕E 创建框架窗口
*	形    参：cb  回调函数地址
*	返 回 值: 无
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
*	函 数 名: _DeleteFrame
*	功能说脕E 删除创建的框架窗口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _DeleteFrame(void) 
{
	WM_DeleteWindow(_hLastFrame);
	_hLastFrame = 0;
}
/*
*********************************************************************************************************
*	函 数 名: _cbInsertCard
*	功能说脕E 第二个界面，等待用户插葋E锌�
*	形    参：pMsg  参数指諄E
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbInsertCard(WM_MESSAGE* pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
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
*	函 数 名: _cbBkWindow
*	功能说脕E 桌面窗口的回调函数 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
__IO uint8_t prevent_refresh = 0;  /* 这个参数比较重要，用于防止ICON控件点击下时就触发回调函数的WM_PAINT消息*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
		break;
		case WM_KEY:
        break;
		/* 重绘消息*/
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
*	函 数 名:  _cbMainGUI
*	功能说脕E  主界面，用于显示工厂车间信息，流水线信息，订单信息，和工人信息
*	形    参： pMsg  参数指諄E
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbMainGUI(WM_MESSAGE* pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hWin);
        
            _hTitle = TEXT_CreateEx(0, 5, LCD_GetXSize(), 32, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "自动计数器");
            TEXT_SetTextAlign(_hTitle, GUI_TA_HCENTER);
            TEXT_SetFont(_hTitle, MAIN_FONT);
			break;
		case WM_KEY:
            // nothing;
        break;
		case WM_PAINT:
			_PaintFrame(); 
            GUI_DispStringHCenterAt("莵E≡裼丒�", FRAME_WIDTH >> 1, 5);
			GUI_DispStringHCenterAt("Please select your language", FRAME_WIDTH >> 1, 32);
			break;
		case WM_NOTIFY_PARENT:
            /* 删除这个创建的界面，进葋Eひ瘴募允窘缑丒*/
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
*	函 数 名: MainTask
*	功能说脕E GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{	
	OS_ERR      err;
	CPU_TS		ts;
	OS_MSG_SIZE	msg_size;
	uint8_t	   *p_msg;
	
	prevent_refresh = 0; 

	/* 文件系统初始化 *********************************************************/
    File_Init();

	/* 设置皮肤色 *************************************************************/
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
	 * 在所有的窗口上自动的使用内存设备，在GUI_Init后调用不皝E�
	 * 背景窗口（桌面窗口），之前调用才皝Eū尘按翱� 
	 */
 	WM_SetCreateFlags(WM_CF_MEMDEV);			
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

    /* 进葋E鹘缑丒*/
	_CreateFrame(&_cbMainGUI);
    
	/* 创建一个对话縼E类型XP系统的任务栏 */
	hWinTaskBar = GUI_CreateDialogBox(_aDialogCreateMain, 
	                            GUI_COUNTOF(_aDialogCreateMain), 
								&_cbCallbackMain, 
								0, 
								0, 
								0);

	/* 
	 * 创建定时器，其功能是经过指定周期后，向指定窗口发送消息。
	 * 该定时器觼E付ù翱谙喙亓� 
	 */
	WM_CreateTimer(hWinTaskBar,  /* 接受信息的窗口的句眮E*/
	               0, 	         /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
				   1000,         /* 周期，此周期过后指定窗口应收到消息*/
				   0);	         /* 留待将来使用，应为0 */

	while (1) 
	{
		/* 用于接收按紒E绻邮盏剑葱写ッＷ� */
		p_msg = OSTaskQPend(10,
						    OS_OPT_PEND_BLOCKING,
						    &msg_size,
						    &ts,
						    &err);
		
		if(p_msg[0] == 1)
		{
			/* 隐藏主界面的窗口 */
			WM_HideWindow(hWin);
			WM_HideWindow(hWinTaskBar);
			GUI_SetBkColor(GUI_BLUE); 
			GUI_Clear(); 
			WM_Exec();

			/* 执行触摸校准 */
			TOUCH_Calibration();
			
			/* 显示主界面窗口 */
			WM_ShowWindow(hWin);
			WM_ShowWindow(hWinTaskBar);
			GUI_SetBkColor(GUI_BLUE);
			GUI_Clear();
			GUI_SetFont(&GUI_FontHZ_SimSun_16);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringHCenterAt("按键K2用于触摸校准,电容屏无需校准", LCD_GetXSize()/2, LCD_GetYSize() - 54);
			GUI_Exec();
		}
		else
		{
			GUI_Delay(2);
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

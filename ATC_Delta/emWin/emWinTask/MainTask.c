/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.2d
*	说    明 : 综合实例主界面设计
*	修改记录 :
*		版本号    日期          作者        说明
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      1. 更改主界面同时支持4.3寸，5寸和7寸屏的显示
*                                           2. 暂时删除动态LOGO的显示，后面版本再加入
*       V1.2    2014-07-19    Eric2013      1. 解决ICON控件需要点击两次才可以进入的问题。
*                                           2. 删除FLASH的界面和触摸校准界面
*       V1.2b   2014-12-05    Eric2013      1. 暂时删除背景图片的显示
*                                           2. 加入四点触摸校准
*       V1.2c   2015-05-10 	  Eric2013      1. 升级固件库到V1.5.0
*       								    2. 升级BSP板级支持包 
*      									    3. 升级fatfs到0.11
*       								    4. 升级STemWin到5.26
*       								    5. 添加7寸800*480分辨率电容屏支持，添加3.5寸480*320的ILI9488支持。
*      									    6. 在应用App_FileBrowse中通过函数CHOOSEFILE_SetButtonText修改按键大小，以便电容触摸屏可以点击到。
*	    V1.2d  2015-12-18  Eric2013         1. 升级BSP板级支持包 
*                                           2. 升级STemWin到5.28
*                                           3. 添加4.3寸和5寸电容屏支持。       
*	
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"

/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/ 
#define ICONVIEW_TBorder   10   /* 控件ICONVIEW的上边距 */
#define ICONVIEW_LBorder   10   /* 控件ICONVIEW的左边距 */
#define ICONVIEW_RBorder   10   /* 控件ICONVIEW的右边距 */
#define ICONVIEW_BBorder   36   /* 控件ICONVIEW的下边距 */   
#define ICONVIEW_Width     70   /* 控件ICONVIEW的宽 */  
#define ICONVIEW_Height    85   /* 控件ICONVIEW的高，包括图标的Y方向间距 */  
#define ICONVIEW_YSpace    5    /* 控件ICONVIEW的Y方向间距 */  
#define ICONVIEW_ImagNum   20   /* 控件ICONVIEW中的图标个数 */ 

#define Imag_LBorder     2   /* 图标的左边距 */
#define Imag_TBorder     2   /* 图标的上边距 */
#define Time_Width       90  /* 显示时间的宽度 */
#define ImagStart_Width  40  /* 开始图标的宽度 */
#define ImagSpeak_Width  40  /* 音量图标的宽度 */
#define ImagSignal_Width 40  /* 信号图标的宽度 */
#define ImagTime_Width   210 /* 上面开始图标，音量图标，信号图标以及时间显示的总宽度40+40+40+90=210*/

#define Time_Line1   1   /* 第一行时间显示的Y坐标 */
#define Time_Line2   17  /* 第二行时间显示的Y坐标 */
#define Time_Height  16  /* 时间显示的高度 */

/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
WM_HWIN hWin;           /* ICONVIEW的句柄 */
WM_HWIN hWinTaskBar;    /* 任务栏句柄 */
uint8_t OnICON0  = 0;   /* ICONVIEW控件按下的标志，0表示未按下，1表示按下 */
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

/* 用于桌面ICONVIEW图标的创建 */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

/* 用于桌面ICONVIEW图标的创建 */
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
*				                         对话框初始化选项
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
*	功能说明: 文件系统初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void File_Init(void)
{
	/* 方便以后修改图标的位置 */
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
*	功能说明: 显示RTC时间
*	形    参：pMsg 指针参数            
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
*	功能说明: 任务栏的重绘函数 
*	形    参：pMsg 指针参数
*	返 回 值: 无
*********************************************************************************************************
*/
void PaintDialogMain(WM_MESSAGE * pMsg)
{
	GUI_SetBkColor(GUI_WHITE); 
    GUI_Clear(); 
	
	/* 绘制启动小图标 */
	GUI_DrawBitmap(&bmStart, 
	               Imag_LBorder, 
	               Imag_TBorder);
	
	/* 绘制喇叭小图标 */
	GUI_DrawBitmap(&bmSpeaker, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width + Imag_LBorder, 
	               Imag_TBorder);
	
	/* 绘制信号小图标 */
	GUI_DrawBitmap(&bmsignal, 
	               LCD_GetXSize() - Time_Width - ImagSpeak_Width - ImagSignal_Width  + Imag_LBorder, 
	               Imag_TBorder);	
}

/*
*********************************************************************************************************
*	函 数 名: InitDialogMain
*	功能说明: 对话框初始化
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
*	形    参：pMsg
*	返 回 值: 无
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
*	函 数 名: _cbBkWindow
*	功能说明: 桌面窗口的回调函数 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
__IO uint8_t prevent_refresh = 0;  /* 这个参数比较重要，用于防止ICON控件点击下时就触发回调函数的WM_PAINT消息*/
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
					/* ICON控件点击消息 */
					case WM_NOTIFICATION_CLICKED:
						prevent_refresh = 1;
						break;
					
					case WM_NOTIFICATION_SCROLL_CHANGED:
						prevent_refresh = 0;
						break;
					
					/* ICON控件释放消息 */
					case WM_NOTIFICATION_RELEASED: 
						
					    /* 打开相应选项 */
						switch(ICONVIEW_GetSel(pMsg->hWinSrc))
						{
							/* 我的电脑界面*******************************************************************/
                            case 0:	
								OnICON0 = 1;
								OnICON0Clicked();			
								break;	
                            
                            /* 设置界面***********************************************************************/
                            case 1:
                                OnICON1 = 1;
								OnICON1Clicked();
								break;
                            
                            /* AD7606界面*********************************************************************/
                            case 2:
							    OnICON2 = 1;
								OnICON2Clicked();	
								break;
                            
                            /* 图片浏览界面********************************************************************/
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
                            
                            /* 3D界面**********************************************************************/
                            case 4:
                                OnICON4 = 1;
								OnICON4Clicked();
								break;
                            
                            /* 蓝牙界面********************************************************************/
                            case 5:
								break;
                            
                            /* 摄像头界面******************************************************************/
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
                            
							/* 时钟界面*******************************************************************/
                            case 7:
								break;
                            
                            /* FM/AM界面******************************************************************/
                            case 8:
                                OnICON8 = 1;
								OnICON8Clicked();
								break;
                             
                             /* GPS界面******************************************************************/
                            case 9:
								break;
                             
                             /* MP3界面******************************************************************/
                            case 10:
								break;
                             
                            /* NET界面*******************************************************************/
                            case 11:
								OnICON11 = 1;
								OnICON11Clicked();
								break;
                            
							/* 录音机*******************************************************************/
                            case 12:
                                OnICON12 = 1;
								OnICON12Clicked();
								break;
                            
							/* 传感器界面***************************************************************/
                            case 13:
                                OnICON13 = 1;
								OnICON13Clicked();
								break;

                            /* 文本界面****************************************************************/
                            case 14:
								break;
                            
                            /* USB********************************************************************/
                            case 15:
								break;
                            
							/* Video界面**************************************************************/
                            case 16:
								break;
                            
							/* WIFI界面***************************************************************/
                            case 17:
								break;
                            
                            /* wireless界面**********************************************************/
                            case 18:
								break;
							
                            /* signal界面************************************************************/
                            case 19:
								break;								
						}	
					 break;
				}
			break;
		}
		break;
		
		/* 重绘消息*/
		case WM_PAINT:
			if(g_LcdWidth == 800)
			{
				if(prevent_refresh == 0)
				{ 
					GUI_SetBkColor(GUI_BLUE);
					GUI_Clear();
					GUI_SetFont(&GUI_FontHZ_SimSun_16);
					GUI_SetColor(GUI_WHITE);
	 				GUI_DispStringHCenterAt("按键K2用于触摸校准,电容屏无需校准", LCD_GetXSize()/2, LCD_GetYSize() - 54);
					prevent_refresh = 1;
				}	
			}
			else
			{
				GUI_SetBkColor(GUI_BLUE);
				GUI_Clear();
				GUI_SetFont(&GUI_FontHZ_SimSun_16);
				GUI_SetColor(GUI_WHITE);
				GUI_DispStringHCenterAt("按键K2用于触摸校准,电容屏无需校准", LCD_GetXSize()/2, LCD_GetYSize() - 54);
			}
			
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}
							   
/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{	
	uint8_t i;
	
	OS_ERR      err;
	CPU_TS		ts;
	OS_MSG_SIZE	msg_size;
	uint8_t	   *p_msg;
	
	uint16_t ICONVIEW_HNum = 0;  /* ICONVIEW在水平方向的图像数 */
	uint16_t ICONVIEW_VNum = 0;  /* ICONVIEW在垂直方向的图像数 */
	
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
	 * 在所有的窗口上自动的使用内存设备，在GUI_Init后调用不包括
	 * 背景窗口（桌面窗口），之前调用才包括背景窗口 
	 */
 	WM_SetCreateFlags(WM_CF_MEMDEV);			
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	
	/* 设置ICONVIEW的显示位置 ********************************************************************/
	ICONVIEW_VNum = (LCD_GetYSize() - ICONVIEW_TBorder - ICONVIEW_BBorder) / ICONVIEW_Height;
	ICONVIEW_HNum = (LCD_GetXSize() - ICONVIEW_LBorder - ICONVIEW_RBorder) / ICONVIEW_Width;
	
	/*在指定位置创建指定尺寸的ICONVIEW 小工具*/
	hWin = ICONVIEW_CreateEx(ICONVIEW_TBorder, 					/* 小工具的最左像素（在父坐标中）*/
						     ICONVIEW_LBorder, 					/* 小工具的最上像素（在父坐标中）*/
							 ICONVIEW_HNum * ICONVIEW_Width,    /* 小工具的水平尺寸（单位：像素）*/
							 ICONVIEW_VNum * ICONVIEW_Height, 	/* 小工具的垂直尺寸（单位：像素）*/
	                         WM_HBKWIN, 				        /* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
							 WM_CF_SHOW | WM_CF_HASTRANS,       /* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */ 
	                         0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* 默认是0，如果不够现实可设置增减垂直滚动条 */
							 GUI_ID_ICONVIEW0, 			        /* 小工具的窗口ID */
							 ICONVIEW_Width, 				    /* 图标的水平尺寸 */
							 ICONVIEW_Height - ICONVIEW_YSpace);/* 图标的垂直尺寸 */
	
	
    /* 如果显示屏中装不下这么多的图标，加下拉滚动条 */
	if(ICONVIEW_VNum * ICONVIEW_HNum < ICONVIEW_ImagNum )
	{
		SCROLLBAR_SetWidth(SCROLLBAR_CreateAttached(hWin,SCROLLBAR_CF_VERTICAL),16);
	}
	
	/* 向ICONVIEW 小工具添加新图标 */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) 
	{
		
		ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[i].pBitmap, _aBitmapItem[i].pText);
	}
	
	/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
	ICONVIEW_SetBkColor(hWin, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);
	
	/* 设置字体 */
	ICONVIEW_SetFont(hWin, &GUI_Font16B_ASCII);
	
	/* 设置图标在x 或y 方向上的间距。*/
	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
	
	/* 设置对齐方式 在5.22版本中最新加入的 */
	ICONVIEW_SetIconAlign(hWin, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);

	/* 创建一个对话框,类型XP系统的任务栏 */
	hWinTaskBar = GUI_CreateDialogBox(_aDialogCreateMain, 
	                            GUI_COUNTOF(_aDialogCreateMain), 
								&_cbCallbackMain, 
								0, 
								0, 
								0);

	/* 
	 * 创建定时器，其功能是经过指定周期后，向指定窗口发送消息。
	 * 该定时器与指定窗口相关联。 
	 */
	WM_CreateTimer(hWinTaskBar,  /* 接受信息的窗口的句柄 */
	               0, 	         /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
				   1000,         /* 周期，此周期过后指定窗口应收到消息*/
				   0);	         /* 留待将来使用，应为0 */

	while (1) 
	{
		/* 用于接收按键，如果接收到，执行触摸校准 */
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

/*
*********************************************************************************************************
*	                                  
*	模块名称 : 文件管理
*	文件名称 : App_FileManage.c
*	版    本 : V1.2
*	说    明 : 文件系统管理，支持SD, NANDFLASH, U盘
*	修改记录 :
*		版本号    日期          作者         说明
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      1. 更改主界面同时支持4.3寸，5寸和7寸屏的显示
*                                           2. 删除MENU控件
*       v1.2    2014-07-19    Eric2013      1. 配合主界面实现快速的背景刷新
*                                            
*	
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
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
#define File_DropDownHeight    30   /* 控件dropdown的高度 */
#define File_ProgBarStartX     55   /* 控件progbar的起始x */
#define File_ProgBarStartY     70   /* 控件progbar的起始y */
#define File_ProgBarWidth      170  /* 控件progbar的宽度  */
#define File_ProgBarHeight     25   /* 控件progbar的高度  */
#define File_ProgBarCap        30   /* 控件progbar的间距  */

#define Imag_StartX  11   /* 图标的起始X */
#define Imag_StartY  35   /* 图标的起始Y */
#define Imag_YCap    30   /* 图标的间距 */

/*
*********************************************************************************************************
*	函 数 名: Ext_GetFree
*	功能说明: 得到磁盘剩余容量
*	形    参： drv    磁盘编号("0:"/"1:")
*              total  总容量（单位KB）
*              free   剩余容量	 （单位KB）
*	返 回 值: 0,正常.其他,错误代码		        
*********************************************************************************************************
*/
uint8_t Ext_GetFree(uint8_t *drv, uint32_t *total, uint32_t *free)
{
	FATFS *FS;
	uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv, (DWORD *)&fre_clust, &FS);
    if(res==0)
	{											   
	    tot_sect = (FS->n_fatent - 2)* FS->csize;	//得到总扇区数
	    fre_sect = fre_clust * FS->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				    //扇区大小不是512字节,则转换为512字节
		tot_sect *= FS->ssize / 512;
		fre_sect *= FS->ssize / 512;
#endif	  
		*total = tot_sect>>1;	//单位为KB
		*free = fre_sect>>1;	//单位为KB 
 	}
	return res;
}

/*
*********************************************************************************************************
*							对话框初始化选项		        
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
*	函 数 名: PaintDialogFile
*	功能说明: 文件管理界面的重绘  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void PaintDialogFile(WM_MESSAGE * pMsg)
{

}

/*
*********************************************************************************************************
*	函 数 名: InitDialogFile
*	功能说明: 初始化对话框  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void InitDialogFile(WM_MESSAGE * pMsg)
{
    char buf[25];
    WM_HWIN hWin = pMsg->hWin;
	uint32_t DiskFree, DiskTotal;
	WM_HWIN hClient;
	
	/* 重新设置对话框的显示位置和大小 */
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
	
	
	hClient = WM_GetClientWindow(hWin);
	
	/* 设置控件DROPDOWN0的位置 */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0), 
					WM_GetWindowOrgX(hClient),  
					WM_GetWindowOrgY(hClient),  
					LCD_GetXSize() - 10,
					File_DropDownHeight);
	
	/* 设置控件PROGBAR0的位置 */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR0), 
					WM_GetWindowOrgX(hClient) + File_ProgBarStartX,  
					WM_GetWindowOrgY(hClient) + File_ProgBarStartY,  
					File_ProgBarWidth,
					File_ProgBarHeight);
					
	/* 设置控件PROGBAR1的位置 */
	WM_SetWindowPos(WM_GetDialogItem(hWin,GUI_ID_PROGBAR1), 
					WM_GetWindowOrgX(hClient) + File_ProgBarStartX,  
					WM_GetWindowOrgY(hClient) + File_ProgBarStartY + File_ProgBarCap,  
					File_ProgBarWidth,
					File_ProgBarHeight);
					
	/* 设置控件PROGBAR2的位置 */
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
*	函 数 名: _cbCallbackSYS
*	功能说明: SYS对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无		        
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
			/* 绘制用水平颜色梯度填充的矩形 */
			GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  xSize - 1,	   /* 右下角X 位置 */
							  ySize - 1,  	   /* 右下角Y 位置 */
							  GUI_WHITE,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_LIGHTBLUE);  /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  60,			   /* 左上角Y 位置 */
							  400,	           /* 右下角X 位置 */
							  61,              /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  185,			   /* 左上角Y 位置 */
							  400,	           /* 右下角X 位置 */
							  186,             /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */

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
					
                /* 初始化USB HOST */
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
					
                /*  关闭USB HOST */
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
					
                /* 初始化USB DEVICE */
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
					
                /* 关闭USB DEVICE */
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
*	函 数 名: OnICON0Clicked
*	功能说明: 主函数
*	形    参：pMsg
*	返 回 值: 无
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

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

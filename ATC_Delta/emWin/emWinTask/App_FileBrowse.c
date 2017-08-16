/*
*********************************************************************************************************
*	                                  
*	模块名称 : 文件浏览
*	文件名称 : App_FileBrowse.c
*	版    本 : V1.2
*	说    明 : 这里主要演示文件管理控件的使用，不过这个使用这个控件过程中
*              存在两个问题。
*			   1. 打开文件的时候，这个控件默认会在文件的后面加一个反斜杠
*                 而这个反斜杠，比如0：\PIC\，在使用fatfs的时候是打不开的
                  所以要把这个我去掉。
			   2. 还有一个BUG就是，文件的大小显示，这个控件将文件大小缩小了
			      1024倍，比如KB代表MB。
				这两个问题在程序里面都得到了修正
*	修改记录 :
*		版本号    日期          作者         说明
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      1. 更改主界面同时支持4.3寸，5寸和7寸屏的显示
*                                           2. 删除MENU控件
*       v1.2    2014-07-19    Eric2013      1. 配合主界面实现快速的背景刷新
*       v1.2c   2014-07-19    Eric2013      1. 通过函数CHOOSEFILE_SetButtonText修改按键大小，以便电容触摸屏可以点击到。
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
#define _MAX_PATH 256
#define _MAX_EXT  256

/*
********************************************************************
*
*       Static data
*
**********************************************************************
*/
static struct {
  U32 Mask;
  char c;
} _aAttrib[] = {
{AM_RDO,      'R'},
{AM_HID,	  'H'},
{AM_SYS,	  'S'},
{AM_VOL,	  'V'},
{AM_LFN,	  'L'},
{AM_DIR,	  'D'},
{AM_ARC,	  'A'}};


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetExt
*/
static void _GetExt(char * pFile, char * pExt) {
  int Len;
  int i;
  int j;

  //
  // Search beginning of extension
  //
  Len = strlen(pFile);
  for (i = Len; i > 0; i--) {
    if (*(pFile + i) == '.') {
      *(pFile + i) = '\0';     // Cut extension from filename
      break;
    }
  }
  //
  // Copy extension
  //
  j = 0;
  while (*(pFile + ++i) != '\0') {
    *(pExt + j) = *(pFile + i);
    j++;
  }
  *(pExt + j) = '\0';          // Set end of string
}

/*********************************************************************
*
*       _GetData
*/
void FilePath_process1(char * source)
{	
	while(*source != '\0')
	{
		source++;
  	}
	source--;
	*source = '\0';
}

void FilePath_process2(char * source)
{	
	while(*source != '\0')
	{
		source++;
  	}
	*source = '\\';
	source++;
	*source = '\0';
}

static int _GetData2(CHOOSEFILE_INFO * pInfo) {

  static char         acAttrib[20]         = { 0 };
  static char         acExt   [_MAX_EXT];
  char                c;
  int                 i;
  int                 r;
  char                *fn; 	
  OS_ERR      	err;

#if _USE_LFN
	finfo.lfname = acFile;
	finfo.lfsize = _MAX_PATH*2+1;
#endif
  OSSchedLock(&err);
  switch (pInfo->Cmd) {
  case CHOOSEFILE_FINDFIRST:
    FilePath_process1(pInfo->pRoot);
	if (f_opendir(&DirInf, pInfo->pRoot) == FR_OK)
	{
		if(f_readdir(&DirInf, &finfo) == FR_OK)
		{
			#if _USE_LFN
			        fn = *finfo.lfname ? finfo.lfname : finfo.fname;
			#else							   
			        fn = finfo.fname;
			#endif

			r = 0;
		}
		else
		{
			r = 1;
		}
	}
	else
	{
		r = 1;
	}
	FilePath_process2(pInfo->pRoot); 
    break;
  case CHOOSEFILE_FINDNEXT:
    if(f_readdir(&DirInf, &finfo) == FR_OK)
	{
		if(!finfo.fname[0]) 
		{
			r = 1;
			break;
		}
		#if _USE_LFN
		        fn = *finfo.lfname ? finfo.lfname : finfo.fname;
		#else							   
		        fn = finfo.fname;
		#endif

		r = 0;
	}
	else
	{
		r = 1;
	}
    break;
  }
  if (r == 0) {
    pInfo->Flags = (finfo.fattrib & _aAttrib[5].Mask) ? CHOOSEFILE_FLAG_DIRECTORY : 0;
    for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) {
      if (finfo.fattrib & _aAttrib[i].Mask) {
        c = _aAttrib[i].c;
      } else {
        c = '-';
      }
      acAttrib[i] = c;
    }
	if(finfo.fattrib & _aAttrib[5].Mask)
	{
		pInfo->pExt    = "Directory";
	}
	else
	{
		_GetExt(fn, acExt);
		pInfo->pExt    = acExt;	
	}
    
    pInfo->pAttrib = acAttrib;
    pInfo->pName   = fn;
    pInfo->SizeL   = finfo.fsize << 10;
    pInfo->SizeH   = finfo.fsize >> 22;
  }
  OSSchedUnlock(&err);
  return r;

}

/*
*********************************************************************************************************
*	函 数 名: OnICON0Clicked
*	功能说明: 文件系统管理主函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void OnICON00Clicked(uint8_t _ucDriveID)
{
	const char        acMask[]            = "*.*";
	CHOOSEFILE_INFO   Info                = { 0 };
	WM_HWIN           hWin;
	int               r;
	char             acDir[_MAX_PATH]     = "1:";
	char const      * apDrives[1]         = { 0 };


	FRAMEWIN_SetDefaultFont(GUI_FONT_20_ASCII);
	FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);
	LISTVIEW_SetDefaultFont(&GUI_FontHZ_SimSun_16);
	HEADER_SetDefaultFont(GUI_FONT_16_ASCII);
	BUTTON_SetDefaultFont(GUI_FONT_16_ASCII);

	/*
		FS_SD		0
		FS_NAND		1
		FS_USB		2
	*/
	acDir[0] = _ucDriveID + 0x30 - 1;
	apDrives[0] = acDir; 
	Info.pfGetData = _GetData2;
	Info.pMask     = acMask;

	while (OnICON00) 
	{

		// 将按钮设置到对话框的上面
		// CHOOSEFILE_SetTopMode(Cnt % 2);
		CHOOSEFILE_SetTopMode(1);
		hWin = CHOOSEFILE_Create(0, 	       //Handle of parent window.
								 0, 			       //X position in pixels of the  dialog in clie nt coordinates.
								 0, 			       //Y position in pixels of the  dialog in client coordinates.
								 LCD_GetXSize(), 	   //X-size of the dialog in pixels.
								 LCD_GetYSize(),       //Y-size of the dialog in pixels
								 apDrives, 		       //Pointer to an array of strings containing the root directories to be used.
								 GUI_COUNTOF(apDrives),//Number of root directories. 
								 0, 				   //Initial index of the root directory to be used.
								 "File Dialog", 	   //Title to be shown in the title bar.
								 0, 				   //Additional flags for the FRAMEWIN widget.
								 &Info);			   //Pointer to a CHOOSEFILE_INFO structure.
		
		CHOOSEFILE_SetButtonText(hWin, CHOOSEFILE_BI_CANCEL, "Close");

		r = GUI_ExecCreatedDialog(hWin);
		
		// 由 case GUI_ID_OK: GUI_EndDialog(hWin, 0);
		if (r == 0) 
		{
			//点击了Archive属性的文件 会进入到这里，也就是说，可以
			//在这里进入图片显示，MP3播放等功能。
			hWin = MESSAGEBOX_Create(Info.pRoot, "File:", 0);
			GUI_Delay(2000);
			WM_DeleteWindow(hWin);
		}
		// 由case GUI_ID_CANCEL: GUI_EndDialog(hWin, 1);  
		else 
		{
			WM_DeleteWindow(hWin);
			OnICON00 = 0;
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

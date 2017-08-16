/*
*********************************************************************************************************
*	                                  
*	ģ������ : �ļ����
*	�ļ����� : App_FileBrowse.c
*	��    �� : V1.2
*	˵    �� : ������Ҫ��ʾ�ļ�����ؼ���ʹ�ã��������ʹ������ؼ�������
*              �����������⡣
*			   1. ���ļ���ʱ������ؼ�Ĭ�ϻ����ļ��ĺ����һ����б��
*                 �������б�ܣ�����0��\PIC\����ʹ��fatfs��ʱ���Ǵ򲻿���
                  ����Ҫ�������ȥ����
			   2. ����һ��BUG���ǣ��ļ��Ĵ�С��ʾ������ؼ����ļ���С��С��
			      1024��������KB����MB��
				�����������ڳ������涼�õ�������
*	�޸ļ�¼ :
*		�汾��    ����          ����         ˵��
*		v1.0    2013-04-26    Eric2013      �׷�
*       v1.1    2014-05-23    Eric2013      1. ����������ͬʱ֧��4.3�磬5���7��������ʾ
*                                           2. ɾ��MENU�ؼ�
*       v1.2    2014-07-19    Eric2013      1. ���������ʵ�ֿ��ٵı���ˢ��
*       v1.2c   2014-07-19    Eric2013      1. ͨ������CHOOSEFILE_SetButtonText�޸İ�����С���Ա���ݴ��������Ե������
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
*	�� �� ��: OnICON0Clicked
*	����˵��: �ļ�ϵͳ���������� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
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

		// ����ť���õ��Ի��������
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
		
		// �� case GUI_ID_OK: GUI_EndDialog(hWin, 0);
		if (r == 0) 
		{
			//�����Archive���Ե��ļ� ����뵽���Ҳ����˵������
			//���������ͼƬ��ʾ��MP3���ŵȹ��ܡ�
			hWin = MESSAGEBOX_Create(Info.pRoot, "File:", 0);
			GUI_Delay(2000);
			WM_DeleteWindow(hWin);
		}
		// ��case GUI_ID_CANCEL: GUI_EndDialog(hWin, 1);  
		else 
		{
			WM_DeleteWindow(hWin);
			OnICON00 = 0;
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

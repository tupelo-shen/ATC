/*
*********************************************************************************************************
*	                                  
*	ģ������ : ͼƬ���
*	�ļ����� : App_DispPic1.c
*	��    �� : V1.2
*	˵    �� : JPG BMP GIFͼƬ���
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
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/ 
#define BUTTON_StartX        4   /* �ؼ�dropdown�ĸ߶� */
#define BUTTON_StartXCap    66   /* �ؼ�dropdown�ĸ߶� */
#define BUTTON_StartYBorder 36   /* �ؼ�progbar����ʼx */
#define BUTTON_Width        60   /* �ؼ�progbar����ʼx */
#define BUTTON_Height       36   /* �ؼ�progbar����ʼx */


#define g_ucPicNum    10    /* ֧�ֵ�ͼƬ��ʾ���� */

/*
*********************************************************************************************************
*                                         ���� 
*********************************************************************************************************
*/
BUTTON_Handle   g_ahButton[7];

static char BMP_Name[g_ucPicNum][20];
static int8_t BMP_Num=0;
static int8_t BMP_Index=0;

static char JPG_Name[g_ucPicNum][20];
static int8_t JPG_Num=0;
static int8_t JPG_Index=0;

static char GIF_Name[g_ucPicNum][20];
static int8_t GIF_Num=0;
static int8_t GIF_Index=0;

static uint8_t g_ucPicType = 0;

/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow2
*	����˵��: ͼƬ��ʾ���ڵĻص����� 
*	��    �Σ�
*             WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 
void _cbBkWindow2(WM_MESSAGE * pMsg) 
{
	OS_ERR      	err;
	
	switch (pMsg->MsgId) 
    {
		case WM_PAINT:
            _PaintFrame();
            //���ֻ�ǵõ�����
            _ShowBMP(BMP_Name[BMP_Index]);
            OSSchedLock(&err);
			GUI_BMP_Draw(_acBuffer2, 50, 50);
			OSSchedUnlock(&err);
            //BMP_Index--;
        break;
		
		case WM_NOTIFY_PARENT:
            _DeleteFrame();
            _CreateFrame(&_cbMainGUI);
		break;
		default:
		WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: OnICON1Clicked
*	����˵��: ͼƬ��������� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OnICON3Clicked(void)
{ 	
    char *fn;   /* This function is assuming non-Unicode cfg. */
	
	BMP_Num=0;
	BMP_Index=0;
	JPG_Num=0;
	JPG_Index=0;
	
	GIF_Num=0;
	GIF_Index=0;
	g_ucPicType = 0;

	//GUI_SetColor(GUI_RED);
	//GUI_SetTextMode(GUI_TM_TRANS);
	//GUI_SetFont(&GUI_Font24B_ASCII);
	//WM_SetCallback(WM_HBKWIN, _cbBkWindow2);
	
	/*  */
	if (f_opendir(&DirInf, "0:/pic") == FR_OK)
	{
		/*  */
		while (f_readdir(&DirInf, &finfo) == FR_OK)
		{
			if(!finfo.fname[0]) break;

			if (finfo.fattrib & AM_ARC) 
			{
				/* BMP */
				if(strstr(finfo.fname,".BMP")||strstr(finfo.fname,".bmp"))
				{
					if(BMP_Num < 10)
					{												
						#if _USE_LFN
						        fn = *finfo.lfname ? finfo.lfname : finfo.fname;
						#else							   
						        fn = finfo.fname;
						#endif	
						strcpy(BMP_Name[BMP_Num], "0:/pic/");	
						strcat(BMP_Name[BMP_Num++], fn);
					}
				}
				
				/* JPG */
				if(strstr(finfo.fname,".JPG")||strstr(finfo.fname,".jpg"))
				{
					if(JPG_Num < 10)
					{
						#if _USE_LFN
						        fn = *finfo.lfname ? finfo.lfname : finfo.fname;
						#else							   
						        fn = finfo.fname;
						#endif
						strcpy(JPG_Name[JPG_Num], "0:/pic/");	
						strcat(JPG_Name[JPG_Num++], fn);
					}
				}
				
				/* JPG */
				if(strstr(finfo.fname,".GIF")||strstr(finfo.fname,".gif"))
				{
					if(GIF_Num < 10)
					{
						#if _USE_LFN
						        fn = *finfo.lfname ? finfo.lfname : finfo.fname;
						#else							   
						        fn = finfo.fname;
						#endif
						strcpy(GIF_Name[GIF_Num], "0:/pic/");	
						strcat(GIF_Name[GIF_Num++], fn);
					}
				}
			}
	     }							 
	}
	else
	{
		return;	
	}
	
// 	/* ��һ��ͼ */
// 	g_ahButton[0] = BUTTON_Create(BUTTON_StartX, 
// 	                              LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height, 
// 	                              GUI_ID_BUTTON0,  
// 	                              BUTTON_CF_SHOW);
// 	BUTTON_SetText(g_ahButton[0], "PRE");

// 	/* ��һ��ͼ */
// 	g_ahButton[1] = BUTTON_Create(BUTTON_StartX+BUTTON_StartXCap, 
// 	                              LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height, 
// 	                              GUI_ID_BUTTON1,  
// 	                              BUTTON_CF_SHOW);
// 	BUTTON_SetText(g_ahButton[1], "NEXT");
// 	
// 	/* ͼƬ�л� */
// 	g_ahButton[2] = BUTTON_Create(BUTTON_StartX+BUTTON_StartXCap*2,
// 								  LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height, 
// 								  GUI_ID_BUTTON2,  
// 								  BUTTON_CF_SHOW);
// 								  BUTTON_SetText(g_ahButton[2], "BMP");

// 	/* ����ͼƬ���� */
// 	g_ahButton[3] = BUTTON_Create(BUTTON_StartX+BUTTON_StartXCap*3,
// 								  LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height,
// 								  GUI_ID_BUTTON3,  
// 								  BUTTON_CF_SHOW);
// 								  BUTTON_SetText(g_ahButton[3], "BackPic");
//   
// 	/* �Ŵ� */
// 	g_ahButton[4] = BUTTON_Create(BUTTON_StartX+BUTTON_StartXCap*4,
// 								  LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height,
// 								  GUI_ID_BUTTON4,  
// 								  BUTTON_CF_SHOW);
// 								  BUTTON_SetText(g_ahButton[4], "AMP");

//     /* ��С */
// 	g_ahButton[5] = BUTTON_Create(BUTTON_StartX+BUTTON_StartXCap*5,
// 								  LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height,
// 								  GUI_ID_BUTTON5,  
// 								  BUTTON_CF_SHOW);
// 								  BUTTON_SetText(g_ahButton[5], "REDUCE");

//      /* ��С */
// 	g_ahButton[6] = BUTTON_Create(BUTTON_StartX+BUTTON_StartXCap*6,
// 								  LCD_GetYSize() - BUTTON_StartYBorder,
// 	                              BUTTON_Width,
// 	                              BUTTON_Height,
// 								  GUI_ID_OK,  
// 								  BUTTON_CF_SHOW);
// 								  BUTTON_SetText(g_ahButton[6], "RETURN");

// 	while(OnICON3)
// 	{					 				
// 		if(g_ucPicType == 2)
// 		{
//     		if(i < InfoGif1.NumImages)
//     	    {                                    
//     			OSSchedLock(&err);
//     	        //GUI_GIF_GetImageInfo(_acBuffer2, file.fsize, &InfoGif2, i);
//                 GUI_GIF_DrawSub(_acBuffer2, 
//                                 file.fsize, 
//                                 (LCD_GetXSize() - InfoGif1.xSize)/2, 
//                         		(LCD_GetYSize() - InfoGif1.ySize)/2, 
//                                 i++);                             
//     	        OSSchedUnlock(&err); 
//                 WM_ShowWindow(g_ahButton[0]);
//     			WM_ShowWindow(g_ahButton[1]);
//     			WM_ShowWindow(g_ahButton[2]);
//     			WM_ShowWindow(g_ahButton[3]);
//     			WM_ShowWindow(g_ahButton[4]);
//     			WM_ShowWindow(g_ahButton[5]);
//     			WM_ShowWindow(g_ahButton[6]);
//                 if(InfoGif2.Delay == 0)
//                 {
//                     GUI_Delay(100);
//                 }
//                 else
//                 {
//                     GUI_Delay(InfoGif2.Delay*10);
//                 }                          						 
//     	    }
//     	    else
//     	    {
//     	        i = 0;
//     	    }
// 		}
// 		else
// 		{	
// 			WM_ShowWindow(g_ahButton[0]);
// 			WM_ShowWindow(g_ahButton[1]);
// 			WM_ShowWindow(g_ahButton[2]);
// 			WM_ShowWindow(g_ahButton[3]);
// 			WM_ShowWindow(g_ahButton[4]);
// 			WM_ShowWindow(g_ahButton[5]);
// 			WM_ShowWindow(g_ahButton[6]);
// 			GUI_Delay(10); 
// 		}
// 	}
	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

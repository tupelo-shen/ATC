/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI各个部分的总头文件
*	文件名称 : MainTask.c
*	版    本 : V1.2
*	说    明 : GUI界面主函数
*	修改记录 :
*		版本号    日期          作者        说明
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      删除部分代码
*       v1.2    2014-07-19    Eric2013      主要优化背景图片刷新速度和准确而灵敏的触摸效果
*	
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MainTask_H
#define __MainTask_H

#include "stdlib.h"
#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"
#include "MESSAGEBOX.h"

#include "ff.h"
#include "diskio.h"
#include "param.h"

/*
************************************************************************
*						  FatFs
************************************************************************
*/
extern FRESULT result;
extern FIL file;
extern FILINFO finfo;
extern DIR DirInf;
extern UINT bw;
extern FATFS fs;
extern FATFS fs_nand;
extern FATFS fs_usb;
//extern char _acBuffer[0x2000];
extern char *_acBuffer2;

extern void _WriteByte2File(U8 Data, void * p);

/* 用于BMP图片的显示 */
uint8_t  _ShowBMP(const char * sFilename); 

extern GUI_JPEG_INFO JpegInfo;
extern GUI_GIF_INFO InfoGif1;
extern GUI_GIF_IMAGE_INFO InfoGif2;
/*
************************************************************************
*						emWin字体
************************************************************************
*/
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_12;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_FangSong_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Hei_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Kai_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_1616;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_2424;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_16;

extern GUI_CONST_STORAGE GUI_BITMAP bmComputer;
extern GUI_CONST_STORAGE GUI_BITMAP bmSettings;
extern GUI_CONST_STORAGE GUI_BITMAP bmAD7606;	
extern GUI_CONST_STORAGE GUI_BITMAP bmPicture;

extern GUI_CONST_STORAGE GUI_BITMAP bm3D;
extern GUI_CONST_STORAGE GUI_BITMAP bmBluetooth;
extern GUI_CONST_STORAGE GUI_BITMAP bmCamera;	
extern GUI_CONST_STORAGE GUI_BITMAP bmClock;

extern GUI_CONST_STORAGE GUI_BITMAP bmFMAM;
extern GUI_CONST_STORAGE GUI_BITMAP bmGPS;
extern GUI_CONST_STORAGE GUI_BITMAP bmMP3;	
extern GUI_CONST_STORAGE GUI_BITMAP bmNet;

extern GUI_CONST_STORAGE GUI_BITMAP bmRecorder;
extern GUI_CONST_STORAGE GUI_BITMAP bmSensor;
extern GUI_CONST_STORAGE GUI_BITMAP bmText;	
extern GUI_CONST_STORAGE GUI_BITMAP bmUSB;

extern GUI_CONST_STORAGE GUI_BITMAP bmVedio;
extern GUI_CONST_STORAGE GUI_BITMAP bmWIFI;
extern GUI_CONST_STORAGE GUI_BITMAP bmWireless;	
extern GUI_CONST_STORAGE GUI_BITMAP bmSignal;

extern GUI_CONST_STORAGE GUI_BITMAP	bmStart;
extern GUI_CONST_STORAGE GUI_BITMAP	bmSpeaker;
extern GUI_CONST_STORAGE GUI_BITMAP	bmsignal;

extern  GUI_CONST_STORAGE GUI_BITMAP bmfileusb;
extern  GUI_CONST_STORAGE GUI_BITMAP bmfiledisk;
extern  GUI_CONST_STORAGE GUI_BITMAP bmfilesd;
extern  GUI_CONST_STORAGE GUI_BITMAP bmfilecamera;

extern void Touch_Calibrate(void);

extern uint8_t OnICON0;
extern void OnICON0Clicked(void);

extern uint8_t OnICON00;
extern void OnICON00Clicked(uint8_t _ucDriveID);

extern uint8_t OnICON1;
extern void OnICON1Clicked(void);

extern uint8_t OnICON2;
extern void OnICON2Clicked(void);

extern uint8_t OnICON3;
extern void OnICON3Clicked(void);

extern uint8_t OnICON4;
extern void OnICON4Clicked(void);

extern uint8_t OnICON5;
extern void OnICON5Clicked(void);

extern uint8_t OnICON6;
extern void OnICON6Clicked(void);

extern uint8_t OnICON7;
extern void OnICON7Clicked(void);

extern uint8_t OnICON8;
extern void OnICON8Clicked(void);

extern uint8_t OnICON9;
extern void OnICON9Clicked(void);

extern uint8_t OnICON10;
extern void OnICON10Clicked(void);

extern uint8_t OnICON11;
extern void OnICON11Clicked(void);

extern uint8_t OnICON12;
extern void OnICON12Clicked(void);

extern uint8_t OnICON13;
extern void OnICON13Clicked(void);

extern uint8_t OnICON14;
extern void OnICON14Clicked(void);

extern uint8_t OnICON15;
extern void OnICON15Clicked(void);

extern uint8_t OnICON16;
extern void OnICON16Clicked(void);

extern uint8_t OnICON17;
extern void OnICON17Clicked(void);

extern uint8_t OnICON18;
extern void OnICON18Clicked(void);

extern uint8_t OnICON19;
extern void OnICON19Clicked(void);

extern void _cbBkWindow2(WM_MESSAGE * pMsg);

typedef enum {
  USB_IDLE   = 0,
  USB_WAIT,  
  USB_DEVICE,
  USB_HOST,
}USB_State;

extern WM_HWIN hFrame;
extern uint8_t g_ucState;
extern uint8_t g_ucBacPicBuff[40];
extern GRAPH_SCALE_Handle hScaleV;         /* Handle of vertical scale */
extern GRAPH_SCALE_Handle hScaleH;         /* Handle of horizontal scale */

extern GRAPH_DATA_Handle  ahData1[3];       /* Array of handles for the GRAPH_DATA objects */
extern GRAPH_DATA_Handle  ahData2[3];       /* Array of handles for the GRAPH_DATA objects */

extern GUI_COLOR _aColor[3];

extern PARAM_T g_tParam;

extern  __IO uint8_t prevent_refresh;
#endif

/*****************************(END OF FILE) *********************************/

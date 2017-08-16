/*
*********************************************************************************************************
*
*	模块名称 : web server 演示
*	文件名称 : App_Net.c
*	版    本 : V1.1
*	说    明 : 提供WEB服务器功能。主要目的是测试DM9000AEP网卡电路和DM9161 PHY电路。
*	修改记录 :
*		版本号    日期          作者         说明
*		v1.0    2013-04-26    Eric2013      首发
*       v1.1    2014-05-23    Eric2013      1. 更改主界面同时支持4.3寸，5寸和7寸屏的显示
*       v1.2    2014-07-19    Eric2013      1. 配合主界面实现快速的背景刷新
*	
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "stdio.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"
#include "dm9k_uip.h"
#include "includes.h"
#include "includes.h"
#include "MainTask.h"

/* for LwIP */
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main_lwip.h"
#include "httpd_w.h"


#define GUI_ID_TEXT10             (GUI_ID_USER +  0)
#define GUI_ID_TEXT11             (GUI_ID_USER +  1)
#define GUI_ID_TEXT12             (GUI_ID_USER +  2)


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

/* 缺省IP地址, 保存在Flash参数区 */

struct timer periodic_timer, arp_timer;

void uip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate);
void uip_pro(void);

void lwip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate);
void lwip_pro(void);


extern PARAM_T g_tParam;
extern __IO uint32_t  EthInitStatus;
extern __IO uint8_t EthLinkStatus;

/*
*********************************************************************************************************
*	函 数 名: WebServer
*	功能说明: web服务器程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void WebServer(void)
{

	
}


/*
*********************************************************************************************************
*	下面是 uIP 部分的代码
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: uip_start
*	功能说明: 启动uip
*	形    参: _uiIP
*	返 回 值: 无
*********************************************************************************************************
*/
void uip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate)
{
	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	tapdev_init();	/* 初始化网卡硬件 */
	uip_init();

	//uip_ipaddr(ipaddr, 192,168,1,10);	/* 缺省IP */

	uip_sethostaddr((uint16_t *)_IP);

	//uip_ipaddr(ipaddr, 255,255,255,0);	/* 地址掩码 */
	uip_setnetmask((uint16_t *)_NetMask);

	//uip_ipaddr(ipaddr, 192,168,1,1);	/* 默认网关 */
	uip_setdraddr((uint16_t *)_NetGate);

	httpd_init();
}

/*
*********************************************************************************************************
*	函 数 名: uip_pro
*	功能说明: uip 轮询，插入到主循环中
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void uip_pro(void)
{
	uint16_t i;

	uip_len = tapdev_read();
	if(uip_len > 0)
	{
		if (BUF->type == htons(UIP_ETHTYPE_IP))
		{
			uip_arp_ipin();
			uip_input();
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if (uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}
		else if (BUF->type == htons(UIP_ETHTYPE_ARP))
		{
			uip_arp_arpin();
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if (uip_len > 0)
			{
				tapdev_send();
			}
		}
	}
	else if(timer_expired(&periodic_timer))
	{
		//printf("  0.5s periodic timer expired\n\r");
		timer_reset(&periodic_timer);
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}

		#if UIP_UDP
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}
		#endif /* UIP_UDP */

		/* Call the ARP timer function every 10 seconds. */
		if (timer_expired(&arp_timer))
		{
			//printf("  10s periodic timer expired\n\r");
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}

/*---------------------------------------------------------------------------*/
void uip_log(char *m)
{
	printf("uIP log message: %s\n", m);
}

void resolv_found(char *name, u16_t *ipaddr)
{
	//  u16_t *ipaddr2;

	if (ipaddr == NULL)
	{
		printf("Host '%s' not found.\n", name);
	}
	else
	{
		printf("Found name '%s' = %d.%d.%d.%d\n", name,
		htons(ipaddr[0]) >> 8,
		htons(ipaddr[0]) & 0xff,
		htons(ipaddr[1]) >> 8,
		htons(ipaddr[1]) & 0xff);
		/*    webclient_get("www.sics.se", 80, "/~adam/uip");*/
	}
}

#ifdef __DHCPC_H__
	void dhcpc_configured(const struct dhcpc_state *s)
	{
		uip_sethostaddr(s->ipaddr);
		uip_setnetmask(s->netmask);
		uip_setdraddr(s->default_router);
		resolv_conf(s->dnsaddr);
	}
#endif /* __DHCPC_H__ */

void smtp_done(unsigned char code)
{
	printf("SMTP done with code %d\n", code);
}
void webclient_closed(void)
{
	printf("Webclient: connection closed\n");
}
void webclient_aborted(void)
{
	printf("Webclient: connection aborted\n");
}

void webclient_timedout(void)
{
	printf("Webclient: connection timed out\n");
}

void webclient_connected(void)
{
	printf("Webclient: connected, waiting for data...\n");
}

void webclient_datahandler(char *data, u16_t len)
{
	printf("Webclient: got %d bytes of data.\n", len);
}

/*
*********************************************************************************************************
*	下面是 LwIP 部分的代码
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: uip_start
*	功能说明: 启动uip
*	形    参: _uiIP
*	返 回 值: 无
*********************************************************************************************************
*/
void lwip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate)
{
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */
	ETH_BSP_Config();

	/* Initilaize the LwIP stack */
	LwIP_Init();

	/* Http webserver Init */
	httpd_init_w();
}

/*
*********************************************************************************************************
*	函 数 名: lwip_pro
*	功能说明: lwip 轮询，插入到主循环中
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void lwip_pro(void)
{	
	OS_ERR      err;
	/* check if any packet received */
	if (ETH_CheckFrameReceived())
	{
		/* process received ethernet packet */
		LwIP_Pkt_Handle();
	}

	/* handle periodic timers for LwIP */
	
	LwIP_Periodic_Handle(OSTimeGet(&err) / 10);
}

/*
*********************************************************************************************************
*							对话框初始化选项		        
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateNet[] = {
    { FRAMEWIN_CreateIndirect,  "Webserver",                  0,               0,  0,  800, 444, 0, 0},
	{ TEXT_CreateIndirect,      "LwIP DM9161",           GUI_ID_TEXT0,         15, 36, 300,24, 0,0},  
	{ TEXT_CreateIndirect,      "UIP DM9000",            GUI_ID_TEXT1,         15, 156, 300,24, 0,0}, 

	{ TEXT_CreateIndirect,      "LwIP DM9161",           GUI_ID_TEXT8,         200, 36, 300,24, 0,0},  
	{ TEXT_CreateIndirect,      "UIP DM9000",            GUI_ID_TEXT9,         200, 156, 300,24, 0,0}, 

    { TEXT_CreateIndirect,      "IP   : 192.168.1.12",   GUI_ID_TEXT2,       15, 52 + 13, 192,16, 0,0},
    { TEXT_CreateIndirect,      "MASK : 255.255.255.0",  GUI_ID_TEXT3,       15, 74 + 13, 184,16, 0,0},
    { TEXT_CreateIndirect,      "GATE : 192.168.1.1",    GUI_ID_TEXT4,       15, 96 + 13, 128,16, 0,0},

    { TEXT_CreateIndirect,      "IP   : 192.168.1.11",   GUI_ID_TEXT5,       15, 197 - 12,160,16, 0,0},
    { TEXT_CreateIndirect,      "MASK : 255.255.255.0",  GUI_ID_TEXT6,       15, 222 - 12,120,16, 0,0},
    { TEXT_CreateIndirect,      "GATE : 192.168.1.1",    GUI_ID_TEXT7,       15, 247 - 12,200, 16, 0,0}
};

/*
*********************************************************************************************************
*	函 数 名: PaintDialogFile
*	功能说明: 文件管理界面的重绘  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void PaintDialogNet(WM_MESSAGE * pMsg)
{
//    WM_HWIN hWin = pMsg->hWin;

}

/*
*********************************************************************************************************
*	函 数 名: InitDialogFile
*	功能说明: 初始化对话框  
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
void InitDialogNet(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	uint32_t id;
	
	WM_SetWindowPos(hWin,0, 0,LCD_GetXSize(), LCD_GetYSize());
    //
    //FRAMEWIN
    //
	FRAMEWIN_SetFont(hWin, &GUI_Font24B_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_SetTitleHeight(hWin,28);


	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font24B_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font24B_ASCII);

    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2),&GUI_Font16_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT5),&GUI_Font16_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT6),&GUI_Font16_ASCII);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT7),&GUI_Font16_ASCII);
	
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT8),&GUI_Font24B_ASCII);
	TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT9),&GUI_Font24B_ASCII);

	id = dm9k_ReadID();

	if (id == 0x0A469000)
	{
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT9), "Success");
	}
	else
	{
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT9), "Fail");

	}
	
	if (EthInitStatus == 0)
	{
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT8), "Fail");
	}
	else
	{
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT8), "Success");
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackSYS
*	功能说明: SYS对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无		        
*********************************************************************************************************
*/
static void _cbCallbackNet(WM_MESSAGE * pMsg) 
{
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
		    OnICON11 = 0;
			prevent_refresh = 0;
		    break;
		case WM_PAINT:
			/* 绘制用水平颜色梯度填充的矩形 */
			GUI_DrawGradientV(0,			   /* 左上角X 位置 */
							  0,			   /* 左上角Y 位置 */
							  xSize - 1,	   /* 右下角X 位置 */
							  ySize - 1,  /* 右下角Y 位置 */
							  GUI_WHITE,	   /* 矩形最左侧要绘制的颜色 */
							  GUI_LIGHTBLUE);  /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  60,			   /* 左上角Y 位置 */
							  400,	           /* 右下角X 位置 */
							  61,              /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
							  180,			   /* 左上角Y 位置 */
							  400,	           /* 右下角X 位置 */
							  181,              /* 右下角Y 位置 */
							  GUI_RED,	       /* 矩形最左侧要绘制的颜色 */
							  GUI_YELLOW);     /* 矩形最右侧要绘制的颜色 */

            PaintDialogNet(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialogNet(pMsg);
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
    }

	WM_DefaultProc(pMsg);	
}

/*
*********************************************************************************************************
*	函 数 名: OnICON11Clicked
*	功能说明: 主函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void OnICON11Clicked(void)
{ 	
	hFrame = GUI_CreateDialogBox(_aDialogCreateNet, 
                                 GUI_COUNTOF(_aDialogCreateNet), 
                                 &_cbCallbackNet, 
                                 0, 
                                 0, 
                                 0); 
	while(OnICON11)
	{	 				 				
		GUI_Delay(10);     				
	}
	
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : web server ��ʾ
*	�ļ����� : App_Net.c
*	��    �� : V1.1
*	˵    �� : �ṩWEB���������ܡ���ҪĿ���ǲ���DM9000AEP������·��DM9161 PHY��·��
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

/* ȱʡIP��ַ, ������Flash������ */

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
*	�� �� ��: WebServer
*	����˵��: web����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WebServer(void)
{

	
}


/*
*********************************************************************************************************
*	������ uIP ���ֵĴ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: uip_start
*	����˵��: ����uip
*	��    ��: _uiIP
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void uip_start(uint8_t *_IP, uint8_t *_NetMask, uint8_t *_NetGate)
{
	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	tapdev_init();	/* ��ʼ������Ӳ�� */
	uip_init();

	//uip_ipaddr(ipaddr, 192,168,1,10);	/* ȱʡIP */

	uip_sethostaddr((uint16_t *)_IP);

	//uip_ipaddr(ipaddr, 255,255,255,0);	/* ��ַ���� */
	uip_setnetmask((uint16_t *)_NetMask);

	//uip_ipaddr(ipaddr, 192,168,1,1);	/* Ĭ������ */
	uip_setdraddr((uint16_t *)_NetGate);

	httpd_init();
}

/*
*********************************************************************************************************
*	�� �� ��: uip_pro
*	����˵��: uip ��ѯ�����뵽��ѭ����
*	��    ��: ��
*	�� �� ֵ: ��
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
*	������ LwIP ���ֵĴ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: uip_start
*	����˵��: ����uip
*	��    ��: _uiIP
*	�� �� ֵ: ��
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
*	�� �� ��: lwip_pro
*	����˵��: lwip ��ѯ�����뵽��ѭ����
*	��    ��: ��
*	�� �� ֵ: ��
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
*							�Ի����ʼ��ѡ��		        
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
*	�� �� ��: PaintDialogFile
*	����˵��: �ļ����������ػ�  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void PaintDialogNet(WM_MESSAGE * pMsg)
{
//    WM_HWIN hWin = pMsg->hWin;

}

/*
*********************************************************************************************************
*	�� �� ��: InitDialogFile
*	����˵��: ��ʼ���Ի���  
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
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
*	�� �� ��: _cbCallbackSYS
*	����˵��: SYS�Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��		        
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
			/* ������ˮƽ��ɫ�ݶ����ľ��� */
			GUI_DrawGradientV(0,			   /* ���Ͻ�X λ�� */
							  0,			   /* ���Ͻ�Y λ�� */
							  xSize - 1,	   /* ���½�X λ�� */
							  ySize - 1,  /* ���½�Y λ�� */
							  GUI_WHITE,	   /* ���������Ҫ���Ƶ���ɫ */
							  GUI_LIGHTBLUE);  /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  60,			   /* ���Ͻ�Y λ�� */
							  400,	           /* ���½�X λ�� */
							  61,              /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

			GUI_DrawGradientH(0,			   /* ���Ͻ�X λ�� */
							  180,			   /* ���Ͻ�Y λ�� */
							  400,	           /* ���½�X λ�� */
							  181,              /* ���½�Y λ�� */
							  GUI_RED,	       /* ���������Ҫ���Ƶ���ɫ */
							  GUI_YELLOW);     /* �������Ҳ�Ҫ���Ƶ���ɫ */

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
*	�� �� ��: OnICON11Clicked
*	����˵��: ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

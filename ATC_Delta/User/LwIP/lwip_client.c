/*
*********************************************************************************************************
*
*	模块名称 : lwip client
*	文件名称 : lwip_client.c
*	版    本 : V1.0
*	说    明 : LwIP 客户端
*	修改记录 :
*		版本号    日期          作者         说明
*		v1.0    2017-08-30    Tupelo Shen    首发
*	
*	Copyright (C), 2017-2027, 迭迩塔智控 www.delta.com
*
*********************************************************************************************************
*/
/********************************************************************************************************
 *                                          INCLUDES
 ********************************************************************************************************/
#include "string.h"
#include "stm32f4x7_eth_bsp.h"
#include "netconf.h"
#include "lwip/tcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "main_lwip.h"
#include "lwip_client.h"
/********************************************************************************************************
 *                                          FUNC DEFINITION
 ********************************************************************************************************/
struct netconn  *tcp_clientconn;				//TCP CLIENT网络连接结构体
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 *tcp_client_sendbuf = "Explorer STM32F407 NETCONN TCP Client send data\r\n";	
                                                //TCP客户端发送数据缓冲区
u8 tcp_client_flag;		                        //TCP客户端数据发送标志位

//LWIP回调函数使用的结构体
struct tcp_client_struct
{
	u8 state;               //当前连接状
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct pbuf *p;         //指向接收/或传输的pbuf
};


typedef struct
{
	u32	header;
	u8	mac[MAC_SIZE];
	u32	ip[IP_SIZE];
	u8	ack;
	u16	cmd_type;
	u32	status;
	u32 data_len;
	u8	data[DATA_RCV_LEN_DEFAULT];
	u32 tail;
	u32	crc;	
}st_EthRcvData;

typedef struct
{
	u32	header;
	u8	mac[MAC_SIZE];
	u32	ip[IP_SIZE];
	u8	ack;
	u16	cmd_type;
	u32	status;	
	u32 data_len;
	u8	data[DATA_SEND_LEN_DEFAULT];	
	u32 tail;
	u32	crc;
}st_EthSendData;

/*
 * TCP Client receive buf
 */
typedef struct {
	u32 rcv_len;
	u8 	data[DATA_HEADER_LEN + DATA_RCV_LEN_DEFAULT + CRC_SIZE + TAIL_SIZE];
}st_TcpRcvBuf;
st_TcpRcvBuf tcp_rcv_buf;
/*
 * TCP client send data buf
 */
typedef struct {
	u32 send_len;
	u8 	data[DATA_HEADER_LEN + DATA_SEND_LEN_DEFAULT + CRC_SIZE + TAIL_SIZE];
}st_TcpSendBuf;
st_TcpSendBuf tcp_send_buf;

struct tcp_pcb *tcppcb;  	                    //定义一个TCP服务器控制块
struct ip_addr rmtipaddr;  	                    //远端ip地址
/*
 * TCP Client 	测试全局状态标记变量
 * bit7:0,		没有数据要发送;1,有数据要发送
 * bit6:0,		没有收到数据;1,收到数据了.
 * bit5:0,		没有连接上服务器;1,连接上服务器了.
 * bit4~0:		保留
 */
u8 tcp_client_flag = 0;


// 客户端连接相关函数
static err_t    tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t    tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t    tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t    tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void     tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct *es);
static void     tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
static void     tcp_client_error(void *arg, err_t err);

// 数据处理相关函数
static void     tcpClientRcvdataProc(void);
static void     procDataFromPkt(void);
static void     setIdleData(void);
static void     setLoginData(void);
static void     setRequestOrderData(void);
static u32      setTcpSendData(st_EthSendData* data_ptr);
static void     setTcpClientSendFlag(void);

/*********************************************************************************************************
*                                     setTcpClientSendFlag  
*
* @Description : setTcpClientSendFlag
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/
static void setTcpClientSendFlag(void)
{
	__disable_irq();
	tcp_client_flag |= 1<<7;
	__enable_irq();
}
/*
*********************************************************************************************************
*	函 数 名: LwipClientConnect
*	功能说明: tcp客户端任务函数 
*	形    参：void
*	返 回 值: void
*********************************************************************************************************
*/
err_t LwipClientConnect(void)
{
    err_t err = ERR_OK;
    
    tcppcb = tcp_new();	//创建一个新的pcb
    if(tcppcb)
    {
        IP4_ADDR(&rmtipaddr, 192, 168, 1, 5);
        err = tcp_bind(tcppcb, IP_ADDR_ANY, 8087);
        if(err == ERR_OK)
		{
			err = tcp_connect(tcppcb,&rmtipaddr,8087,tcp_client_connected);  
		}
    }
    
    return err;
}
/*
*********************************************************************************************************
*	函 数 名: LwipClientReconnect
*	功能说明: tcp客户端重新连接服务器 
*	形    参：void
*	返 回 值: void
*********************************************************************************************************
*/
err_t LwipClientReconnect(void)
{
    err_t err = ERR_OK;
    
    // 关闭上一次连接
    tcp_client_connection_close(tcppcb, 0);
    
    tcppcb = tcp_new();	//创建一个新的pcb
    if(tcppcb)
    {
        IP4_ADDR(&rmtipaddr, 192, 168, 1, 5);
        err = tcp_bind(tcppcb, IP_ADDR_ANY, 8087);
        if(err == ERR_OK)
		{
			err = tcp_connect(tcppcb,&rmtipaddr,8087,tcp_client_connected);  
		}
    }
    
    return err;
}
/*********************************************************************************************************
*                                     tcpClientRcvdataProc  
*
* @Description : 客户端接收数据处理
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/
static void tcpClientRcvdataProc(void)
{	
	if(tcp_client_flag & 1<<6)
	{		
		tcp_client_flag &= ~(1<<6);
		procDataFromPkt();
	}
}
/*********************************************************************************************************
*                                     procDataFromPkt  
*
* @Description : 客户端接收数据处理
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/
static void procDataFromPkt(void)
{
#if 0
	st_EthRcvData rcv_data;
	u32 data_ptr = 0;
	u32 index = 0;
	u32 data_len = 0;
	u8 	work_shop_info[17] = {0};
	u32 work_shop_info_size = 0;
	u8 	work_line_info[17] = {0};
	u32 employe_is_exist = 0;
	u32 employe_num_size = 0;
	u8 	order_data[17] = {0};
	u32 order_data_size = 0;

	if(!findDataPktHeader())
	{
		for(index=0;index<4;index++)
		{
			rcv_data.header |= (u32)((0x000000FF & (u32)tcp_rcv_buf.data[data_ptr++]) << ((3-index) * 8));
		}
		for(index=0;index<MAC_SIZE;index++)
		{
			rcv_data.mac[index] = tcp_rcv_buf.data[data_ptr++];
		}
		for(index=0;index<IP_SIZE;index++)
		{
			rcv_data.ip[index] = tcp_rcv_buf.data[data_ptr++];
		}
		rcv_data.ack = tcp_rcv_buf.data[data_ptr++];
		for(index=0;index<2;index++)
		{
			rcv_data.cmd_type |= (u16)((0x00FF & (u16)tcp_rcv_buf.data[data_ptr++]) << ((1-index) * 8));
		}
		for(index=0;index<4;index++)
		{
			rcv_data.status |= (u32)((0x000000FF & (u32)tcp_rcv_buf.data[data_ptr++]) << ((3-index) * 8));
		}
		data_len = findDataPktTailer(&tcp_rcv_buf.data[data_ptr], (tcp_rcv_buf.rcv_len-DATA_HEADER_LEN));
		if(data_len>4)
		{
			switch(rcv_data.cmd_type)
			{
				case 0x0001:				// 空闲数据,请求流水线
					work_shop_info_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len-4);
				
					if(work_shop_info_size > 4)
					{
						for(index=0;(index<work_shop_info_size-4) && index <16;index++)
						{
							work_shop_info[index] = tcp_rcv_buf.data[data_ptr++];
						}
						work_shop_info[index] = '\0';
						change_text(&gWorkShop_Inform[0], (char *)work_shop_info);
						data_ptr += 4;
						for(index=0;(index<data_len-work_shop_info_size-4) && index <16;index++)
						{
							work_line_info[index] = tcp_rcv_buf.data[data_ptr++];
						}
						work_line_info[index] = '\0';
						change_text(&gWorkShop_Inform[1], (char *)work_line_info);
					}
				break;
				case 0x0002:			// 发送login数据
					employe_num_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len-4);
					if(employe_num_size > 4)
					{
						for(index=0;(index<employe_num_size-4) && index <4;index++)
						{
							employe_is_exist |= (((0x000000FF) & ((u32)tcp_rcv_buf.data[data_ptr++])) << ((3-index) * 8));
						}
						if(employe_is_exist == 0x00000002)
						{
							SetMainWorkState(STATE_MAIN_MASK&STATE_MAIN_LOGIN);
							change_text(&gStaff_Inform[1], "12345678");	// 更新员工编号
						}else if(employe_is_exist == 0x00000001)
						{
							SetMainWorkState(STATE_MAIN_MASK&STATE_MAIN_IDLE);
							change_text(&gStaff_Inform[1], "********");	// 显示星号
						}
					}
				break;
				case 0x0005:			// 订单数据
					//生产单、订单、规格型号、计划数、完成数、补充数
					order_data_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len-4);
					if(order_data_size > 4)
					{
						// 显示"生产单id"
						for(index=0;index < order_data_size-4;index++)
						{
							order_data[index] = tcp_rcv_buf.data[data_ptr++];
						}
						order_data[index] = '\0';
						data_ptr += 4;
						change_text(&gOrder_Inform[0], (char *)order_data);	
					}
					else
					{	
						change_text(&gOrder_Inform[0], (char *)cu8_StatusDataNone);	// 显示"无"
					}
					// 显示"订单id"
					data_len = data_len-order_data_size-4;
					order_data_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len);
					if(order_data_size > 4)
					{
						for(index=0;index < order_data_size-4;index++)
						{
							order_data[index] = tcp_rcv_buf.data[data_ptr++];
						}
						order_data[index] = '\0';
						data_ptr += 4;
						change_text(&gOrder_Inform[1], (char *)order_data);
					}
					else
					{
						change_text(&gOrder_Inform[1], (char *)cu8_StatusDataNone);	// 显示"无"
					}
					// 显示"规格型号"
					data_len = data_len-order_data_size;
					order_data_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len);
					if(order_data_size > 4)
					{
						for(index=0;index < order_data_size-4;index++)
						{
							order_data[index] = tcp_rcv_buf.data[data_ptr++];
						}
						order_data[index] = '\0';
						data_ptr += 4;
						change_text(&gOrder_Inform[2], (char *)order_data);
					}
					else
					{
						change_text(&gOrder_Inform[2], (char *)cu8_StatusDataNone);	// 显示"无"
					}
					// 显示"计划数"
					data_len = data_len-order_data_size;
					order_data_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len);
					if(order_data_size > 4)
					{
						for(index=0;index < order_data_size-4;index++)
						{
							order_data[index] = tcp_rcv_buf.data[data_ptr++];
						}
						order_data[index] = '\0';
						data_ptr += 4;
						change_text(&gProduce_Inf[0], (char *)order_data);
					}
					else
					{
						change_text(&gProduce_Inf[0], (char *)cu8_StatusDataNone);	// 显示"无"
					}
					// 显示"完成数"
					data_len = data_len-order_data_size;
					order_data_size = findDataPktDelimiter(&tcp_rcv_buf.data[data_ptr], data_len);
					if(order_data_size > 4)
					{
						for(index=0;index < order_data_size-4;index++)
						{
							order_data[index] = tcp_rcv_buf.data[data_ptr++];
						}
						order_data[index] = '\0';
						data_ptr += 4;
						change_text(&gProduce_Inf[1], (char *)order_data);
					}
					else
					{
						change_text(&gProduce_Inf[1], (char *)cu8_StatusDataNone);	// 显示"无"
					}
					// 显示"补充数"
					data_len = data_len-order_data_size;
					if(data_len > 0)
					{
						for(index=0;index < data_len;index++)
						{
							order_data[index] = tcp_rcv_buf.data[data_ptr++];
						}
						order_data[index] = '\0';
						change_text(&gProduce_Inf[2], (char *)order_data);
					}
					else
					{
						change_text(&gProduce_Inf[2], (char *)cu8_StatusDataNone);	// 显示"无"
					}
				break;
				default:
					// don't process
				break;
			}
		}
		else/* no data */
		{
			
		}
	}
#endif
}
/*********************************************************************************************************
*                                     TcpClientProc  
*
* @Description : 处理客户端数据
* @Arguments   : 
* @Returns     : none.
**********************************************************************************************************/
void TcpClientProc(void)
{
    // 处理接收数据
    tcpClientRcvdataProc();
    
    switch(CurWorkState & STATE_MAIN_MASK)
    {
        case STATE_MAIN_IDLE:				
            setIdleData();
            setTcpClientSendFlag();
        break;
        
        case STATE_MAIN_LOGINNING:
            setLoginData();
            setTcpClientSendFlag();
        break;
        
        case STATE_MAIN_LOGIN:
            setRequestOrderData();
            setTcpClientSendFlag();
        break;
        
        case STATE_MAIN_WORK:
            
        break;
        
        case STATE_MAIN_ERROR:
            
        break;
        
        default:
            // do nothing
        break;
    }
}
/*********************************************************************************************************
*                                        setIdleData
*
* @Description : 填充空闲数据包
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/

static void setIdleData(void)
{	
	send_data.header = 0xA5A5A5A5;
	send_data.mac[0] = MAC_ADDR0;
	send_data.mac[1] = MAC_ADDR1;
	send_data.mac[2] = MAC_ADDR2;
	send_data.mac[3] = MAC_ADDR3;
	send_data.mac[4] = MAC_ADDR4;
	send_data.mac[5] = MAC_ADDR5;

	send_data.ip[0] = 0x00;
	send_data.ip[1] = 0x00;
	send_data.ip[2] = 0x00;
	send_data.ip[3] = 0x00;
	
	send_data.ack = 0x01;
	send_data.cmd_type = 0x0001;	// 空闲状态
	send_data.status = 0x00000000;
	send_data.data_len = 0x0;
	send_data.tail = 0x5A5A5A5A;	
	send_data.crc = 0x00000000;
	setTcpSendData(&send_data);	
}
/*********************************************************************************************************
*                                        setLoginData
*
* @Description : 填充用户打卡成功数据包
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/

static void setLoginData(void)
{
	u8* ptrICInfo = NULL;
	
	//ptrICInfo = GetICCardInfo();
	
	send_data.header = 0xA5A5A5A5;
	send_data.mac[0] = MAC_ADDR0;
	send_data.mac[1] = MAC_ADDR1;
	send_data.mac[2] = MAC_ADDR2;
	send_data.mac[3] = MAC_ADDR3;
	send_data.mac[4] = MAC_ADDR4;
	send_data.mac[5] = MAC_ADDR5;
	if(ptrICInfo!=NULL)
	{
		send_data.ip[0] = *(ptrICInfo+2);
		send_data.ip[1] = *(ptrICInfo+3);
		send_data.ip[2] = *(ptrICInfo+4);
		send_data.ip[3] = *(ptrICInfo+5);
	}
	
	send_data.ack = 0x01;
	send_data.cmd_type = 0x0002;	// 登陆中
	send_data.status = 0x00000000;
	send_data.data_len = 0x0;
	send_data.tail = 0x5A5A5A5A;	
	send_data.crc = 0x00000000;
	setTcpSendData(&send_data);	
}
/*********************************************************************************************************
*                                        setRequestOrderData
*
* @Description : 填充请求订单信息数据包
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/

static void setRequestOrderData(void)
{
	u8* ptrICInfo = NULL;
	
	//ptrICInfo = GetICCardInfo();
	
	send_data.header = 0xA5A5A5A5;
	send_data.mac[0] = MAC_ADDR0;
	send_data.mac[1] = MAC_ADDR1;
	send_data.mac[2] = MAC_ADDR2;
	send_data.mac[3] = MAC_ADDR3;
	send_data.mac[4] = MAC_ADDR4;
	send_data.mac[5] = MAC_ADDR5;
	if(ptrICInfo!=NULL)
	{
		send_data.ip[0] = *(ptrICInfo+2);
		send_data.ip[1] = *(ptrICInfo+3);
		send_data.ip[2] = *(ptrICInfo+4);
		send_data.ip[3] = *(ptrICInfo+5);
	}
	
	send_data.ack = 0x01;
	send_data.cmd_type = 0x0005;	// 请求订单
	send_data.status = 0x00000000;
	send_data.data_len = 0x0;
	send_data.tail = 0x5A5A5A5A;	
	send_data.crc = 0x00000000;
	setTcpSendData(&send_data);	
}
/*********************************************************************************************************
*                                     setTcpSendData  
*
* @Description : fill the data buf for transmittion.
* @Arguments   : 
* @Returns     : none.
**********************************************************************************************************/ 
static u32 setTcpSendData(st_EthSendData* data_ptr)
{
	u32 index;
	
	// fill header
	tcp_send_buf.data[0] = (u8)(((data_ptr->header) & 0xFF000000) >> 24);
	tcp_send_buf.data[1] = (u8)(((data_ptr->header) & 0x00FF0000) >> 16);
	tcp_send_buf.data[2] = (u8)(((data_ptr->header) & 0x0000FF00) >> 8);
	tcp_send_buf.data[3] = (u8)(((data_ptr->header) & 0x000000FF) >> 0);
	// fill MAC adress
	for(index = 0; index < MAC_SIZE; index++)
	{
		tcp_send_buf.data[index+4] = data_ptr->mac[index];
	}
	// fill IP adress
	for(index = 0; index < IP_SIZE; index++)
	{
		tcp_send_buf.data[index+4+MAC_SIZE] = data_ptr->ip[index];
	}

	index = 4 + MAC_SIZE + IP_SIZE;
	tcp_send_buf.data[index++] = 0x01;
	tcp_send_buf.data[index++] = (u8)((data_ptr->cmd_type & 0xFF00) >> 8);
	tcp_send_buf.data[index++] = (u8)((data_ptr->cmd_type & 0x00FF) >> 0);
	// status
	tcp_send_buf.data[index++] = (u8)((data_ptr->status & 0xFF000000) >> 24);
	tcp_send_buf.data[index++] = (u8)((data_ptr->status & 0x00FF0000) >> 16);
	tcp_send_buf.data[index++] = (u8)((data_ptr->status & 0x0000FF00) >> 8);
	tcp_send_buf.data[index++] = (u8)((data_ptr->status & 0x000000FF) >> 0);
	// data
	for(;index < (DATA_HEADER_LEN + data_ptr->data_len); index++)
	{
		tcp_send_buf.data[index] = data_ptr->data[index-DATA_HEADER_LEN];
	}
	// tail
	tcp_send_buf.data[index++] = (u8)((data_ptr->tail & 0xFF000000) >> 24);
	tcp_send_buf.data[index++] = (u8)((data_ptr->tail & 0x00FF0000) >> 16);
	tcp_send_buf.data[index++] = (u8)((data_ptr->tail & 0x0000FF00) >> 8);
	tcp_send_buf.data[index++] = (u8)((data_ptr->tail & 0x000000FF) >> 0);
	// CRC
	tcp_send_buf.data[index++] = (u8)((data_ptr->crc & 0xFF000000) >> 24);
	tcp_send_buf.data[index++] = (u8)((data_ptr->crc & 0x00FF0000) >> 16);
	tcp_send_buf.data[index++] = (u8)((data_ptr->crc & 0x0000FF00) >> 8);
	tcp_send_buf.data[index++] = (u8)((data_ptr->crc & 0x000000FF) >> 0);
	
	tcp_send_buf.send_len = data_ptr->data_len + DATA_HEADER_LEN + CRC_SIZE + TAIL_SIZE;
	
	return 0;
}
/*********************************************************************************************************
*                                     tcp_client_connected  
*
* @Description : Call this function after a TCP connection setupped.
* @Arguments   : none
* @Returns     : none
**********************************************************************************************************/
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)  
{       
	struct tcp_client_struct *es = NULL;

	if(err == ERR_OK)
	{
		es = (struct tcp_client_struct *)mem_malloc(sizeof(struct tcp_client_struct));
		if(es)	/* malloc memory successfully! */
		{
			es->state = ES_TCPCLIENT_CONNECTED;	/* connected successfully */
			es->pcb	= tpcb;
			es->p = NULL;
			tcp_arg(tpcb,es);					/* update args of all callback functions */
			tcp_recv(tpcb,tcp_client_recv);		
			tcp_err(tpcb,tcp_client_error);
			tcp_sent(tpcb,tcp_client_sent);
			tcp_poll(tpcb,tcp_client_poll,1);
			tcp_client_flag |= 1<<5;			/* mark that connected remote tcp server */
			err = ERR_OK;
		}
		else
		{
			tcp_client_connection_close(tpcb, es);
			err = ERR_MEM;	/* malloc memory failed */
		}
	}
	else
	{
		tcp_client_connection_close(tpcb, es);
	}
	
	return err;
}
/*********************************************************************************************************
*                                     tcp_client_recv  
*
* @Description : tcp_receiv callback
* @Arguments   : 
*				arg:	argument to be passed to receive callback
*				tpcb: 	tcp connection control block
*				err: 	receive error code
* @Returns     : 
*				err_t: 	retuned error
**********************************************************************************************************/
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
	u32 	data_len = 0;
	struct 	pbuf *q;
	struct 	tcp_client_struct *es;
	err_t 	ret_err;
	
	LWIP_ASSERT("arg != NULL",arg != NULL);
	
	es = (struct tcp_client_struct *)arg; 
	if(p == NULL)//如果从服务器接收到空的数据帧就关闭连接
	{
		es->state 	= ES_TCPCLIENT_CLOSING;//需要关闭TCP 连接了 
 		es->p 		= p; 
		ret_err		= ERR_OK;
	}
	else if(err != ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
	{ 
		if(p) pbuf_free(p);//释放接收pbuf
		ret_err = err;
	}
	else if(es->state == ES_TCPCLIENT_CONNECTED)	//当处于连接状态时
	{
		if(p != NULL)//当处于连接状态并且接收到的数据不为空时
		{
			memset(&tcp_rcv_buf.data[0], 0, DATA_RCV_LEN_DEFAULT);  //数据接收缓冲区清零
			
			/* 遍历完整个pbuf链表 */
			for(q=p;q!=NULL;q=q->next)  
			{
				//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (DATA_RCV_LEN_DEFAULT - data_len)) 
					memcpy(&tcp_rcv_buf.data[0] + data_len, q->payload, (DATA_RCV_LEN_DEFAULT-data_len));//拷贝数据
				else 
					memcpy(&tcp_rcv_buf.data[0] + data_len, q->payload, q->len);
				data_len += q->len;
				tcp_rcv_buf.rcv_len = data_len;
				
				if(data_len > DATA_RCV_LEN_DEFAULT) break; //超出TCP客户端接收数组,跳出	
			}
			//printf("Receive Data: = %x;Len = %d\r\n",tcp_rcv_buf.data[0], tcp_rcv_buf.rcv_len);
			tcp_client_flag |= 1<<6;		//标记接收到数据了
 			tcp_recved(tpcb,p->tot_len);	//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  					//释放内存
			ret_err = ERR_OK;
		}
	}
	else  //接收到数据但是连接已经关闭,
	{ 
		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
		es->p = NULL;
		pbuf_free(p); //释放内存
		ret_err = ERR_OK;
	}
	
	return ret_err;
}
/*********************************************************************************************************
*                                     tcp_client_error  
*
* @Description : tcp_err callback
* @Arguments   : 
*				arg:	argument to be passed to receive callback
*				err: 	receive error code
* @Returns     : none.
**********************************************************************************************************/
static void tcp_client_error(void *arg, err_t err)
{  
	struct tcp_client_struct *es = (struct tcp_client_struct*)arg;  
  
    if(err == ERR_OK ){  
          /* No error, everything OK. */  
          return ;  
     }     
    switch(err)  
    {   
        case ERR_MEM:                                            /* Out of memory error.     */  
            //printf("\r\n ERR_MEM   \r\n");  
            break;    
        case ERR_BUF:                                            /* Buffer error.            */  
            //printf("\r\n ERR_BUF   \r\n");  
            break;  
        case ERR_TIMEOUT:                                       /* Timeout.                 */  
            //printf("\r\n ERR_TIMEOUT   \r\n");  
            break;  
        case ERR_RTE:                                            /* Routing problem.         */        
            //printf("\r\n ERR_RTE   \r\n");  
            break;  
       case ERR_ISCONN:                                          /* Already connected.       */  
            //printf("\r\n ERR_ISCONN   \r\n");  
            break;  
        case ERR_ABRT:                                           /* Connection aborted.      */  
            //printf("\r\n ERR_ABRT   \r\n");  
            break;  
        case ERR_RST:                                            /* Connection reset.        */       
            //printf("\r\n ERR_RST   \r\n");  
            break;  
        case ERR_CONN:                                           /* Not connected.           */  
			//printf("\r\n ERR_CONN   \r\n");  
            break;  
        case ERR_CLSD:                                           /* Connection closed.       */  
            //printf("\r\n ERR_CLSD   \r\n");  
            break;  
        case ERR_VAL:                                            /* Illegal value.           */  
			//printf("\r\n ERR_VAL   \r\n");  
			return;  
        case ERR_ARG:                                            /* Illegal argument.        */  
			//printf("\r\n ERR_ARG   \r\n");  
            return;  
        case ERR_USE:                                            /* Address in use.          */  
			//printf("\r\n ERR_USE   \r\n");  
           return;   
        case ERR_IF:                                             /* Low-level netif error    */  
            //printf("\r\n ERR_IF   \r\n");  
            break;  
        case ERR_INPROGRESS:                                     /* Operation in progress    */  
            //printf("\r\n ERR_INPROGRESS   \r\n");  
            break;  
  
    }  
	es->state  = ES_TCPCLIENT_CLOSING;
	tcp_client_flag &= ~(1<<5);
}
/*********************************************************************************************************
*                                     tcp_client_poll  
*
* @Description : tcp_poll callback
* @Arguments   : 
*				arg:	argument to be passed to receive callback
*				tpcb: 	tcp connection control block
* @Returns     : 
*				err_t: 	retuned error
**********************************************************************************************************/
#define TEST_SIZE           10
char test_array[TEST_SIZE+1] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0};
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es = (struct tcp_client_struct*)arg; 
	
	/* 连接处于空闲可以发送数据 */ 
	if(es != NULL)  
	{
		/* 判断是否有数据要发送 */
		if(tcp_client_flag & ( 1 << 7))	 
		{
			//__disable_irq();
			//es->p = pbuf_alloc(PBUF_TRANSPORT, tcp_send_buf.send_len, PBUF_POOL);	//申请内存
            es->p = pbuf_alloc(PBUF_TRANSPORT, TEST_SIZE, PBUF_RAM);	//申请内存
			//pbuf_take(es->p, &tcp_send_buf.data[0], tcp_send_buf.send_len);	//将tcp_send_buf[中的数据拷贝到es->p中
            pbuf_take(es->p, &test_array, TEST_SIZE);	//将tcp_send_buf[中的数据拷贝到es->p中
			tcp_client_senddata(tpcb,es);//将tcp_send_buf[]里面复制给pbuf的数据发送出去
			tcp_client_flag &= ~(1<<7);	//清除数据发送标志
			if(es->p) pbuf_free(es->p);	//释放内存
			//__disable_irq();
		}
		else if(es->state == ES_TCPCLIENT_CLOSING)
		{ 
 			tcp_client_connection_close(tpcb, es);//关闭TCP连接
		} 
		
		ret_err=ERR_OK;
	}
	else
	{ 
		tcp_abort(tpcb);//终止连接,删除pcb控制块
		ret_err = ERR_ABRT;
	}
	
	return ret_err;
} 
/*********************************************************************************************************
*                                     tcp_client_sent  
*
* @Description : tcp_client_sent
* @Arguments   : 
*				es:	
*				tpcb: 	tcp connection control block
* @Returns     : none.
**********************************************************************************************************/
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_client_struct *es;
	LWIP_UNUSED_ARG(len);
	es=(struct tcp_client_struct*)arg;
	if(es->p)tcp_client_senddata(tpcb,es);//发送数据
	return ERR_OK;
}
/*********************************************************************************************************
*                                     tcp_client_senddata  
*
* @Description : send data
* @Arguments   : 
*				es:	
*				tpcb: 	tcp connection control block
* @Returns     : none.
**********************************************************************************************************/
static void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	struct pbuf *ptr; 
 	err_t wr_err = ERR_OK;
	
	while((wr_err == ERR_OK) && es->p && (es->p->len <= tcp_sndbuf(tpcb)))
	{
		ptr = es->p;
		wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1); //将要发送的数据加入到发送缓冲队列中
		if(wr_err == ERR_OK)
		{  
			es->p=ptr->next;			//指向下一个pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf的ref加一
			pbuf_free(ptr);				//释放ptr 
		}
		else if(wr_err == ERR_MEM)
			es->p = ptr;
		
		tcp_output(tpcb);		//将发送缓冲队列中的数据立即发送出去
	} 	
} 
/*********************************************************************************************************
*                                     tcp_client_connection_close  
*
* @Description : Closed connection.
* @Arguments   : 
*				 es:	
*				 tpcb: 	tcp connection control block
* @Returns     : none.
**********************************************************************************************************/
static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct *es)
{
	//移除回调
	tcp_abort(tpcb);//终止连接,删除pcb控制块
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  
	if(es) mem_free(es); 
	tcp_client_flag &= ~(1<<5);//标记连接断开了
}
/***************************** 迭迩塔智控 www.delta.com (END OF FILE) *********************************/

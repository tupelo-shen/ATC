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
// #include "lwip/opt.h"
// #include <lwip/api.h>
// #include "lwip/ip_addr.h"
// #include "lwip/pbuf.h"
// #include "lwip/err.h"
// #include "lwip/netbuf.h"
#include "lwip_client.h"
#include "stm32f4x7_eth_bsp.h"
#include "netconf.h"
//#include "netmain.h"
#include "lwip/tcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
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

static err_t    tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t    tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t    tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t    tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void     tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct *es);
static void     tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
static void     tcp_client_error(void *arg, err_t err);
/*
*********************************************************************************************************
*	函 数 名: LwipClientPro
*	功能说明: tcp客户端任务函数 
*	形    参：void
*	返 回 值: void
*********************************************************************************************************
*/
void LwipClientPro(void)
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
			if(err != ERR_OK)
			{
				// error process;
			}
		}
    }
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
            printf("\r\n ERR_INPROGRESS   \r\n");  
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
#if 0
/*
*********************************************************************************************************
*	函 数 名: LwipClientPro
*	功能说明: tcp客户端任务函数 
*	形    参：void
*	返 回 值: void
*********************************************************************************************************
*/
void LwipClientPro(void)
{
	u32                 data_len = 0;
	struct pbuf         *q;
	err_t               err, recv_err;
	static ip_addr_t    server_ipaddr, loca_ipaddr;
	static u16 		    server_port, loca_port;

	server_port = REMOTE_PORT;
	//IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	IP4_ADDR(&server_ipaddr, 192, 168, 1, 5);
    
	while (1) 
	{
        // 创建一个TCP链接
		tcp_clientconn = netconn_new(NETCONN_TCP);
        
        // 连接服务器, 返回值不等于ERR_OK,删除tcp_clientconn连接
		err = netconn_connect(tcp_clientconn, &server_ipaddr, server_port);
        if(err != ERR_OK)  
        {
            netconn_delete(tcp_clientconn);
        }
		else if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;
			netconn_getaddr(tcp_clientconn, &loca_ipaddr, &loca_port, 1); //获取本地IP主机IP地址和端口号
			while(1)
			{
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				{
					err = netconn_write(tcp_clientconn ,tcp_client_sendbuf, strlen((char*)tcp_client_sendbuf), NETCONN_COPY); //发送tcp_server_sentbuf中的数据
					if(err != ERR_OK)
					{
						// 发送失败的情况
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
                recvbuf = netconn_recv(tcp_clientconn);
				if(recvbuf)  //接收到数据
				{	
					// OS_ENTER_CRITICAL(); //关中断
					memset(tcp_client_recvbuf, 0, TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					// OS_EXIT_CRITICAL();  //开中断
					data_len=0;  //复制完成后data_len要清零。					
					printf("%s\r\n",tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(recvbuf == NULL)  //关闭连接
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("服务器%d.%d.%d.%d断开连接\r\n",192, 168, 1,5);
					break;
				}
			}
		}
	}
}
#endif
/***************************** 迭迩塔智控 www.delta.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : lwip client
*	�ļ����� : lwip_client.c
*	��    �� : V1.0
*	˵    �� : LwIP �ͻ���
*	�޸ļ�¼ :
*		�汾��    ����          ����         ˵��
*		v1.0    2017-08-30    Tupelo Shen    �׷�
*	
*	Copyright (C), 2017-2027, �������ǿ� www.delta.com
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
struct netconn  *tcp_clientconn;				//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 *tcp_client_sendbuf = "Explorer STM32F407 NETCONN TCP Client send data\r\n";	
                                                //TCP�ͻ��˷������ݻ�����
u8 tcp_client_flag;		                        //TCP�ͻ������ݷ��ͱ�־λ

//LWIP�ص�����ʹ�õĽṹ��
struct tcp_client_struct
{
	u8 state;               //��ǰ����״
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *p;         //ָ�����/�����pbuf
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

struct tcp_pcb *tcppcb;  	                    //����һ��TCP���������ƿ�
struct ip_addr rmtipaddr;  	                    //Զ��ip��ַ
/*
 * TCP Client 	����ȫ��״̬��Ǳ���
 * bit7:0,		û������Ҫ����;1,������Ҫ����
 * bit6:0,		û���յ�����;1,�յ�������.
 * bit5:0,		û�������Ϸ�����;1,�����Ϸ�������.
 * bit4~0:		����
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
*	�� �� ��: LwipClientPro
*	����˵��: tcp�ͻ��������� 
*	��    �Σ�void
*	�� �� ֵ: void
*********************************************************************************************************
*/
void LwipClientPro(void)
{
    err_t err = ERR_OK;
    
    tcppcb = tcp_new();	//����һ���µ�pcb
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
	if(p == NULL)//����ӷ��������յ��յ�����֡�͹ر�����
	{
		es->state 	= ES_TCPCLIENT_CLOSING;//��Ҫ�ر�TCP ������ 
 		es->p 		= p; 
		ret_err		= ERR_OK;
	}
	else if(err != ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
	{ 
		if(p) pbuf_free(p);//�ͷŽ���pbuf
		ret_err = err;
	}
	else if(es->state == ES_TCPCLIENT_CONNECTED)	//����������״̬ʱ
	{
		if(p != NULL)//����������״̬���ҽ��յ������ݲ�Ϊ��ʱ
		{
			memset(&tcp_rcv_buf.data[0], 0, DATA_RCV_LEN_DEFAULT);  //���ݽ��ջ���������
			
			/* ����������pbuf���� */
			for(q=p;q!=NULL;q=q->next)  
			{
				//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
				//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
				if(q->len > (DATA_RCV_LEN_DEFAULT - data_len)) 
					memcpy(&tcp_rcv_buf.data[0] + data_len, q->payload, (DATA_RCV_LEN_DEFAULT-data_len));//��������
				else 
					memcpy(&tcp_rcv_buf.data[0] + data_len, q->payload, q->len);
				data_len += q->len;
				tcp_rcv_buf.rcv_len = data_len;
				
				if(data_len > DATA_RCV_LEN_DEFAULT) break; //����TCP�ͻ��˽�������,����	
			}
			//printf("Receive Data: = %x;Len = %d\r\n",tcp_rcv_buf.data[0], tcp_rcv_buf.rcv_len);
			tcp_client_flag |= 1<<6;		//��ǽ��յ�������
 			tcp_recved(tpcb,p->tot_len);	//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
			pbuf_free(p);  					//�ͷ��ڴ�
			ret_err = ERR_OK;
		}
	}
	else  //���յ����ݵ��������Ѿ��ر�,
	{ 
		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
		es->p = NULL;
		pbuf_free(p); //�ͷ��ڴ�
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
	
	/* ���Ӵ��ڿ��п��Է������� */ 
	if(es != NULL)  
	{
		/* �ж��Ƿ�������Ҫ���� */
		if(tcp_client_flag & ( 1 << 7))	 
		{
			//__disable_irq();
			//es->p = pbuf_alloc(PBUF_TRANSPORT, tcp_send_buf.send_len, PBUF_POOL);	//�����ڴ�
            es->p = pbuf_alloc(PBUF_TRANSPORT, TEST_SIZE, PBUF_RAM);	//�����ڴ�
			//pbuf_take(es->p, &tcp_send_buf.data[0], tcp_send_buf.send_len);	//��tcp_send_buf[�е����ݿ�����es->p��
            pbuf_take(es->p, &test_array, TEST_SIZE);	//��tcp_send_buf[�е����ݿ�����es->p��
			tcp_client_senddata(tpcb,es);//��tcp_send_buf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
			tcp_client_flag &= ~(1<<7);	//������ݷ��ͱ�־
			if(es->p) pbuf_free(es->p);	//�ͷ��ڴ�
			//__disable_irq();
		}
		else if(es->state == ES_TCPCLIENT_CLOSING)
		{ 
 			tcp_client_connection_close(tpcb, es);//�ر�TCP����
		} 
		
		ret_err=ERR_OK;
	}
	else
	{ 
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
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
	if(es->p)tcp_client_senddata(tpcb,es);//��������
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
		wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1); //��Ҫ���͵����ݼ��뵽���ͻ��������
		if(wr_err == ERR_OK)
		{  
			es->p=ptr->next;			//ָ����һ��pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
			pbuf_free(ptr);				//�ͷ�ptr 
		}
		else if(wr_err == ERR_MEM)
			es->p = ptr;
		
		tcp_output(tpcb);		//�����ͻ�������е������������ͳ�ȥ
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
	//�Ƴ��ص�
	tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  
	if(es) mem_free(es); 
	tcp_client_flag &= ~(1<<5);//������ӶϿ���
}
#if 0
/*
*********************************************************************************************************
*	�� �� ��: LwipClientPro
*	����˵��: tcp�ͻ��������� 
*	��    �Σ�void
*	�� �� ֵ: void
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
        // ����һ��TCP����
		tcp_clientconn = netconn_new(NETCONN_TCP);
        
        // ���ӷ�����, ����ֵ������ERR_OK,ɾ��tcp_clientconn����
		err = netconn_connect(tcp_clientconn, &server_ipaddr, server_port);
        if(err != ERR_OK)  
        {
            netconn_delete(tcp_clientconn);
        }
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;
			netconn_getaddr(tcp_clientconn, &loca_ipaddr, &loca_port, 1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			while(1)
			{
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					err = netconn_write(tcp_clientconn ,tcp_client_sendbuf, strlen((char*)tcp_client_sendbuf), NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						// ����ʧ�ܵ����
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
                recvbuf = netconn_recv(tcp_clientconn);
				if(recvbuf)  //���յ�����
				{	
					// OS_ENTER_CRITICAL(); //���ж�
					memset(tcp_client_recvbuf, 0, TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					// OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					printf("%s\r\n",tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(recvbuf == NULL)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",192, 168, 1,5);
					break;
				}
			}
		}
	}
}
#endif
/***************************** �������ǿ� www.delta.com (END OF FILE) *********************************/

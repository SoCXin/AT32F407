/*
*********************************************************************************************************
*
*	模块名称 : TCPnet网络协议栈测试
*	文件名称 : app_tcpnet_lib.c
*	版    本 : V1.0
*	说    明 : 测试的功能说明
*              1. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*              2. 创建了一个TCP Server，而且使能了局域网域名NetBIOS，用户只需在电脑端ping armfly
*                 就可以获得板子的IP地址，端口号1001。
*              3. 用户可以在电脑端用网络调试软件创建TCP Client连接此服务器端。
*              4. 按键K1按下，发送8字节的数据给TCP Client。
*              5. 按键K2按下，发送1024字节的数据给TCP Client。
*              6. 按键K3按下，发送5MB字节的数据给TCP Client。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2017-04-17   Eric2013     首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include <stdint.h>
#include <RTL.h>
#include <stdio.h>
#include <Net_Config.h>




/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#define PORT_NUM       1001    /* TCP服务器监听端口号 */


/*
*********************************************************************************************************
*	                                     变量
*********************************************************************************************************
*/
uint8_t socket_tcp;


/*
*********************************************************************************************************
*	函 数 名: tcp_callback
*	功能说明: TCP Socket的回调函数
*	形    参: soc  TCP Socket类型
*             evt  事件类型
*             ptr  事件类型是TCP_EVT_DATA，ptr指向的缓冲区记录着接收到的TCP数据，其余事件记录IP地址
*             par  事件类型是TCP_EVT_DATA，记录接收到的数据个数，其余事件记录端口号
*	返 回 值: 
*********************************************************************************************************
*/
U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
	char buf[50];
	uint16_t i;
	
	/* 确保是socket_tcp的回调 */
	if (soc != socket_tcp) 
	{
		return (0);
	}

	switch (evt) 
	{
		/*
			远程客户端连接消息
		    1、数组ptr存储远程设备的IP地址，par中存储端口号。
		    2、返回数值1允许连接，返回数值0禁止连接。
		*/
		case TCP_EVT_CONREQ:
			sprintf(buf, "远程客户端请求连接IP: %d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
			printf_debug("IP:%s  port:%d\r\n", buf, par);
			return (1);
		
		/* 连接终止 */
		case TCP_EVT_ABORT:
			break;
		
		/* Socket远程连接已经建立 */
		case TCP_EVT_CONNECT:
			printf_debug("Socket is connected to remote peer\r\n");
			break;
		
		/* 连接断开 */
		case TCP_EVT_CLOSE:
		   	printf_debug("Connection has been closed\r\n");
			break;
		
		/* 发送的数据收到远程设备应答 */
		case TCP_EVT_ACK:
			break;
		
		/* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
		case TCP_EVT_DATA:
			printf_debug("Data length = %d\r\n", par);
			for(i = 0; i < par; i++)
			{
				printf_debug("ptr[%d] = %d\r\n", i, ptr[i]);
			}
			break;
	}
	
	return (0);
}

/*
*********************************************************************************************************
*	函 数 名: TCP_StatusCheck
*	功能说明: 检测TCP的连接状态，主要用于网线插拔的判断
*	形    参: 无
*	返 回 值: __TRUE  连接
*             __FALSE 断开
*********************************************************************************************************
*/
uint8_t TCP_StatusCheck(void) 
{
	uint8_t res;
	
	switch (tcp_get_state(socket_tcp)) 
	{
		case TCP_STATE_FREE:
		case TCP_STATE_CLOSED:
			res = tcp_listen (socket_tcp, PORT_NUM);
			printf_debug("tcp listen res = %d\r\n", res);
			break;
		
		case TCP_STATE_LISTEN:
			break;
		
		case TCP_STATE_CONNECT:
			return (__TRUE);
			
		default:  
			break;
	}
	
	return (__FALSE);
}

/*
*********************************************************************************************************
*	函 数 名: tcpnet_poll
*	功能说明: 使用TCPnet必须要一直调用的函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void tcpnet_poll(void)
{
  #if 0
	if(bsp_CheckTimer(0))
	{
		/* 此函数坚决不可以放在中断里面跑 */
		timer_tick ();
	}
  #endif
	
	main_TcpNet ();
}

/*
*********************************************************************************************************
*	函 数 名: TCPnetTest
*	功能说明: TCPnet应用
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TCPnetTest(void)
{  
	int32_t iCount;
	uint8_t *sendbuf;
	uint8_t tcp_status;
	uint16_t maxlen;
	uint8_t res;
	uint8_t ucKeyCode;
	
	
	/* 初始化网络协议栈 */
	init_TcpNet ();

	/* 
	   创建TCP Socket并创建监听，客户端连接服务器后，10秒内无数据通信将断开连接。
	   但是由于这里使能了TCP_TYPE_KEEP_ALIVE，会一直保持连接，不受10秒的时间限制。
	*/
    socket_tcp = tcp_get_socket (TCP_TYPE_SERVER | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_tcp != 0)
	{
		res = tcp_listen (socket_tcp, PORT_NUM);
		printf_debug("tcp listen res = %d\r\n", res);
	}

	/* 创建一个周期是100ms的软定时器 */
  //bsp_StartAutoTimer(0, 100);
	
	while (1) 
	{
		/* TCP轮询 */
		tcpnet_poll();
		
		/* 用于网线插拔的处理 */
		tcp_status = TCP_StatusCheck();
		#if 0
		/* 按键消息的处理 */
		ucKeyCode = bsp_GetKey();
		if ((ucKeyCode != KEY_NONE)&&(tcp_status == __TRUE))
		{
			switch (ucKeyCode)
			{
				/* K1键按下，给远程TCP客户端发送8字节数据 */
				case KEY_DOWN_K1:			  
					printf_debug("tcp_get_state(socket_tcp) = %d\r\n", tcp_get_state(socket_tcp));
					iCount = 8;
					do
					{
						tcpnet_poll();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* 这么计算没问题的 */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = tcp_get_buf(maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* 测试发现只能使用获取的内存 */
							tcp_send (socket_tcp, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;

				/* K2键按下，给远程TCP客户端发送1024字节的数据 */
				case KEY_DOWN_K2:		
					printf_debug("tcp_get_state(socket_tcp) = %d\r\n", tcp_get_state(socket_tcp));
					iCount = 1024;
					do
					{
						tcpnet_poll();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* 这么计算没问题的 */
								maxlen = iCount + maxlen;
							}
							
							/* 这里仅初始化了每次所发送数据包的前8个字节 */
							sendbuf = tcp_get_buf(maxlen);
							sendbuf[0] = 'a';
							sendbuf[1] = 'b';
							sendbuf[2] = 'c';
							sendbuf[3] = 'd';
							sendbuf[4] = 'e';
							sendbuf[5] = 'f';
							sendbuf[6] = 'g';
							sendbuf[7] = 'h';
							
							/* 测试发现只能使用获取的内存 */
							tcp_send (socket_tcp, sendbuf, maxlen);
						}
						
					}while(iCount > 0);					
					break;
					
				/* K3键按下，给远程TCP客户端发送5MB数据 */
				case KEY_DOWN_K3:			  
					printf_debug("tcp_get_state(socket_tcp) = %d\r\n", tcp_get_state(socket_tcp));
					iCount = 5*1024*1024;
					do
					{
						tcpnet_poll();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* 这么计算没问题的 */
								maxlen = iCount + maxlen;
							}
							
							/* 这里仅初始化了每次所发送数据包的前8个字节 */
							sendbuf = tcp_get_buf(maxlen);
							sendbuf[0] = 'a';
							sendbuf[1] = 'b';
							sendbuf[2] = 'c';
							sendbuf[3] = 'd';
							sendbuf[4] = 'e';
							sendbuf[5] = 'f';
							sendbuf[6] = 'g';
							sendbuf[7] = 'h';
							
							/* 测试发现只能使用获取的内存 */
							tcp_send (socket_tcp, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;
				
				 /* 其他的键值不处理 */
				default:                     
					break;
			}
		}
    #endif
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

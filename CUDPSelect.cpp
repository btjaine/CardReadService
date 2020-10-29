#include "pch.h"
#include "CUDPSelect.h"
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#pragma  warning(disable:4996)
#pragma  warning(disable:4800)
#pragma  warning(disable:4244)


CUDPSelect::CUDPSelect(char * cIP,int nPort)
{
	InitSocket();
	CVR_Server_addrSrv.sin_addr.S_un.S_addr = inet_addr(cIP);
	CVR_Server_addrSrv.sin_family = AF_INET;
	CVR_Server_addrSrv.sin_port = htons(nPort);
}


CUDPSelect::~CUDPSelect()
{
	CloseSocket();
}
void CUDPSelect::CloseSocket()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}


int  CUDPSelect::InitSocket()
{
	WSADATA wsaData;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))  //初始化
	{
		return -1;
	}
	if (2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))
	{
		WSACleanup();
		return -2;
	}
	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_Socket)
	{
		return -3;
	}
	return 0;
}

bool CUDPSelect::RecvMsg(char* recvbuf, int nRecvSize)
{
	/*
	const struct timeval* timeout：设置select的超时时间，
	1）传入NULL值：将设置select为阻塞状态，一直等到监视的文件描述符集合中的某个文件描述符发生变化；
	2）传入0秒0毫秒：select变成一个纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行，文件无变化返回0，有变化返回一个正值；
	3）timeout的值大于0：就是等待的超时时间，即 select在timeout时间内阻塞，超时时间之内有事件到来就返回了，否则在超时后不管怎样一定返回；

	返回值

	< 0：select发生错误。
	> 0：监视的文件描述符发生变化。
	= 0：等待超时，监视的文件描述符没有变化。
	*/
	fd_set fdRead;
	timeval timeout;

	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	while (true)
	{
		FD_ZERO(&fdRead);

		FD_SET(m_Socket, &fdRead);

		int ret = select(0, &fdRead, NULL, NULL, &timeout);
		if (ret == SOCKET_ERROR)
		{
			return false;
		}
		else if (ret == 0)//超时
		{
			return false;
		}
		else if (ret > 0)
		{
			if (FD_ISSET(m_Socket, &fdRead))
			{
				int nLen = sizeof(Recv_addrSrv);
				int nRecvBytes = recvfrom(m_Socket, recvbuf, nRecvSize, 0, (SOCKADDR*)&Recv_addrSrv, &nLen);
				if (nRecvBytes == SOCKET_ERROR || nRecvBytes <= 0)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;
}
int     CUDPSelect::SendMsg(char* sendbuf, int nSendSize)
{
	//发送信息
	int m_iSendLen = sendto(m_Socket, sendbuf, nSendSize, 0,
		(SOCKADDR *)&CVR_Server_addrSrv, sizeof(SOCKADDR));
	if (m_iSendLen != nSendSize)
	{
		return -1;
	}
	return nSendSize;
}
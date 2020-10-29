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

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))  //��ʼ��
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
	const struct timeval* timeout������select�ĳ�ʱʱ�䣬
	1������NULLֵ��������selectΪ����״̬��һֱ�ȵ����ӵ��ļ������������е�ĳ���ļ������������仯��
	2������0��0���룺select���һ������ķ����������������ļ��������Ƿ��б仯�������̷��ؼ���ִ�У��ļ��ޱ仯����0���б仯����һ����ֵ��
	3��timeout��ֵ����0�����ǵȴ��ĳ�ʱʱ�䣬�� select��timeoutʱ������������ʱʱ��֮�����¼������ͷ����ˣ������ڳ�ʱ�󲻹�����һ�����أ�

	����ֵ

	< 0��select��������
	> 0�����ӵ��ļ������������仯��
	= 0���ȴ���ʱ�����ӵ��ļ�������û�б仯��
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
		else if (ret == 0)//��ʱ
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
	//������Ϣ
	int m_iSendLen = sendto(m_Socket, sendbuf, nSendSize, 0,
		(SOCKADDR *)&CVR_Server_addrSrv, sizeof(SOCKADDR));
	if (m_iSendLen != nSendSize)
	{
		return -1;
	}
	return nSendSize;
}
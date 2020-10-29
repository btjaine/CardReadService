#pragma once
class CUDPSelect
{
public:
	CUDPSelect(char * cIP, int nPort);
	~CUDPSelect();

	public:
	bool  RecvMsg(char* recvbuf,int nRecvSize);
	int     InitSocket();
	int     SendMsg(char* sendbuf, int nSendSize);
	void   CloseSocket();
private:
	SOCKADDR_IN   CVR_Server_addrSrv;
	SOCKADDR_IN   Recv_addrSrv;
	SOCKET m_Socket;
};


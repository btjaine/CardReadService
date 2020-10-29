#pragma once
#include "pch.h"
#include "CidCardReader.h"
#include  "CLogHelper.h"
#include  "CUDPSelect.h"

typedef  struct TagCardIdMsg
{
	char  m_cMsgType[8];//消息类型 0:链接设备失败  1：链接设备成功   2：读卡失败    3：读卡成功
	SFZCard m_stuCardInInfo;//读取到的身份证信息
}CARD_ID_MSG,*LPCARD_ID_MSG;//身份证读卡程序和人脸识别程序沟通的消息，此处通过UDP发送


extern  HANDLE  g_hExitEvent;


class CIdCardReaderManager
{
public:
	CIdCardReaderManager()
	{
		//获取程序运行路径2015-12-30
		int    nPos;
		m_strFilePath.Empty();
		GetModuleFileName(NULL, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		m_strFilePath.ReleaseBuffer();
		nPos = m_strFilePath.ReverseFind('\\');
		m_strFilePath = m_strFilePath.Left(nPos + 1);

		//获取相关参数
		//获取TCP/IP的网络信息，端口号
		CString m_strSettringFile =m_strFilePath;
		m_strSettringFile += _T("setting.ini");
		//设置相关参数
		ZeroMemory(m_tcFaceUDPPort, sizeof(m_tcFaceUDPPort));
		GetPrivateProfileString(_T("FaceServer"), _T("Server_Port"), L"7890", m_tcFaceUDPPort, 10, m_strSettringFile);
		
		ZeroMemory(m_tcFaceUDPIP, sizeof(m_tcFaceUDPIP));
		GetPrivateProfileString(_T("FaceServer"), _T("Server_Ip"), L"127.0.0.1", m_tcFaceUDPIP, 32, m_strSettringFile);
		//刷新读卡间隔
		m_nReadCarTimeOut = GetPrivateProfileInt(_T("FaceServer"), _T("ReadCard_TimeOut"), 1, m_strSettringFile);
		//读取身份证设备类型
		m_n_device_type= GetPrivateProfileInt(_T("FaceServer"), _T("DeviceType"), 1, m_strSettringFile);
	};
private:
	CIdCardReader*  m_CardReaderInstance=nullptr;
	HANDLE     m_hThreadReadId;//读取卡的线程，这里每个1秒钟读取一次
	TCHAR       m_tcFaceUDPPort[10];//人脸服务器的UDP接收端口
	TCHAR       m_tcFaceUDPIP[64];//人脸服务器的UDP接收IP
	int               m_nReadCarTimeOut;//读卡间隔
	CUDPSelect* m_pUDPInstance;//UDP通讯
	CARD_ID_MSG    m_stuCardIdMsg;//UDP发送的结构
	int               m_n_device_type = 0;//对身份证设备类型 0：海康读身份证设备  1：华视电子   2：可以往后面加
public:
	CString       m_strFilePath;
public:
	void  Start();
	void  Stop();
	static DWORD __stdcall StartThread(void* pInstance);//业务线程壳子
	DWORD TrueStartThread();//真正的业务线程
	bool tchar2char(TCHAR* buf, char* str);
};


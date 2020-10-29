#include "pch.h"
#include "CIdCardReaderManager.h"
#include  "CLogHelper.h"

void CIdCardReaderManager::Start()
{
	//开启线程，在线程中链接设备，然后不断读取读卡器
	m_hThreadReadId = chBEGINTHREADEX(0, 0, StartThread, this, 0, NULL);
	if (NULL == m_hThreadReadId)
	{
		CString strLogMsg = _T("创建StartThread失败");
		CLogHelper::WriteLog(strLogMsg);
	}
	else
	{
		CString strLogMsg = _T("创建StartThread成功");
		CLogHelper::WriteLog(strLogMsg);
	}
}

void CIdCardReaderManager::Stop()
{
	//停掉线程，关闭读卡器
	SetEvent(g_hExitEvent);
	if (m_hThreadReadId)
	{
		WaitForSingleObject(m_hThreadReadId, INFINITE);
	}
}

DWORD __stdcall CIdCardReaderManager::StartThread(void* pInstance)
{
	CIdCardReaderManager* p = static_cast<CIdCardReaderManager*>(pInstance);
	p->TrueStartThread();
	return 0;
}

DWORD CIdCardReaderManager::TrueStartThread()
{
	//初始化UDP
	char m_ttt[32] = { 0 };
	tchar2char(m_tcFaceUDPIP, m_ttt);
	m_pUDPInstance = new CUDPSelect(m_ttt, _wtoi(m_tcFaceUDPPort));

	//新建读卡设备
	m_CardReaderInstance = nullptr;
	if (0==m_n_device_type)
	{
		//海康访客机
		m_CardReaderInstance = new CIdCardReader;
	}
	else if (1== m_n_device_type)
	{
		//华视电子
	}
	
	

	//循环操作设备
	DWORD   m_dwResult = 0;
	while (true)
	{
		ZeroMemory(&m_stuCardIdMsg, sizeof(m_stuCardIdMsg));

		m_dwResult = WaitForSingleObject(g_hExitEvent, m_nReadCarTimeOut * 1000);
		if (m_dwResult == WAIT_OBJECT_0)
		{
			break;
		}
		//开始链接设备	··1	--------------------以下代码是正式代码-连接身份证读卡器的代码
		if (0== m_n_device_type)
		{
			if (!m_CardReaderInstance->GetDeviceState())
			{
				if (m_CardReaderInstance->LoginDevice())
				{
					strcpy(m_stuCardIdMsg.m_cMsgType, "1");
					m_pUDPInstance->SendMsg(reinterpret_cast<char*>(&m_stuCardIdMsg), sizeof(m_stuCardIdMsg));
				}
				else
				{
					strcpy(m_stuCardIdMsg.m_cMsgType, "0");
					m_pUDPInstance->SendMsg(reinterpret_cast<char*>(&m_stuCardIdMsg), sizeof(m_stuCardIdMsg));
				}
				continue;
			}
			//开始读卡
			if (m_CardReaderInstance->ReadIDCard())
			{
				//读卡成功，发送信息到Face识别服务器
				//将信息发送给人脸识别服务：消息类型，身份证信息，
				strcpy(m_stuCardIdMsg.m_cMsgType, "3");
				memcpy(&m_stuCardIdMsg.m_stuCardInInfo, m_CardReaderInstance->GetSFZCardInfo(), sizeof(SFZCard));
				m_pUDPInstance->SendMsg(reinterpret_cast<char*>(&m_stuCardIdMsg), sizeof(m_stuCardIdMsg));
			}
			else
			{
				strcpy(m_stuCardIdMsg.m_cMsgType, "2");
				m_pUDPInstance->SendMsg(reinterpret_cast<char*>(&m_stuCardIdMsg), sizeof(m_stuCardIdMsg));
			}
			//测试用用---------------------------------------------------------------------------------------
			/*strcpy(m_stuCardIdMsg.m_cMsgType, "3");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._id,"320323198001083015");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._name, "殷剑");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._address, "江苏徐州云龙");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._agency, "江苏徐州");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._birth, "19800108");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._expirestart, "20030607");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._expireend, "20830609");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._nation, "汉");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._sex, "男");
			m_pUDPInstance->SendMsg((char*)&m_stuCardIdMsg, sizeof(m_stuCardIdMsg));*/
		}
		else if (1== m_n_device_type)
		{
			//华视电子
		}
	}
	//关闭设备
	m_CardReaderInstance->CloseDevice();
	//消除UDPInstance
	if (m_pUDPInstance)
	{
		delete m_pUDPInstance;
	}
	if (m_CardReaderInstance)
	{
		delete m_CardReaderInstance;
		m_CardReaderInstance = nullptr;
	}
	return 0;
}


bool CIdCardReaderManager::tchar2char(TCHAR* buf, char* str)
{
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, buf, -1, str, iLength, NULL, NULL);
	return  true;
}
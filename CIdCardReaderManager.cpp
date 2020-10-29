#include "pch.h"
#include "CIdCardReaderManager.h"
#include  "CLogHelper.h"

void CIdCardReaderManager::Start()
{
	//�����̣߳����߳��������豸��Ȼ�󲻶϶�ȡ������
	m_hThreadReadId = chBEGINTHREADEX(0, 0, StartThread, this, 0, NULL);
	if (NULL == m_hThreadReadId)
	{
		CString strLogMsg = _T("����StartThreadʧ��");
		CLogHelper::WriteLog(strLogMsg);
	}
	else
	{
		CString strLogMsg = _T("����StartThread�ɹ�");
		CLogHelper::WriteLog(strLogMsg);
	}
}

void CIdCardReaderManager::Stop()
{
	//ͣ���̣߳��رն�����
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
	//��ʼ��UDP
	char m_ttt[32] = { 0 };
	tchar2char(m_tcFaceUDPIP, m_ttt);
	m_pUDPInstance = new CUDPSelect(m_ttt, _wtoi(m_tcFaceUDPPort));

	//�½������豸
	m_CardReaderInstance = nullptr;
	if (0==m_n_device_type)
	{
		//�����ÿͻ�
		m_CardReaderInstance = new CIdCardReader;
	}
	else if (1== m_n_device_type)
	{
		//���ӵ���
	}
	
	

	//ѭ�������豸
	DWORD   m_dwResult = 0;
	while (true)
	{
		ZeroMemory(&m_stuCardIdMsg, sizeof(m_stuCardIdMsg));

		m_dwResult = WaitForSingleObject(g_hExitEvent, m_nReadCarTimeOut * 1000);
		if (m_dwResult == WAIT_OBJECT_0)
		{
			break;
		}
		//��ʼ�����豸	����1	--------------------���´�������ʽ����-�������֤�������Ĵ���
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
			//��ʼ����
			if (m_CardReaderInstance->ReadIDCard())
			{
				//�����ɹ���������Ϣ��Faceʶ�������
				//����Ϣ���͸�����ʶ�������Ϣ���ͣ����֤��Ϣ��
				strcpy(m_stuCardIdMsg.m_cMsgType, "3");
				memcpy(&m_stuCardIdMsg.m_stuCardInInfo, m_CardReaderInstance->GetSFZCardInfo(), sizeof(SFZCard));
				m_pUDPInstance->SendMsg(reinterpret_cast<char*>(&m_stuCardIdMsg), sizeof(m_stuCardIdMsg));
			}
			else
			{
				strcpy(m_stuCardIdMsg.m_cMsgType, "2");
				m_pUDPInstance->SendMsg(reinterpret_cast<char*>(&m_stuCardIdMsg), sizeof(m_stuCardIdMsg));
			}
			//��������---------------------------------------------------------------------------------------
			/*strcpy(m_stuCardIdMsg.m_cMsgType, "3");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._id,"320323198001083015");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._name, "��");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._address, "������������");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._agency, "��������");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._birth, "19800108");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._expirestart, "20030607");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._expireend, "20830609");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._nation, "��");
			strcpy(m_stuCardIdMsg.m_stuCardInInfo._sex, "��");
			m_pUDPInstance->SendMsg((char*)&m_stuCardIdMsg, sizeof(m_stuCardIdMsg));*/
		}
		else if (1== m_n_device_type)
		{
			//���ӵ���
		}
	}
	//�ر��豸
	m_CardReaderInstance->CloseDevice();
	//����UDPInstance
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
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, buf, -1, str, iLength, NULL, NULL);
	return  true;
}
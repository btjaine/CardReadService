#pragma once
#include "pch.h"
#include "CidCardReader.h"
#include  "CLogHelper.h"
#include  "CUDPSelect.h"

typedef  struct TagCardIdMsg
{
	char  m_cMsgType[8];//��Ϣ���� 0:�����豸ʧ��  1�������豸�ɹ�   2������ʧ��    3�������ɹ�
	SFZCard m_stuCardInInfo;//��ȡ�������֤��Ϣ
}CARD_ID_MSG,*LPCARD_ID_MSG;//���֤�������������ʶ�����ͨ����Ϣ���˴�ͨ��UDP����


extern  HANDLE  g_hExitEvent;


class CIdCardReaderManager
{
public:
	CIdCardReaderManager()
	{
		//��ȡ��������·��2015-12-30
		int    nPos;
		m_strFilePath.Empty();
		GetModuleFileName(NULL, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		m_strFilePath.ReleaseBuffer();
		nPos = m_strFilePath.ReverseFind('\\');
		m_strFilePath = m_strFilePath.Left(nPos + 1);

		//��ȡ��ز���
		//��ȡTCP/IP��������Ϣ���˿ں�
		CString m_strSettringFile =m_strFilePath;
		m_strSettringFile += _T("setting.ini");
		//������ز���
		ZeroMemory(m_tcFaceUDPPort, sizeof(m_tcFaceUDPPort));
		GetPrivateProfileString(_T("FaceServer"), _T("Server_Port"), L"7890", m_tcFaceUDPPort, 10, m_strSettringFile);
		
		ZeroMemory(m_tcFaceUDPIP, sizeof(m_tcFaceUDPIP));
		GetPrivateProfileString(_T("FaceServer"), _T("Server_Ip"), L"127.0.0.1", m_tcFaceUDPIP, 32, m_strSettringFile);
		//ˢ�¶������
		m_nReadCarTimeOut = GetPrivateProfileInt(_T("FaceServer"), _T("ReadCard_TimeOut"), 1, m_strSettringFile);
		//��ȡ���֤�豸����
		m_n_device_type= GetPrivateProfileInt(_T("FaceServer"), _T("DeviceType"), 1, m_strSettringFile);
	};
private:
	CIdCardReader*  m_CardReaderInstance=nullptr;
	HANDLE     m_hThreadReadId;//��ȡ�����̣߳�����ÿ��1���Ӷ�ȡһ��
	TCHAR       m_tcFaceUDPPort[10];//������������UDP���ն˿�
	TCHAR       m_tcFaceUDPIP[64];//������������UDP����IP
	int               m_nReadCarTimeOut;//�������
	CUDPSelect* m_pUDPInstance;//UDPͨѶ
	CARD_ID_MSG    m_stuCardIdMsg;//UDP���͵Ľṹ
	int               m_n_device_type = 0;//�����֤�豸���� 0�����������֤�豸  1�����ӵ���   2�������������
public:
	CString       m_strFilePath;
public:
	void  Start();
	void  Stop();
	static DWORD __stdcall StartThread(void* pInstance);//ҵ���߳̿���
	DWORD TrueStartThread();//������ҵ���߳�
	bool tchar2char(TCHAR* buf, char* str);
};


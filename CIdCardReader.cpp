#include "pch.h"
#include "CIdCardReader.h"
#include "CLogHelper.h"
#include <iostream>
#include <fstream>



USB_SDK_DEVICE_INFO    DeviceInfo;

bool  isOnline = false;

void CALLBACK USBEnumDeviceCallBack(USB_SDK_DEVICE_INFO* pDevceInfo, void* pUser)
{
	if (pDevceInfo == NULL)
	{
		isOnline = false;
	}
	if (strcmp(pDevceInfo->szDeviceName, "DS-K1F110-I") == 0)  //�ҵ�SFZ�������ͰѴ��豸��Ϣ��¼��DeviceInfo�� 
	{
		DeviceInfo.dwSize = pDevceInfo->dwSize;
		memcpy(DeviceInfo.byRes, pDevceInfo->byRes, sizeof(pDevceInfo->byRes));
		DeviceInfo.dwPID = pDevceInfo->dwPID;
		DeviceInfo.dwVID = pDevceInfo->dwVID;
		strcpy(DeviceInfo.szDeviceName, pDevceInfo->szDeviceName);
		strcpy(DeviceInfo.szManufacturer, pDevceInfo->szManufacturer);
		strcpy(DeviceInfo.szSerialNumber, pDevceInfo->szSerialNumber);
		isOnline = true;
	}
}



CIdCardReader::CIdCardReader()
{

	int    nPos;
	m_strFilePath.Empty();
	GetModuleFileName(NULL, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	m_strFilePath.ReleaseBuffer();
	nPos = m_strFilePath.ReverseFind('\\');
	m_strFilePath = m_strFilePath.Left(nPos + 1);

	//��ز���
	m_pUSB_SDK_USER_LOGIN_INFO = new USB_SDK_USER_LOGIN_INFO;
	m_pUSB_SDK_DEVICE_REG_RES = new  USB_SDK_DEVICE_REG_RES;
	//װ��
	m_hDLL = LoadLibrary(L"HCUsbSDK.dll");//���ض�̬���ӿ�MyDll.dll�ļ��� 
	m_hBMPDLL = LoadLibrary(L"WltRS.dll");//���ض�̬���ӿ�MyDll.dll�ļ��� 
	USB_SDK_Login = NULL;
	//��ȡ����
	if (m_hDLL)
	{
		USB_SDK_Init = (pUSB_SDK_Init)GetProcAddress(m_hDLL, "USB_SDK_Init");
		USB_SDK_GetDeviceConfig = (pUSB_SDK_GetDeviceConfig)GetProcAddress(m_hDLL, "USB_SDK_GetDeviceConfig");
		USB_SDK_Login = (pUSB_SDK_Login)GetProcAddress(m_hDLL, "USB_SDK_Login");
		USB_SDK_EnumDevice = (pUSB_SDK_EnumDevice)GetProcAddress(m_hDLL, "USB_SDK_EnumDevice");
		USB_SDK_SetDeviceConfig = (pUSB_SDK_SetDeviceConfig)GetProcAddress(m_hDLL, "USB_SDK_SetDeviceConfig");
		USB_SDK_GetLastError = (pUSB_SDK_GetLastError)GetProcAddress(m_hDLL, "USB_SDK_GetLastError");
		USB_SDK_Logout = (pUSB_SDK_Logout)GetProcAddress(m_hDLL, "USB_SDK_Logout");
	}
	else
	{
		CString strLogMsg = _T("װ��DLLʧ��");
		CLogHelper::WriteLog(strLogMsg);
	}
	if (m_hBMPDLL)
	{
		GetBmp = (pGetBmp)GetProcAddress(m_hBMPDLL, "GetBmp");
	}
	m_lUserID = -1;
}
CIdCardReader::~CIdCardReader()
{
	//ж��DLL
	FreeLibrary(m_hDLL);
	FreeLibrary(m_hBMPDLL);
	//ɾ�����ڴ�
	if (m_pUSB_SDK_USER_LOGIN_INFO)
	{
		delete m_pUSB_SDK_USER_LOGIN_INFO;
	}
	if (m_pUSB_SDK_DEVICE_REG_RES)
	{
		delete m_pUSB_SDK_DEVICE_REG_RES;
	}
	CString strLogMsg = _T("ж��DLL���");
	CLogHelper::WriteLog(strLogMsg);
}

BOOL CIdCardReader::LoginDevice()
{
	if (USB_SDK_Init() == false)
	{
		CString strLogMsg = _T("USB_SDK_Initʧ��");
		CLogHelper::WriteLog(strLogMsg);
	}
	else
	{
		//ö��USB
		USB_SDK_EnumDevice((fEnumDeviceCallBack)USBEnumDeviceCallBack, 0);
		if (isOnline)
		{
			//��ʼ��½
			memset(m_pUSB_SDK_USER_LOGIN_INFO, 0, sizeof(USB_SDK_USER_LOGIN_INFO));
			//��ʼ��
			strcpy(m_pUSB_SDK_USER_LOGIN_INFO->szUserName, "admin");
			strcpy(m_pUSB_SDK_USER_LOGIN_INFO->szPassword, "12345");
			m_pUSB_SDK_USER_LOGIN_INFO->dwSize = sizeof(USB_SDK_USER_LOGIN_INFO);
			m_pUSB_SDK_USER_LOGIN_INFO->dwVID = DeviceInfo.dwVID;
			m_pUSB_SDK_USER_LOGIN_INFO->dwPID = DeviceInfo.dwPID;
			m_pUSB_SDK_USER_LOGIN_INFO->dwTimeout = 5000;
			strcpy(m_pUSB_SDK_USER_LOGIN_INFO->szSerialNumber, DeviceInfo.szSerialNumber);

			memset(m_pUSB_SDK_DEVICE_REG_RES, 0, sizeof(USB_SDK_DEVICE_REG_RES));
			m_pUSB_SDK_DEVICE_REG_RES->dwSize = sizeof(USB_SDK_DEVICE_REG_RES);
			//��½
			if (USB_SDK_Login)
			{
				m_lUserID = USB_SDK_Login(m_pUSB_SDK_USER_LOGIN_INFO, m_pUSB_SDK_DEVICE_REG_RES);
				if (m_lUserID == -1)
				{
					CString strLogMsg = _T("��½ʧ��");
					CLogHelper::WriteLog(strLogMsg);
				}
				else
				{
					CString strLogMsg = _T("��½�豸�ɹ�");
					CLogHelper::WriteLog(strLogMsg);
					SetParam();
					SetCardTypeParameter(1);
				}
			}
			else
			{
				CString strLogMsg = _T("��ȡUSB_SDK_Loginʧ��");
				CLogHelper::WriteLog(strLogMsg);
			}
		}
		else
		{
			CString strLogMsg = _T("û�з����豸");
			CLogHelper::WriteLog(strLogMsg);
		}
	}
	return isOnline;
}

BOOL CIdCardReader::ReadIDCard()
{
	bool m_bResult = false;
	if (m_lUserID >= 0)
	{
		 m_bResult = USB_SDK_GetDeviceConfig(m_lUserID, 1000, &InputInfo, &ReadOutputInfo);
		if (m_bResult)
		{
			USB_SDK_SetDeviceConfig(m_lUserID, 0x0100, &ConfigInputInfo, &ConfigOutputInfo);

			// HIKCardReaderAPI.tagUSB_SDK_CERTIFICATE_INFO certificateinfo = ( HIKCardReaderAPI.tagUSB_SDK_CERTIFICATE_INFO ) Marshal.PtrToStructure ( ReadOutputInfo.lpOutBuffer,
			//typeof(HIKCardReaderAPI.tagUSB_SDK_CERTIFICATE_INFO) );
			USB_SDK_CERTIFICATE_INFO certificateinfo;
			memcpy(&certificateinfo, ReadOutputInfo.lpOutBuffer, sizeof(USB_SDK_CERTIFICATE_INFO));
			//��ʼ����
			char    m_cResult[4096] = { 0 };
			char* m_LeftSource = NULL;
			TcharToChar((wchar_t*)certificateinfo.byWordInfo, m_cResult);
			//��ʼ��ȡ�ֶ�ÿ���ֶ�֮����0x20�ָ�
			memset(&m_stuCardInfo, 0, sizeof(m_stuCardInfo));
			//����16���ֽ�-------------------------------------------------------------------
			int m_nPosition = 0;
			//��ȡ��������Ч�ֽ���
			m_nPosition=GetValidfieldLen(m_cResult, certificateinfo.wWordInfoSize, m_stuCardInfo._name);
			//memcpy(m_stuCardInfo._name, m_cResult, 17);
			//m_nPosition = 17;
			//�Ա� 1���ֽ�--------------------------------------------------------------------
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_cResult + m_nPosition;
			//��ȡ�Ա��ֽ�
			m_nPosition = 1; //= GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._sex);
			memcpy(m_stuCardInfo._sex, m_LeftSource, 1);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			

			if (strcmp(m_stuCardInfo._sex, "1") == 0)
			{
				strcpy(m_stuCardInfo._sex, "��");
			}
			else
			{
				strcpy(m_stuCardInfo._sex, "Ů");
			}
			//m_nPosition += 1;
			//����2���ֽ�--------------------------------------------------------------------
			m_nPosition=2;// = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._nation);
			memcpy(m_stuCardInfo._nation, m_LeftSource, 2);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//���庺��
			CString  _nation = L"";
			if (strlen(m_stuCardInfo._nation) > 0)
			{
				switch (atoi(m_stuCardInfo._nation))
				{
					case 1:
						_nation = L"��";
						break;
					case 2:
						_nation = L"�ɹ�";
						break;
					case 3:
						_nation = L"��";
						break;
					case 4:
						_nation = L"��";
						break;
					case 5:
						_nation = L"ά���";
						break;
					case 6:
						_nation = L"��";
						break;
					case 7:
						_nation = L"��";
						break;
					case 8:
						_nation = L"׳";
						break;
					case 9:
						_nation = L"����";
						break;
					case 10:
						_nation = L"����";
						break;
					case 11:
						_nation = L"��";
						break;
					case 12:
						_nation = L"��";
						break;
					case 13:
						_nation = L"��";
						break;
					case 14:
						_nation = L"��";
						break;
					case 15:
						_nation = L"����";
						break;
					case 16:
						_nation = L"����";
						break;
					case 17:
						_nation = L"������";
						break;
					case 18:
						_nation = L"��";
						break;
					case 19:
						_nation = L"��";
						break;
					case 20:
						_nation = L"����";
						break;
					case 21:
						_nation = L"��";
						break;
					case 22:
						_nation = L"�";
						break;
					case 23:
						_nation = L"��ɽ";
						break;
					case 24:
						_nation = L"����";
						break;
					case 25:
						_nation = L"ˮ";
						break;
					case 26:
						_nation = L"����";
						break;
					case 27:
						_nation = L"����";
						break;
					case 28:
						_nation = L"����";
						break;
					case 29:
						_nation = L"�¶�����";
						break;
					case 30:
						_nation = L"��";
						break;
					case 31:
						_nation = L"���Ӷ�";
						break;
					case 32:
						_nation = L"����";
						break;
					case 33:
						_nation = L"Ǽ";
						break;
					case 34:
						_nation = L"����";
						break;
					case 35:
						_nation = L"����";
						break;
					case 36:
						_nation = L"ë��";
						break;
					case 37:
						_nation = L"����";
						break;
					case 38:
						_nation = L"����";
						break;
					case 39:
						_nation = L"����";
						break;
					case 40:
						_nation = L"����";
						break;
					case 41:
						_nation = L"������";
						break;
					case 42:
						_nation = L"ŭ";
						break;
					case 43:
						_nation = L"���α��";
						break;
					case 44:
						_nation = L"����˹";
						break;
					case 45:
						_nation = L"���¿�";
						break;
					case 46:
						_nation = L"�°�";
						break;
					case 47:
						_nation = L"����";
						break;
					case 48:
						_nation = L"ԣ��";
						break;
					case 49:
						_nation = L"��";
						break;
					case 50:
						_nation = L"������";
						break;
					case 51:
						_nation = L"����";
						break;
					case 52:
						_nation = L"���״�";
						break;
					case 53:
						_nation = L"����";
						break;
					case 54:
						_nation = L"�Ű�";
						break;
					case 55:
						_nation = L"���";
						break;
					case 56:
						_nation = L"��ŵ";
						break;
					default:
						_nation = L"����";
						break;
				}
				TcharToChar(_nation.GetBuffer(0), m_stuCardInfo._nation);
			}
			//m_nPosition += 2;
			//����--------------------------------------------------------------------
			m_nPosition = 8;// = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._birth);
			memcpy(m_stuCardInfo._birth, m_LeftSource, 8);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//��ַ--------------------------------------------------------------------
			m_nPosition = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._address);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;

			//memcpy(m_stuCardInfo._address, m_cResult + m_nPosition, 37);
			//m_nPosition += 37;

			//���֤����--------------------------------------------------------------------
			memcpy(m_stuCardInfo._id, m_LeftSource, 18);
			m_nPosition = 18;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//_agency------------��֤����--------------------------------------------------------
			//memcpy(m_stuCardInfo._agency, m_LeftSource, 25);
			 m_nPosition = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._agency);// 25;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//_expirestart----------��ʼ��Ч��----------------------------------------------------------
			memcpy(m_stuCardInfo._expirestart, m_LeftSource, 8);
			m_nPosition = 8;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//_expirestart--------������Ч��------------------------------------------------------------
			memcpy(m_stuCardInfo._expireend, m_LeftSource, 8);
			m_nPosition = 8;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			/*//_txzhm--------------------------------------------------------------------
			memcpy(m_stuCardInfo._txzhm, m_cResult + m_nPosition, 10);
			m_nPosition += 10;
			//_qfcs--------------------------------------------------------------------
			memcpy(m_stuCardInfo._qfcs, m_cResult + m_nPosition, 3);
			m_nPosition += 10;

			//_zjlxbs--------------------------------------------------------------------
			memcpy(m_stuCardInfo._zjlxbs, m_cResult + m_nPosition, 2);*/
			//����BMP�ļ�
			char m_cFilePath[1024] = { 0 };
			char m_cFileBmpPath[1024] = { 0 };
			TcharToChar(m_strFilePath.GetBuffer(0), m_cFileBmpPath);
			sprintf(m_cFilePath, "%sVistorPhoto/%s.wlt", m_cFileBmpPath, m_stuCardInfo._id);
			//sprintf(m_cFileBmpPath, "%s%s.bmp", m_strFilePath.GetBuffer(0),m_stuCardInfo._id);
			std::ofstream fout(m_cFilePath, std::ios_base::out | std::ios_base::binary);
			fout.write((char*)certificateinfo.byPicInfo, certificateinfo.wPicInfoSize);
			fout.flush();
			fout.close();
			//����BMP
			GetBmp(m_cFilePath, 2);
			m_bResult = true;
		}
		else
		{
			//AfxMessageBox("����ʧ��!");
			m_bResult = false;
		}
	}
	return m_bResult;
}

BOOL CIdCardReader::EncodeWlt(char* cFileName, int nType)
{
	//nType: 1:Com   2:USB
	//cFileName:.Wlt�ļ�
	return GetBmp(cFileName, nType);
}

BOOL CIdCardReader::CloseDevice()
{
	bool ret = false;

	if (m_lUserID != -1)
	{
		ret = USB_SDK_Logout(m_lUserID);
	}
	m_lUserID = -1;
	isOnline = false;
	return isOnline;
}

BOOL CIdCardReader::GetDeviceState()
{
	return isOnline;
}

SFZCard* CIdCardReader::GetSFZCardInfo()
{
	return  &m_stuCardInfo;
}

void CIdCardReader::SetParam()
{
/*------------------------------�����֤��������----------------------------------------------*/
/*-----------------------------------��������˸��������------------------------------------------*/

	//ConfigOutputInfo = new HIKCardReaderAPI.tagUSB_CONFIG_OUTPUT_INFO();
	//ConfigInputInfo = new HIKCardReaderAPI.tagUSB_CONFIG_INPUT_INFO();

	beepflick.dwSize = sizeof(USB_SDK_BEEP_AND_FLICKER);
	beepflick.byBeepType = 3;
	beepflick.byBeepCount = 2;
	beepflick.byFlickerType = 4;
	beepflick.byFlickerCount = 2;

	ConfigInputInfo.dwInBufferSize = beepflick.dwSize;
	LPDWORD inptr = new DWORD[beepflick.dwSize];
	//Marshal.StructureToPtr(beepflick, inptr, true);
	memcpy(inptr, &beepflick, sizeof(beepflick));
	ConfigInputInfo.lpInBuffer = inptr;
}

void CIdCardReader::SetCardTypeParameter(int timeout)
{
	memset(&InputInfo, 0, sizeof(USB_CONFIG_INPUT_INFO));
	memset(&FindOutputInfo, 0, sizeof(USB_CONFIG_OUTPUT_INFO));
	memset(&ReadOutputInfo, 0, sizeof(USB_CONFIG_OUTPUT_INFO));

	memset(&waitsecond, 0, sizeof(USB_SDK_WAIT_SECOND));
	waitsecond.dwSize = sizeof(USB_SDK_WAIT_SECOND);


	InputInfo.dwInBufferSize = waitsecond.dwSize;
	waitsecond.byWait = timeout;
	InputInfo.lpInBuffer = new DWORD[waitsecond.dwSize];
	//��waitsecond ���Ƶ�InputInfo.lpInBuffer ��
	//Marshal.StructureToPtr ( waitsecond, InputInfo.lpInBuffer, true );
	memcpy(InputInfo.lpInBuffer, &waitsecond, sizeof(waitsecond));


	memset(&cardInfo, 0, sizeof(cardInfo));
	cardInfo.dwSize = sizeof(cardInfo);
	FindOutputInfo.dwOutBufferSize = cardInfo.dwSize;
	FindOutputInfo.lpOutBuffer = new DWORD[cardInfo.dwSize];
	//Marshal.StructureToPtr ( cardInfo, FindOutputInfo.lpOutBuffer, true );
	memcpy(FindOutputInfo.lpOutBuffer, &cardInfo, sizeof(cardInfo));

	memset(&CertificateInfo, 0, sizeof(CertificateInfo));
	CertificateInfo.dwSize = sizeof(CertificateInfo);
	ReadOutputInfo.dwOutBufferSize = CertificateInfo.dwSize;
	ReadOutputInfo.lpOutBuffer = new DWORD[CertificateInfo.dwSize];
	//Marshal.StructureToPtr(CertificateInfo, ReadOutputInfo.lpOutBuffer, true);
	memcpy(ReadOutputInfo.lpOutBuffer, &CertificateInfo, sizeof(CertificateInfo));
}

bool CIdCardReader::TcharToChar(const wchar_t* tchar, char* _char)
{
	int iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
	return  true;
}

bool  CIdCardReader::CharToTchar(const char* _char, wchar_t* tchar)
{
	int iLength;
	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
	return true;
}

int CIdCardReader::GetValidfieldLen(char* cSource,int cSourceLen,char* cFieldValue)
{
	int n_AllFieldLen = 0;//�����ֽڳ���
	if (cSource == NULL || strlen(cSource) <= 0)  return -1;
	bool m_bIsValidEnd = false;//�Ƿ�����Ч�ֽڵ�ĩβ
	for (int i = 0; i < cSourceLen; i++)
	{
		n_AllFieldLen = i;
		if (cSource[i] == 0x20)
		{
			if (m_bIsValidEnd==false)
			{
				m_bIsValidEnd = true;
			}
		}
		else
		{
			if (m_bIsValidEnd==false)
			{
				cFieldValue[i] = cSource[i];
			}
			else
			{
				//������һ���ֶ�������
				break;
			}
		}
	}
	return n_AllFieldLen;
}

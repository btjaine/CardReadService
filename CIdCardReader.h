#pragma once
#include  "CardIDReader.h"

//����ָ������

typedef BOOL(__stdcall* pUSB_SDK_Init)();
typedef BOOL(__stdcall* pUSB_SDK_Logout)(LONG lUserID);
typedef DWORD(__stdcall* pUSB_SDK_GetLastError)();
typedef BOOL(__stdcall* pUSB_SDK_GetDeviceConfig)(LONG lUserID, DWORD dwCommand, LPUSB_CONFIG_INPUT_INFO pInputInfo, LPUSB_CONFIG_OUTPUT_INFO pOutputInfo);//��ȡ�豸����
typedef LONG(__stdcall* pUSB_SDK_Login)(LPUSB_SDK_USER_LOGIN_INFO pUsbLoginInfo, LPUSB_SDK_DEVICE_REG_RES pDevRegRes);//��¼�豸
typedef BOOL(__stdcall* pUSB_SDK_EnumDevice)(fEnumDeviceCallBack cbEnumDeviceCallBack, void* pUser);//ö��USB�豸
typedef BOOL(__stdcall* pUSB_SDK_SetDeviceConfig)(LONG lUserID, DWORD dwCommand, LPUSB_CONFIG_INPUT_INFO pConfigInputInfo, LPUSB_CONFIG_OUTPUT_INFO pConfigOutputInfo);//���ò���

typedef int(__stdcall* pGetBmp)(char* file_name, int intf);//����ͼƬ



class CIdCardReader
{
public:
	CIdCardReader();
	~CIdCardReader();
private:
	CString m_strFilePath;
	HINSTANCE  m_hDLL;
	HINSTANCE  m_hBMPDLL;
	LONG  m_lUserID;//�豸��½ID
	//��½�õ���Դ
	LPUSB_SDK_USER_LOGIN_INFO  m_pUSB_SDK_USER_LOGIN_INFO;
	LPUSB_SDK_DEVICE_REG_RES    m_pUSB_SDK_DEVICE_REG_RES;
	USB_CONFIG_INPUT_INFO       ConfigInputInfo;
	USB_CONFIG_OUTPUT_INFO    ConfigOutputInfo;
	USB_SDK_BEEP_AND_FLICKER  beepflick;
	USB_CONFIG_INPUT_INFO    InputInfo;
	USB_CONFIG_OUTPUT_INFO FindOutputInfo;
	USB_CONFIG_OUTPUT_INFO ReadOutputInfo;
	USB_SDK_WAIT_SECOND waitsecond;
	USB_SDK_ACTIVATE_CARD_RES cardInfo;
	USB_SDK_CERTIFICATE_INFO CertificateInfo;
	SFZCard                                     m_stuCardInfo;
private:
	pUSB_SDK_Init USB_SDK_Init;//��ʼ��
	pUSB_SDK_GetDeviceConfig USB_SDK_GetDeviceConfig;//��ȡ����
	pUSB_SDK_Login USB_SDK_Login;//��½�豸
	pUSB_SDK_EnumDevice USB_SDK_EnumDevice;//ö��USB
	pUSB_SDK_SetDeviceConfig   USB_SDK_SetDeviceConfig;//���ò���
	pUSB_SDK_GetLastError    USB_SDK_GetLastError;
	pGetBmp                             GetBmp;//����BMP�ļ�
	pUSB_SDK_Logout    USB_SDK_Logout;
public:
	//����
	BOOL LoginDevice();//��½�豸
	BOOL ReadIDCard();//��ʼ����
	BOOL EncodeWlt(char*  cFileName,int nType);//����wlt�ļ�����������ͬĿ¼�²���һ����ͬ��BMP�ļ���
	BOOL CloseDevice();//�ر��豸
	BOOL GetDeviceState();//�õ��豸״̬
	SFZCard* GetSFZCardInfo();//��ȡ��ȡ�������֤����Ϣ
private:
	void SetParam();
	void SetCardTypeParameter(int timeout);
	bool TcharToChar(const wchar_t* tchar, char* _char);
	bool CharToTchar(const char* _char, wchar_t* tchar);
	int    GetValidfieldLen(char* cSource, int cSourceLen, char* cFieldValue);//����ֵ����Ч�ֶγ��� nSpaceLen:�ǿո�ĳ���  ����Ч����+�ո񳤶�=���ֶεĳ���
};


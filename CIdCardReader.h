#pragma once
#include  "CardIDReader.h"

//函数指针申明

typedef BOOL(__stdcall* pUSB_SDK_Init)();
typedef BOOL(__stdcall* pUSB_SDK_Logout)(LONG lUserID);
typedef DWORD(__stdcall* pUSB_SDK_GetLastError)();
typedef BOOL(__stdcall* pUSB_SDK_GetDeviceConfig)(LONG lUserID, DWORD dwCommand, LPUSB_CONFIG_INPUT_INFO pInputInfo, LPUSB_CONFIG_OUTPUT_INFO pOutputInfo);//获取设备参数
typedef LONG(__stdcall* pUSB_SDK_Login)(LPUSB_SDK_USER_LOGIN_INFO pUsbLoginInfo, LPUSB_SDK_DEVICE_REG_RES pDevRegRes);//登录设备
typedef BOOL(__stdcall* pUSB_SDK_EnumDevice)(fEnumDeviceCallBack cbEnumDeviceCallBack, void* pUser);//枚举USB设备
typedef BOOL(__stdcall* pUSB_SDK_SetDeviceConfig)(LONG lUserID, DWORD dwCommand, LPUSB_CONFIG_INPUT_INFO pConfigInputInfo, LPUSB_CONFIG_OUTPUT_INFO pConfigOutputInfo);//设置参数

typedef int(__stdcall* pGetBmp)(char* file_name, int intf);//解析图片



class CIdCardReader
{
public:
	CIdCardReader();
	~CIdCardReader();
private:
	CString m_strFilePath;
	HINSTANCE  m_hDLL;
	HINSTANCE  m_hBMPDLL;
	LONG  m_lUserID;//设备登陆ID
	//登陆用的资源
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
	pUSB_SDK_Init USB_SDK_Init;//初始化
	pUSB_SDK_GetDeviceConfig USB_SDK_GetDeviceConfig;//获取配置
	pUSB_SDK_Login USB_SDK_Login;//登陆设备
	pUSB_SDK_EnumDevice USB_SDK_EnumDevice;//枚举USB
	pUSB_SDK_SetDeviceConfig   USB_SDK_SetDeviceConfig;//设置参数
	pUSB_SDK_GetLastError    USB_SDK_GetLastError;
	pGetBmp                             GetBmp;//解密BMP文件
	pUSB_SDK_Logout    USB_SDK_Logout;
public:
	//方法
	BOOL LoginDevice();//登陆设备
	BOOL ReadIDCard();//开始读卡
	BOOL EncodeWlt(char*  cFileName,int nType);//解密wlt文件，，会在相同目录下产生一个相同的BMP文件。
	BOOL CloseDevice();//关闭设备
	BOOL GetDeviceState();//得到设备状态
	SFZCard* GetSFZCardInfo();//获取读取到的身份证的信息
private:
	void SetParam();
	void SetCardTypeParameter(int timeout);
	bool TcharToChar(const wchar_t* tchar, char* _char);
	bool CharToTchar(const char* _char, wchar_t* tchar);
	int    GetValidfieldLen(char* cSource, int cSourceLen, char* cFieldValue);//返回值是有效字段长度 nSpaceLen:是空格的长度  ，有效长度+空格长度=该字段的长度
};


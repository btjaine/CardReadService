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
	if (strcmp(pDevceInfo->szDeviceName, "DS-K1F110-I") == 0)  //找到SFZ读卡器就把此设备信息记录到DeviceInfo里 
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

	//相关参数
	m_pUSB_SDK_USER_LOGIN_INFO = new USB_SDK_USER_LOGIN_INFO;
	m_pUSB_SDK_DEVICE_REG_RES = new  USB_SDK_DEVICE_REG_RES;
	//装载
	m_hDLL = LoadLibrary(L"HCUsbSDK.dll");//加载动态链接库MyDll.dll文件； 
	m_hBMPDLL = LoadLibrary(L"WltRS.dll");//加载动态链接库MyDll.dll文件； 
	USB_SDK_Login = NULL;
	//获取函数
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
		CString strLogMsg = _T("装载DLL失败");
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
	//卸载DLL
	FreeLibrary(m_hDLL);
	FreeLibrary(m_hBMPDLL);
	//删掉用内存
	if (m_pUSB_SDK_USER_LOGIN_INFO)
	{
		delete m_pUSB_SDK_USER_LOGIN_INFO;
	}
	if (m_pUSB_SDK_DEVICE_REG_RES)
	{
		delete m_pUSB_SDK_DEVICE_REG_RES;
	}
	CString strLogMsg = _T("卸载DLL完成");
	CLogHelper::WriteLog(strLogMsg);
}

BOOL CIdCardReader::LoginDevice()
{
	if (USB_SDK_Init() == false)
	{
		CString strLogMsg = _T("USB_SDK_Init失败");
		CLogHelper::WriteLog(strLogMsg);
	}
	else
	{
		//枚举USB
		USB_SDK_EnumDevice((fEnumDeviceCallBack)USBEnumDeviceCallBack, 0);
		if (isOnline)
		{
			//开始登陆
			memset(m_pUSB_SDK_USER_LOGIN_INFO, 0, sizeof(USB_SDK_USER_LOGIN_INFO));
			//初始化
			strcpy(m_pUSB_SDK_USER_LOGIN_INFO->szUserName, "admin");
			strcpy(m_pUSB_SDK_USER_LOGIN_INFO->szPassword, "12345");
			m_pUSB_SDK_USER_LOGIN_INFO->dwSize = sizeof(USB_SDK_USER_LOGIN_INFO);
			m_pUSB_SDK_USER_LOGIN_INFO->dwVID = DeviceInfo.dwVID;
			m_pUSB_SDK_USER_LOGIN_INFO->dwPID = DeviceInfo.dwPID;
			m_pUSB_SDK_USER_LOGIN_INFO->dwTimeout = 5000;
			strcpy(m_pUSB_SDK_USER_LOGIN_INFO->szSerialNumber, DeviceInfo.szSerialNumber);

			memset(m_pUSB_SDK_DEVICE_REG_RES, 0, sizeof(USB_SDK_DEVICE_REG_RES));
			m_pUSB_SDK_DEVICE_REG_RES->dwSize = sizeof(USB_SDK_DEVICE_REG_RES);
			//登陆
			if (USB_SDK_Login)
			{
				m_lUserID = USB_SDK_Login(m_pUSB_SDK_USER_LOGIN_INFO, m_pUSB_SDK_DEVICE_REG_RES);
				if (m_lUserID == -1)
				{
					CString strLogMsg = _T("登陆失败");
					CLogHelper::WriteLog(strLogMsg);
				}
				else
				{
					CString strLogMsg = _T("登陆设备成功");
					CLogHelper::WriteLog(strLogMsg);
					SetParam();
					SetCardTypeParameter(1);
				}
			}
			else
			{
				CString strLogMsg = _T("获取USB_SDK_Login失败");
				CLogHelper::WriteLog(strLogMsg);
			}
		}
		else
		{
			CString strLogMsg = _T("没有发现设备");
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
			//开始解析
			char    m_cResult[4096] = { 0 };
			char* m_LeftSource = NULL;
			TcharToChar((wchar_t*)certificateinfo.byWordInfo, m_cResult);
			//开始获取字段每个字段之间是0x20分割
			memset(&m_stuCardInfo, 0, sizeof(m_stuCardInfo));
			//姓名16个字节-------------------------------------------------------------------
			int m_nPosition = 0;
			//获取姓名的有效字节数
			m_nPosition=GetValidfieldLen(m_cResult, certificateinfo.wWordInfoSize, m_stuCardInfo._name);
			//memcpy(m_stuCardInfo._name, m_cResult, 17);
			//m_nPosition = 17;
			//性别 1个字节--------------------------------------------------------------------
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_cResult + m_nPosition;
			//获取性别字节
			m_nPosition = 1; //= GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._sex);
			memcpy(m_stuCardInfo._sex, m_LeftSource, 1);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			

			if (strcmp(m_stuCardInfo._sex, "1") == 0)
			{
				strcpy(m_stuCardInfo._sex, "男");
			}
			else
			{
				strcpy(m_stuCardInfo._sex, "女");
			}
			//m_nPosition += 1;
			//民族2个字节--------------------------------------------------------------------
			m_nPosition=2;// = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._nation);
			memcpy(m_stuCardInfo._nation, m_LeftSource, 2);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//民族汉字
			CString  _nation = L"";
			if (strlen(m_stuCardInfo._nation) > 0)
			{
				switch (atoi(m_stuCardInfo._nation))
				{
					case 1:
						_nation = L"汉";
						break;
					case 2:
						_nation = L"蒙古";
						break;
					case 3:
						_nation = L"回";
						break;
					case 4:
						_nation = L"藏";
						break;
					case 5:
						_nation = L"维吾尔";
						break;
					case 6:
						_nation = L"苗";
						break;
					case 7:
						_nation = L"彝";
						break;
					case 8:
						_nation = L"壮";
						break;
					case 9:
						_nation = L"布依";
						break;
					case 10:
						_nation = L"朝鲜";
						break;
					case 11:
						_nation = L"满";
						break;
					case 12:
						_nation = L"侗";
						break;
					case 13:
						_nation = L"瑶";
						break;
					case 14:
						_nation = L"白";
						break;
					case 15:
						_nation = L"土家";
						break;
					case 16:
						_nation = L"哈尼";
						break;
					case 17:
						_nation = L"哈萨克";
						break;
					case 18:
						_nation = L"傣";
						break;
					case 19:
						_nation = L"黎";
						break;
					case 20:
						_nation = L"傈僳";
						break;
					case 21:
						_nation = L"佤";
						break;
					case 22:
						_nation = L"畲";
						break;
					case 23:
						_nation = L"高山";
						break;
					case 24:
						_nation = L"拉祜";
						break;
					case 25:
						_nation = L"水";
						break;
					case 26:
						_nation = L"东乡";
						break;
					case 27:
						_nation = L"纳西";
						break;
					case 28:
						_nation = L"景颇";
						break;
					case 29:
						_nation = L"柯尔克孜";
						break;
					case 30:
						_nation = L"土";
						break;
					case 31:
						_nation = L"达斡尔";
						break;
					case 32:
						_nation = L"仫佬";
						break;
					case 33:
						_nation = L"羌";
						break;
					case 34:
						_nation = L"布朗";
						break;
					case 35:
						_nation = L"撒拉";
						break;
					case 36:
						_nation = L"毛南";
						break;
					case 37:
						_nation = L"仡佬";
						break;
					case 38:
						_nation = L"锡伯";
						break;
					case 39:
						_nation = L"阿昌";
						break;
					case 40:
						_nation = L"普米";
						break;
					case 41:
						_nation = L"塔吉克";
						break;
					case 42:
						_nation = L"怒";
						break;
					case 43:
						_nation = L"乌孜别克";
						break;
					case 44:
						_nation = L"俄罗斯";
						break;
					case 45:
						_nation = L"鄂温克";
						break;
					case 46:
						_nation = L"德昂";
						break;
					case 47:
						_nation = L"保安";
						break;
					case 48:
						_nation = L"裕固";
						break;
					case 49:
						_nation = L"京";
						break;
					case 50:
						_nation = L"塔塔尔";
						break;
					case 51:
						_nation = L"独龙";
						break;
					case 52:
						_nation = L"鄂伦春";
						break;
					case 53:
						_nation = L"赫哲";
						break;
					case 54:
						_nation = L"门巴";
						break;
					case 55:
						_nation = L"珞巴";
						break;
					case 56:
						_nation = L"基诺";
						break;
					default:
						_nation = L"错误";
						break;
				}
				TcharToChar(_nation.GetBuffer(0), m_stuCardInfo._nation);
			}
			//m_nPosition += 2;
			//生日--------------------------------------------------------------------
			m_nPosition = 8;// = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._birth);
			memcpy(m_stuCardInfo._birth, m_LeftSource, 8);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//地址--------------------------------------------------------------------
			m_nPosition = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._address);
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;

			//memcpy(m_stuCardInfo._address, m_cResult + m_nPosition, 37);
			//m_nPosition += 37;

			//身份证号码--------------------------------------------------------------------
			memcpy(m_stuCardInfo._id, m_LeftSource, 18);
			m_nPosition = 18;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//_agency------------发证机关--------------------------------------------------------
			//memcpy(m_stuCardInfo._agency, m_LeftSource, 25);
			 m_nPosition = GetValidfieldLen(m_LeftSource, certificateinfo.wWordInfoSize, m_stuCardInfo._agency);// 25;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//_expirestart----------开始有效期----------------------------------------------------------
			memcpy(m_stuCardInfo._expirestart, m_LeftSource, 8);
			m_nPosition = 8;
			certificateinfo.wWordInfoSize -= m_nPosition;
			m_LeftSource = m_LeftSource + m_nPosition;
			//_expirestart--------结束有效期------------------------------------------------------------
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
			//生成BMP文件
			char m_cFilePath[1024] = { 0 };
			char m_cFileBmpPath[1024] = { 0 };
			TcharToChar(m_strFilePath.GetBuffer(0), m_cFileBmpPath);
			sprintf(m_cFilePath, "%sVistorPhoto/%s.wlt", m_cFileBmpPath, m_stuCardInfo._id);
			//sprintf(m_cFileBmpPath, "%s%s.bmp", m_strFilePath.GetBuffer(0),m_stuCardInfo._id);
			std::ofstream fout(m_cFilePath, std::ios_base::out | std::ios_base::binary);
			fout.write((char*)certificateinfo.byPicInfo, certificateinfo.wPicInfoSize);
			fout.flush();
			fout.close();
			//解密BMP
			GetBmp(m_cFilePath, 2);
			m_bResult = true;
		}
		else
		{
			//AfxMessageBox("读卡失败!");
			m_bResult = false;
		}
	}
	return m_bResult;
}

BOOL CIdCardReader::EncodeWlt(char* cFileName, int nType)
{
	//nType: 1:Com   2:USB
	//cFileName:.Wlt文件
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
/*------------------------------读身份证参数设置----------------------------------------------*/
/*-----------------------------------蜂鸣及闪烁参数设置------------------------------------------*/

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
	//将waitsecond 复制到InputInfo.lpInBuffer 中
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
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char    
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
	int n_AllFieldLen = 0;//整个字节长度
	if (cSource == NULL || strlen(cSource) <= 0)  return -1;
	bool m_bIsValidEnd = false;//是否到了有效字节的末尾
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
				//到了下一个字段内容了
				break;
			}
		}
	}
	return n_AllFieldLen;
}

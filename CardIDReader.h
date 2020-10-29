#pragma once
#include "pch.h"


#define  USB_SDK_API   extern "C"

typedef  struct tagUSB_SDK_USER_LOGIN_INFO
{
	DWORD dwSize; //�ṹ���С
	DWORD dwTimeout; //��¼��ʱʱ�䣨��λ�����룩
	DWORD dwVID;  //�豸VID��ö���豸ʱ�õ�
	DWORD dwPID;  //�豸PID��ö���豸ʱ�õ�
	char szUserName[32];           //[MAX_USERNAME_LEN/*32*/]; //�û���
	char szPassword[16];          //[MAX_PASSWORD_LEN/*16*/]; //����
	char szSerialNumber[48];       //[MAX_SERIAL_NUM_LEN/*48*/]; //�豸���кţ�ö���豸ʱ�õ�
	BYTE byRes[80];
}USB_SDK_USER_LOGIN_INFO,*LPUSB_SDK_USER_LOGIN_INFO;



typedef  struct tagUSB_SDK_DEVICE_INFO
{
	DWORD dwSize;   //�ṹ���С
	DWORD dwVID;   //�豸VID
	DWORD dwPID;   //�豸PID
	char szManufacturer[32];  //[MAX_MANUFACTURE_LEN/*32*/];//�����̣�������������
	char szDeviceName[32];  //[MAX_DEVICE_NAME_LEN/*32*/];//�豸���ƣ�������������
	char szSerialNumber[48];  //[MAX_SERIAL_NUM_LEN/*48*/];//�豸���кţ�������������
    BYTE  byRes[68];
}USB_SDK_DEVICE_INFO,*LPUSB_SDK_DEVICE_INFO;


typedef  struct tagUSB_SDK_DEVICE_REG_RES
{
	DWORD dwSize;   //�ṹ���С
	char szDeviceName[32];   //[MAX_DEVICE_NAME_LEN /*32*/]; //�豸����
	char szSerialNumber[48];  //[MAX_SERIAL_NUM_LEN /*48*/]; //�豸���к�
	DWORD dwSoftwareVersion; //����汾��,��16λ�����汾,��16λ�Ǵΰ汾
    BYTE byRes[40];
}USB_SDK_DEVICE_REG_RES,*LPUSB_SDK_DEVICE_REG_RES;


typedef  struct tagUSB_SDK_CERTIFICATE_INFO
{
	DWORD dwSize; //�ṹ���С
	WORD wWordInfoSize; //������Ϣ����
	WORD wPicInfoSize; //��Ƭ��Ϣ����
	WORD wFingerPrintInfoSize; //ָ����Ϣ����
	BYTE  byRes2[2];
	BYTE  byWordInfo[256];   //[WORD_LEN/*256*/]; //������Ϣ
	BYTE  byPicInfo[1024];   //[PIC_LEN/*1024*/]; //��Ƭ��Ϣ
	BYTE  byFingerPrintInfo[1024];    //[FINGER_PRINT_LEN/*1024*/]; //ָ����Ϣ
	BYTE  byRes[40];
}USB_SDK_CERTIFICATE_INFO,*LPUSB_SDK_CERTIFICATE_INFO;


typedef struct tagUSB_CONFIG_INPUT_INFO
{
	LPDWORD lpCondBuffer;        //ָ������������
	DWORD dwCondBufferSize;//������������С
	LPDWORD lpInBuffer;          //ָ�����������
	DWORD dwInBufferSize;   //���뻺������С
	BYTE byRes[48];
}USB_CONFIG_INPUT_INFO,*LPUSB_CONFIG_INPUT_INFO;

typedef  struct tagUSB_CONFIG_OUTPUT_INFO
{
	LPDWORD lpOutBuffer;      //ָ�����������
	DWORD dwOutBufferSize;  //�����������С
	BYTE byRes[56];
}USB_CONFIG_OUTPUT_INFO,*LPUSB_CONFIG_OUTPUT_INFO;

typedef  struct tagUSB_SDK_BEEP_AND_FLICKER
{
	DWORD  dwSize;      //�ṹ���С
	BYTE    byBeepType;  //�������� 0��Ч��1������2������3������4ֹͣ
	BYTE byBeepCount; //���д�������ֻ��������������Ч���Ҳ���Ϊ0��
	BYTE byFlickerType;  //��˸���� 0��Ч��1������2����3��ȷ��4ֹͣ
	BYTE byFlickerCount; //��˸��������ֻ�Դ�����ȷ��Ч���Ҳ���Ϊ0��
	BYTE byRes[24];
}USB_SDK_BEEP_AND_FLICKER,*LPUSB_SDK_BEEP_AND_FLICKER;

typedef  struct  tagUSB_SDK_ACTIVATE_CARD_RES
{
	DWORD dwSize;
	BYTE byCardType;// �����ͣ�0-TypeA m1����1-TypeA cpu��,2-TypeB,3-125kHz Id����
	BYTE bySerialLen; //���������к��ֽڳ���
	BYTE  bySerial[10];//���������к�
	BYTE bySelectVerifyLen; //ѡ��ȷ�ϳ���
	BYTE  bySelectVerify[3]; //ѡ��ȷ��(3�ֽ�)
	BYTE  byRes[12];
}USB_SDK_ACTIVATE_CARD_RES,*LPUSB_SDK_ACTIVATE_CARD_RES;

typedef  struct  tagUSB_SDK_WAIT_SECOND
{
	DWORD dwSize;   //�ṹ���С
	BYTE byWait; // 1Byte�����ȴ�ʱ�䣨0-һֱִ��ֱ���п���Ӧ��������Ӧ1S��λ��
	BYTE byRes[27];
}USB_SDK_WAIT_SECOND,*LPUSB_SDK_WAIT_SECOND;



typedef struct TagSFZCard
{
	char   _name[64];//����
	char   _sex[16];//�Ա�
	char   _nation[16];//����
	char   _birth[32];//����
	char   _address[512];//��ַ
	char   _id[64];
	char   _agency[64];
	char   _expirestart[64];
	char   _expireend[64];
	char   _txzhm[64];
	char   _qfcs[64];
	char   _zjlxbs[64];
	//char   _FP[64];
}SFZCard, * LPSFZCard;





typedef void(CALLBACK *fEnumDeviceCallBack)(USB_SDK_DEVICE_INFO *pDevceInfo, void *pUser);
//[UnmanagedFunctionPointer(CallingConvention.StdCall)]
//public delegate void fEnumDeviceCallBack(ref tagUSB_SDK_DEVICE_INFO pDevceInfo, IntPtr pUser);


BOOL  __stdcall USB_SDK_GetDeviceConfig(LONG lUserID, DWORD dwCommand, LPUSB_CONFIG_INPUT_INFO pInputInfo,  LPUSB_CONFIG_OUTPUT_INFO pOutputInfo);
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_GetDeviceConfig(int lUserID, int dwCommand, ref tagUSB_CONFIG_INPUT_INFO input_info, ref tagUSB_CONFIG_OUTPUT_INFO pOutputInfo);


USB_SDK_API BOOL  __stdcall USB_SDK_Init();//USB_SDK�ĳ�ʼ��
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_Init();

USB_SDK_API BOOL  __stdcall USB_SDK_Cleanup();//USB_SDK�ķ���ʼ��
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_Cleanup();

USB_SDK_API BOOL  __stdcall USB_SDK_SetLogToFile(DWORD dwLogLevel, char const *strLogDir, BOOL bAutoDel);
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_SetLogToFile(int dwLogLevel, string strLogDir, bool bAutoDel);

USB_SDK_API DWORD __stdcall USB_SDK_GetLastError();//��ȡ������
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern int USB_SDK_GetLastError();

USB_SDK_API char* __stdcall USB_SDK_GetErrorMsg(DWORD dwErrorCode);//��ȡ�������Ӧ����Ϣ
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern string USB_SDK_GetErrorMsg(int dwErrorCode);

USB_SDK_API BOOL  __stdcall USB_SDK_EnumDevice(fEnumDeviceCallBack cbEnumDeviceCallBack, void* pUser);//ö��USB�豸
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_EnumDevice(fEnumDeviceCallBack cbEnumDeviceCallBack, IntPtr pUser);


USB_SDK_API LONG  __stdcall USB_SDK_Login(LPUSB_SDK_USER_LOGIN_INFO pUsbLoginInfo, LPUSB_SDK_DEVICE_REG_RES pDevRegRes);//��¼�豸
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern int USB_SDK_Login(ref tagUSB_SDK_USER_LOGIN_INFO pUsbLoginInfo, ref tagUSB_SDK_DEVICE_REG_RES pDevRegRes);

USB_SDK_API BOOL  __stdcall USB_SDK_Logout(LONG lUserID);//�ر�USB�豸
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_Logout(int lUserID);


USB_SDK_API BOOL  __stdcall USB_SDK_SetDeviceConfig(LONG lUserID,
DWORD dwCommand,
LPUSB_CONFIG_INPUT_INFO pConfigInputInfo,
LPUSB_CONFIG_OUTPUT_INFO pConfigOutputInfo);
/*[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
public static extern bool USB_SDK_SetDeviceConfig(int lUserID,
	int dwCommand,
	ref tagUSB_CONFIG_INPUT_INFO pConfigInputInfo,
	ref tagUSB_CONFIG_OUTPUT_INFO pConfigOutputInfo);*/

USB_SDK_API DWORD __stdcall USB_SDK_GetSDKVersion(void);
/*[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
public static extern int USB_SDK_GetSDKVersion();

[DllImport("Dewlt.dll", CallingConvention = CallingConvention.StdCall)]
public extern static int dewlt(string file_name);*/

USB_SDK_API  int  __stdcall  GetBmp(char* file_name, int intf);
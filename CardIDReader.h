#pragma once
#include "pch.h"


#define  USB_SDK_API   extern "C"

typedef  struct tagUSB_SDK_USER_LOGIN_INFO
{
	DWORD dwSize; //结构体大小
	DWORD dwTimeout; //登录超时时间（单位：毫秒）
	DWORD dwVID;  //设备VID，枚举设备时得到
	DWORD dwPID;  //设备PID，枚举设备时得到
	char szUserName[32];           //[MAX_USERNAME_LEN/*32*/]; //用户名
	char szPassword[16];          //[MAX_PASSWORD_LEN/*16*/]; //密码
	char szSerialNumber[48];       //[MAX_SERIAL_NUM_LEN/*48*/]; //设备序列号，枚举设备时得到
	BYTE byRes[80];
}USB_SDK_USER_LOGIN_INFO,*LPUSB_SDK_USER_LOGIN_INFO;



typedef  struct tagUSB_SDK_DEVICE_INFO
{
	DWORD dwSize;   //结构体大小
	DWORD dwVID;   //设备VID
	DWORD dwPID;   //设备PID
	char szManufacturer[32];  //[MAX_MANUFACTURE_LEN/*32*/];//制造商（来自描述符）
	char szDeviceName[32];  //[MAX_DEVICE_NAME_LEN/*32*/];//设备名称（来自描述符）
	char szSerialNumber[48];  //[MAX_SERIAL_NUM_LEN/*48*/];//设备序列号（来自描述符）
    BYTE  byRes[68];
}USB_SDK_DEVICE_INFO,*LPUSB_SDK_DEVICE_INFO;


typedef  struct tagUSB_SDK_DEVICE_REG_RES
{
	DWORD dwSize;   //结构体大小
	char szDeviceName[32];   //[MAX_DEVICE_NAME_LEN /*32*/]; //设备名称
	char szSerialNumber[48];  //[MAX_SERIAL_NUM_LEN /*48*/]; //设备序列号
	DWORD dwSoftwareVersion; //软件版本号,高16位是主版本,低16位是次版本
    BYTE byRes[40];
}USB_SDK_DEVICE_REG_RES,*LPUSB_SDK_DEVICE_REG_RES;


typedef  struct tagUSB_SDK_CERTIFICATE_INFO
{
	DWORD dwSize; //结构体大小
	WORD wWordInfoSize; //文字信息长度
	WORD wPicInfoSize; //相片信息长度
	WORD wFingerPrintInfoSize; //指纹信息长度
	BYTE  byRes2[2];
	BYTE  byWordInfo[256];   //[WORD_LEN/*256*/]; //文字信息
	BYTE  byPicInfo[1024];   //[PIC_LEN/*1024*/]; //相片信息
	BYTE  byFingerPrintInfo[1024];    //[FINGER_PRINT_LEN/*1024*/]; //指纹信息
	BYTE  byRes[40];
}USB_SDK_CERTIFICATE_INFO,*LPUSB_SDK_CERTIFICATE_INFO;


typedef struct tagUSB_CONFIG_INPUT_INFO
{
	LPDWORD lpCondBuffer;        //指向条件缓冲区
	DWORD dwCondBufferSize;//条件缓冲区大小
	LPDWORD lpInBuffer;          //指向输出缓冲区
	DWORD dwInBufferSize;   //输入缓冲区大小
	BYTE byRes[48];
}USB_CONFIG_INPUT_INFO,*LPUSB_CONFIG_INPUT_INFO;

typedef  struct tagUSB_CONFIG_OUTPUT_INFO
{
	LPDWORD lpOutBuffer;      //指向输出缓冲区
	DWORD dwOutBufferSize;  //输出缓冲区大小
	BYTE byRes[56];
}USB_CONFIG_OUTPUT_INFO,*LPUSB_CONFIG_OUTPUT_INFO;

typedef  struct tagUSB_SDK_BEEP_AND_FLICKER
{
	DWORD  dwSize;      //结构体大小
	BYTE    byBeepType;  //蜂鸣类型 0无效，1连续，2慢鸣，3快鸣，4停止
	BYTE byBeepCount; //鸣叫次数，（只对慢鸣、快鸣有效，且不能为0）
	BYTE byFlickerType;  //闪烁类型 0无效，1连续，2错误，3正确，4停止
	BYTE byFlickerCount; //闪烁次数，（只对错误、正确有效，且不能为0）
	BYTE byRes[24];
}USB_SDK_BEEP_AND_FLICKER,*LPUSB_SDK_BEEP_AND_FLICKER;

typedef  struct  tagUSB_SDK_ACTIVATE_CARD_RES
{
	DWORD dwSize;
	BYTE byCardType;// 卡类型（0-TypeA m1卡，1-TypeA cpu卡,2-TypeB,3-125kHz Id卡）
	BYTE bySerialLen; //卡物理序列号字节长度
	BYTE  bySerial[10];//卡物理序列号
	BYTE bySelectVerifyLen; //选择确认长度
	BYTE  bySelectVerify[3]; //选择确认(3字节)
	BYTE  byRes[12];
}USB_SDK_ACTIVATE_CARD_RES,*LPUSB_SDK_ACTIVATE_CARD_RES;

typedef  struct  tagUSB_SDK_WAIT_SECOND
{
	DWORD dwSize;   //结构体大小
	BYTE byWait; // 1Byte操作等待时间（0-一直执行直到有卡响应，其他对应1S单位）
	BYTE byRes[27];
}USB_SDK_WAIT_SECOND,*LPUSB_SDK_WAIT_SECOND;



typedef struct TagSFZCard
{
	char   _name[64];//姓名
	char   _sex[16];//性别
	char   _nation[16];//民族
	char   _birth[32];//生日
	char   _address[512];//地址
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


USB_SDK_API BOOL  __stdcall USB_SDK_Init();//USB_SDK的初始化
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_Init();

USB_SDK_API BOOL  __stdcall USB_SDK_Cleanup();//USB_SDK的反初始化
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_Cleanup();

USB_SDK_API BOOL  __stdcall USB_SDK_SetLogToFile(DWORD dwLogLevel, char const *strLogDir, BOOL bAutoDel);
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_SetLogToFile(int dwLogLevel, string strLogDir, bool bAutoDel);

USB_SDK_API DWORD __stdcall USB_SDK_GetLastError();//获取错误码
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern int USB_SDK_GetLastError();

USB_SDK_API char* __stdcall USB_SDK_GetErrorMsg(DWORD dwErrorCode);//获取错误码对应的信息
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern string USB_SDK_GetErrorMsg(int dwErrorCode);

USB_SDK_API BOOL  __stdcall USB_SDK_EnumDevice(fEnumDeviceCallBack cbEnumDeviceCallBack, void* pUser);//枚举USB设备
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern bool USB_SDK_EnumDevice(fEnumDeviceCallBack cbEnumDeviceCallBack, IntPtr pUser);


USB_SDK_API LONG  __stdcall USB_SDK_Login(LPUSB_SDK_USER_LOGIN_INFO pUsbLoginInfo, LPUSB_SDK_DEVICE_REG_RES pDevRegRes);//登录设备
//[DllImport("HCUsbSDK.dll", CallingConvention = CallingConvention.StdCall)]
//public static extern int USB_SDK_Login(ref tagUSB_SDK_USER_LOGIN_INFO pUsbLoginInfo, ref tagUSB_SDK_DEVICE_REG_RES pDevRegRes);

USB_SDK_API BOOL  __stdcall USB_SDK_Logout(LONG lUserID);//关闭USB设备
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
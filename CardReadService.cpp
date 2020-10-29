// CardReadService.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "CardReadService.h"
#include  "CIdCardReaderManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include"iphlpapi.h"
#pragma comment(lib, "Iphlpapi.lib")

#include <shlwapi.h>        // API StrCmpIA 头文件
#pragma comment(lib, "shlwapi.lib")

#include <Strsafe.h>        // API StringCbPrintfA 头文件
#include <shellapi.h>       // API lstrcpyA 头文件


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//为了方便把启动服务的放到一个函数里===并无其他特殊用途2017-10-12
void StartMainEntrance(bool bIsService = false);
/*********************************************************服务框架相关_Begin***********************************************************************/
#include <winsvc.h>//////////////////////////////////服务要包含的头文件////////////////////////////////////////////////////////////////////////////////
//定义全局函数变量
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);

TCHAR szServiceName[] = _T("BT_ReadIdCard_Service");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;
HANDLE g_hExitEvent = nullptr; //控制退出的时候用
CIdCardReaderManager* g_pService = nullptr;


//*********************************************************
//Functiopn:			Init
//Description:			初始化
//Calls:				main
//Called By:				
//Table Accessed:				
//Table Updated:				
//Input:				
//Output:				
//Return:				
//Others:				
//History:				
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void Init()
{
	hServiceStatus = nullptr;
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState = SERVICE_STOPPED;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
}

//*********************************************************
//Functiopn:			ServiceMain
//Description:			服务主函数，这在里进行控制对服务控制的注册
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI ServiceMain()
{
	// Register the control request handler
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	//注册服务控制
	hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
	if (hServiceStatus == nullptr)
	{
		LogEvent(_T("Handler not installed"));
		return;
	}
	SetServiceStatus(hServiceStatus, &status);
	//更改服务运行的状态
	status.dwWin32ExitCode = S_OK;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);
	//启动主要的服务程序在这个地方完成，并且要在这个永久等待......除非按照我们的设定往下执行
	StartMainEntrance(true);

	WaitForSingleObject(g_hExitEvent, INFINITE);
	//此处添加停止业务
	if (g_pService)
	{
		g_pService->Stop(); //这个地方要start退出才能去继续进行
	}

	delete g_pService;
	g_pService = nullptr;
	if (g_hExitEvent)
	{
		CloseHandle(g_hExitEvent);
	}
	status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &status);
	LogEvent(_T("Service stopped"));
}

//*********************************************************
//Functiopn:			ServiceStrl
//Description:			服务控制主函数，这里实现对服务的控制，
//						当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:				dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		status.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hServiceStatus, &status);
		//让线程有信号
		if (g_hExitEvent) SetEvent(g_hExitEvent);

		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		LogEvent(_T("Bad service request"));
	}
}

//*********************************************************
//Functiopn:			IsInstalled
//Description:			判断服务是否已经被安装
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL IsInstalled()
{
	BOOL bResult = FALSE;

	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

	if (hSCM != nullptr)
	{
		//打开服务
		SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
		if (hService != nullptr)
		{
			bResult = TRUE;
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hSCM);
	}
	return bResult;
}

//*********************************************************
//Functiopn:			Install
//Description:			安装服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL Install()
{
	if (IsInstalled())
		return TRUE;

	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	if (hSCM == nullptr)
	{
		MessageBox(nullptr, _T("Couldn't open service manager"), szServiceName, MB_OK);
		return FALSE;
	}

	// Get the executable file path
	TCHAR szFilePath[MAX_PATH];
	::GetModuleFileName(nullptr, szFilePath, MAX_PATH);

	//创建服务
	SC_HANDLE hService = ::CreateService(
		hSCM, szServiceName, szServiceName,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		szFilePath, nullptr, nullptr, _T(""), nullptr, nullptr);

	if (hService == nullptr)
	{
		CloseServiceHandle(hSCM);
		MessageBox(nullptr, _T("Couldn't create service"), szServiceName, MB_OK);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return TRUE;
}

//*********************************************************
//Functiopn:			Uninstall
//Description:			删除服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
BOOL Uninstall()
{
	if (!IsInstalled())
		return TRUE;

	SC_HANDLE hSCM = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

	if (hSCM == nullptr)
	{
		MessageBox(nullptr, _T("Couldn't open service manager"), szServiceName, MB_OK);
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);

	if (hService == nullptr)
	{
		CloseServiceHandle(hSCM);
		MessageBox(nullptr, _T("Couldn't open service"), szServiceName, MB_OK);
		return FALSE;
	}
	SERVICE_STATUS status;
	ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//删除服务
	BOOL bDelete = DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	if (bDelete)
		return TRUE;

	LogEvent(_T("Service could not be deleted"));
	return FALSE;
}

//*********************************************************
//Functiopn:			LogEvent
//Description:			记录服务事件
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//<author>niying <time>2006-8-10		<version>		<desc>
//*********************************************************
void LogEvent(LPCTSTR pFormat, ...)
{
	TCHAR chMsg[512];
	HANDLE hEventSource;
	LPTSTR lpszStrings[1];
	va_list pArg;

	va_start(pArg, pFormat);
	_vstprintf_s(chMsg, pFormat, pArg);
	va_end(pArg);

	lpszStrings[0] = chMsg;

	hEventSource = RegisterEventSource(nullptr, szServiceName);
	if (hEventSource != nullptr)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, nullptr, 1, 0, (LPCTSTR*)&lpszStrings[0], nullptr);
		DeregisterEventSource(hEventSource);
	}
}

/***************************************************服务框架相关_END******************************************************************************/
void StartMainEntrance(bool bIsService)
{
	if (g_pService == nullptr)
	{
		g_pService = new CIdCardReaderManager();
	}
	if (g_pService)
	{
		//事件-----------一定要放在这个地方因为检测线程会在构造函数中运行
		g_hExitEvent = nullptr;
		g_hExitEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		//模拟服务的运行，10后自动退出。应用时将主要任务放于此即可
		LogEvent(_T("Begining Start BT_ReadIdCard_Service!"));

		//开启
		g_pService->Start();
		if (!bIsService)
		{
			WaitForSingleObject(g_hExitEvent, INFINITE);
		}
	}
}

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: 在此处为应用程序的行为编写代码。
			wprintf(L"错误: MFC 初始化失败\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
			Init();
			dwThreadID = GetCurrentThreadId();
			SERVICE_TABLE_ENTRY st[] =
			{
				{szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
				{nullptr, nullptr}
			};
			/*////////////启动服务--用----开始////////////////////////////////////////////////////////////////////////
			if (!::StartServiceCtrlDispatcher(st))
			{
				//LogEvent(_T("Register Service Main Function Error!"));
				CString strLogMsg = _T("开启服务失败！");
				CLogHelper::WriteLog(strLogMsg);
			}
			//主线程退出标志
		   //LogEvent(_T("HKC_ParkingSet_Service Main exit!"));
			CString strLogMsg = _T("服务退出！");
			CLogHelper::WriteLog(strLogMsg);
			*//////////////启动服务-用-----结束/////////////////////////////////////////////////////////////////////////

			//////////////测试用-控制台运行--开始///////////////////////////////////////////////////////////////////////
			StartMainEntrance();
			///////////////测试用--控制台运行---结束/////////////////////////////////////////////////////////////////////////////////////
		}
	}
	else
	{
		// TODO: 更改错误代码以符合需要
		wprintf(L"错误: GetModuleHandle 失败\n");
		nRetCode = 1;
	}

	return nRetCode;
}

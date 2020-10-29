#include "pch.h"
#include "CLogHelper.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale>

#define LOG_FILE_NAME     _T("Log.txt")

CLogHelper::CLogHelper(void)
{
}

CLogHelper::~CLogHelper(void)
{
}

void CLogHelper::WriteLog(LPCTSTR lpszLog)
{
	// 获取日志文件路径
	static CString strLogFilePath = _T("");
	if (strLogFilePath.IsEmpty())
	{
		strLogFilePath = MakeFilePath();
	}
	// 判断日志文件是否存在，不存在则创建
	wchar_t* pwchLogFilePath = strLogFilePath.AllocSysString();
	errno_t err = 0;
	if ((err = _taccess_s(pwchLogFilePath, 0)) != 0)
	{
		CStdioFile file;
		if (file.Open(strLogFilePath, CStdioFile::modeCreate))
		{
			file.Close();
		}
	}
	// 向日志文件写入日志
	CStdioFile file;
	if (file.Open((LPCTSTR)strLogFilePath, CStdioFile::modeWrite | CStdioFile::shareDenyNone))
	{
		CString strMsg = MakeLogMsg(lpszLog);
		file.SeekToEnd();
		char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, "chs");// 设定区域
		file.WriteString(strMsg);
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);// 还原区域设定
		file.Close();
	}

}

CString CLogHelper::MakeLogMsg(LPCTSTR lpszLog)
{
	CTime time = CTime::GetCurrentTime();
	CString strMsg = time.Format("[%Y-%m-%d %H:%M:%S] ");
	strMsg = strMsg + lpszLog + _T("\r\n");
	return strMsg;
}

CString CLogHelper::MakeFilePath()
{
	// 获取当前进程路径
	TCHAR szFilePath[MAX_PATH];
	memset(szFilePath, 0, MAX_PATH);
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;// 删除文件名，只获得路径字符串
	CString strFilePath = szFilePath;
	strFilePath = strFilePath + LOG_FILE_NAME;

	return strFilePath;
}
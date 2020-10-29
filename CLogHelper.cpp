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
	// ��ȡ��־�ļ�·��
	static CString strLogFilePath = _T("");
	if (strLogFilePath.IsEmpty())
	{
		strLogFilePath = MakeFilePath();
	}
	// �ж���־�ļ��Ƿ���ڣ��������򴴽�
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
	// ����־�ļ�д����־
	CStdioFile file;
	if (file.Open((LPCTSTR)strLogFilePath, CStdioFile::modeWrite | CStdioFile::shareDenyNone))
	{
		CString strMsg = MakeLogMsg(lpszLog);
		file.SeekToEnd();
		char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, "chs");// �趨����
		file.WriteString(strMsg);
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);// ��ԭ�����趨
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
	// ��ȡ��ǰ����·��
	TCHAR szFilePath[MAX_PATH];
	memset(szFilePath, 0, MAX_PATH);
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;// ɾ���ļ�����ֻ���·���ַ���
	CString strFilePath = szFilePath;
	strFilePath = strFilePath + LOG_FILE_NAME;

	return strFilePath;
}
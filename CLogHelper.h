#pragma once
// CLogHelper.h : header file for log information
 
class CLogHelper
{
public:
	CLogHelper(void);
	~CLogHelper(void);
	static void WriteLog(LPCTSTR lpszLog);

private:
	static CString MakeFilePath();
	static CString MakeLogMsg(LPCTSTR lpszLog);
};
// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"

#include <iostream>
#include <afxmt.h>  //CCriticalSection的头文件
#include <string>
#include <locale.h>
#include "conio.h"


using namespace std;


// TODO:  在此处引用程序需要的其他头文件



// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}
//释放数组指针宏
#define RELEASE_ARRAY(x)                      {if(x != NULL ){delete[] x;x=NULL;}}





// _beginThreadex 代替CreateThread的宏
typedef unsigned(__stdcall* PTHREAD_START) (void*);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadID) \
((HANDLE) _beginthreadex( \
(void *) (psa), \
(unsigned) (cbStack), \
(PTHREAD_START) (pfnStartAddr),\
(void *) (pvParam),\
(unsigned) (fdwCreate), \
(unsigned *) (pdwThreadID)))

#endif //PCH_H

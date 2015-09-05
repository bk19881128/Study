#ifndef NEONETWORK 

#define NEONETWORK

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>

#ifdef WIN32
    #include<stdio.h>
    #include<conio.h>
    #include<Windows.h>
    #include<process.h>
    #include<WinSock.h>
#else
    #include<unistd.h>
    #include<errno.h>
    #include<pthread.h>
    #include<fcntl.h>
    #include<netinet/in.h>
    #include<string.h>
    #include<sys/time.h>
    #include<arpa/inet.h>
    #include<termios.h>
    #include<netdb.h>
    #include<getopt.h>
    #include<time.h>
#endif

#ifdef NEO_DEBUG
/*断言定义*/
#define ASSERT(exp)   \
	if(!(exp))        \
	{                 \
	    printf("%s,file:%s,line:%d: %s\r\n",exp,_FILE_,_LINE_,_FUNCTION_); \
    }                                                                       \

#else

#define ASSERT(exp)

#endif

//锁定义
#ifdef WIN32

#define MUTEX CRITICAL_SECTION
#define MUTEXINIT(m) InitializeCriticalSection(m)
#define MUTEXLOCK(m) EnterCriticalSection(m)
#define MUTEXUNLOCK(m) LeaveCriticalSection(m)
#define MUTEXDESTROY(m) DeleteCriticalSection(m)

#else
#define MUTEX pthread_mutex_t
#define MUTEXINIT(m) pthread_mutex_init(m,NULL)
#define MUTEXLOCK(m) pthread_mutex_lock(m)
#define MUTEXUNLOCK(m) pthread_mutex_unlock(m)
#define MUTEXDESTROY(m) pthread_mutex_destroy(m)

#endif


//获取开机的毫秒数
#ifdef WIN32

#else

inline unsigned long GetTickCount(void)
{
	unsigned long IRet=0;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	IRet=tv.tv_usec/1000;
	return IRet;
}

#endif

#ifdef WIN32
//#pragma once
//#include <process.h>

#define THREAD HANDLE
#define THREADID unsigned
#define THREADCREATE(func,args,thread,id)\
	thread=(HANDLE)_beginthreadex(NULL,0,func,(PVOID)args,0,&id);
#define THREADCREATE_ERROR NULL
//线程函数标准构型
#define THREADFUNC(func,args) unsigned _stdcall func(PVOID args)
//检测开机毫秒数
#define _GetTimeOfDay GetTickCount
#define MIN_SLEEP 10

#else

#define THREAD pthread_t
#define THREADID unsigned 
#define THREADCREATE(func,args,thread,id)\
	pthread_create(&thread,NULL,func,args);
#define THREADCREATE_ERROR -1
#define THREADFUNC(func,args) void *func(void *args)
#define Sleep(ms) usleep(ms*1000)

unsigned long GetTickCount(void);
//#include <sys/utime.h>
#define _GetTimeOfDay GetTickCount
#define MIN_SLEEP 1
#endif
//套接字定义
#ifdef WIN32
//#pragma once
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
//#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define WIN_LINUX_SOCKET SOCKET
#define WIN_LINUX_CloseSocket closesocket
#define WIN_LINUX_F_INET AF_INET
#define WIN_LINUX_InvalidSocket INVALID_SOCKET
#define WIN_LINUX_SocketError SOCKET_ERROR
//setsockopt第四个变量定义
#define WIN_LINUX_SetSockOptArg4UseType const char
#define WIN_LINUX_GetSockOptArg4UseType char
#define WIN_LINUX_SendRecvLastArg 0

#define WIN_LINUX_vsnprintf _vsnprintf


#else

#define WIN_LINUX_SOCKET int
#define WIN_LINUX_CloseSocket close
#define WIN_LINUX_F_INET AF_INET
#define WIN_LINUX_InvalidSocket -1
#define WIN_LINUX_SocketError -1
#define WIN_LINUX_SetSockOptArg4UseType void
#define WIN_LINUX_GetSockOptArg4UseType void
#define WIN_LINUX_SendRecvLastArg MSG_NOSIGNAL
#define WIN_LINUX_vsnprintf vsnprintf
#endif



#endif

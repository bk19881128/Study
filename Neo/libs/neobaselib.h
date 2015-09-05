#ifndef NEOBASELIB 

#define NEOBASELIB 

namespace NEOLIB {
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
//屏幕输出回调函数原型
typedef void (*_BASE_LIBRARY_PRINT_INFO_CALLBACK)(void *pCallParam);
//应用程序输出回调函数
typedef void (*_APP_INFO_OUT_CALLBACK)(char *szInfo,void *pCallParam);

class CNEOLog;
class CNEOMemPoolWithLock;
class CNEOTaskPool;
class CNEOTaskRun;
class CNEOLowDebug;

class  CNEOBaseLibrary
{
private:
	static CNEOBaseLibrary* m_Instance;
    CNEOBaseLibrary(const char *szAppName,
        const char *szLogPath,
        const char *szTempPath,
         _BASE_LIBRARY_PRINT_INFO_CALLBACK pPrintInfoCallback, //info输出回调函数指针
        int nTaskPoolThreadMax=DEFAULT_THREAD_MAX, //任务池最大的线程数
        bool bDebugToTTYFlag=true,                 //debug输出到屏幕开关
        void* pPrintInfoCallbackParam=NULL,       //info回调函数指针
        _APP_INFO_OUT_CALLBACK pInfoOutCallback=NULL,// 应用程序回调函数
        void *pInfoOutCallbackParam=NULL
        );

public:
    ~CNEOBaseLibrary();
private:
    //info打印任务
    static bool InfoPrintTaskCallback(void *pCallParam,int &nStatus);
    time_t m_tLastPrint;
    //打印信息回调函数
    _BASE_LIBRARY_PRINT_INFO_CALLBACK m_pPrintInfoCallback;
    void *m_pInfoOutCallbackParam;  
public:
    //应用名的备份保存
    char m_szAppName[NEO_APPLICATION_NAME_SIZE];
    //日志路径
    char m_szLogPathName[NEO_APP_LOG_PATH_NAME_SIZE];
    //临时文件路径
    char m_szTempPathName[NEO_APP_TEMP_PATH_NAME_SIZE];
    //日志模块
    CNEOLog *m_pLog;
    //内存池
    CNEOMemPoolWithLock *m_pMemPool;
    //线程池
    CNEOTaskPool *m_pTaskPool;
    //线程池运行体
    CNEOTaskRun *m_pTaskRun;
    //debug没运行一次，覆盖上次
    CNEOLowDebug *m_pDebug;
	//安全锁
	static CNEOBaseLibrary *getInstance(const char *szAppName,
								const char *szLogPath,
								const char *szTempPath,
								 _BASE_LIBRARY_PRINT_INFO_CALLBACK pPrintInfoCallback, //info输出回调函数指针
								int nTaskPoolThreadMax=DEFAULT_THREAD_MAX, //任务池最大的线程数
								bool bDebugToTTYFlag=true,                 //debug输出到屏幕开关
								void* pPrintInfoCallbackParam=NULL,       //info回调函数指针
								_APP_INFO_OUT_CALLBACK pInfoOutCallback=NULL,// 应用程序回调函数
								void *pInfoOutCallbackParam=NULL);

#ifdef WIN32
       bool m_bSocketInitFlag;
       WORD wVersionRequested;
       WSADATA m_wsaData;

#else
#endif
};


}
#endif


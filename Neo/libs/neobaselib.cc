

#include "neoindex.h" 
#include "neolock.h"     
#include "neodebug.h"          
#include "neosafefunc.h"                  
#include "neomemmanager.h"                       
#include "neoqueue.h"                              
#include "neothread.h"                                      
#include "neolog.h"
#include "neobaselib.h"

namespace NEOLIB {

CMutexLock m_Lock;
CNEOBaseLibrary* CNEOBaseLibrary::m_Instance = NULL;
/////////////////////////////////////////////////////////////////////
CNEOBaseLibrary::CNEOBaseLibrary(const char *szAppName,
    const char *szLogPath,
    const char *szTempPath,
    _BASE_LIBRARY_PRINT_INFO_CALLBACK pPrintInfoCallback, //info输出回调函数指针
    int nTaskPoolThreadMax, //任务池最大的线程数
    bool bDebugToTTYFlag,                 //debug输出到屏幕开关
    void* pPrintInfoCallbackParam,       //info回调函数指针
    _APP_INFO_OUT_CALLBACK pInfoOutCallback,// 应用程序回调函数
    void *pInfoOutCallbackParam
    ):m_pPrintInfoCallback(pPrintInfoCallback),m_pInfoOutCallbackParam(pInfoOutCallbackParam)
{
    m_pDebug=NULL;
    m_pTaskPool=NULL;
    m_pMemPool=NULL;
    m_pLog=NULL;
    SafeStrcpy(m_szAppName,szAppName,NEO_APPLICATION_NAME_SIZE);
    SafeStrcpy(m_szLogPathName,szLogPath,NEO_APP_LOG_PATH_NAME_SIZE);
    SafeStrcpy(m_szTempPathName,szTempPath,NEO_APP_TEMP_PATH_NAME_SIZE);
    //保存信息打印回调函数相关指针
    srand((unsigned int)time(NULL));
    //初始化debug 对象
    m_pDebug=new CNEOLowDebug(m_szLogPathName,
        m_szAppName,
        pInfoOutCallback,
        pInfoOutCallbackParam,
        bDebugToTTYFlag);
    if(!m_pDebug)
    {
       return;
    }
    m_pDebug->DebugToFile("_____________________\n");
    m_pDebug->DebugToFile("Hello World\n");
#ifdef WIN32
    {
       m_bSocketInitFlag=false;
       int err;
       wVersionRequested=MAKEWORD(2,2);
       err=WSAStartup(wVersionRequested,&m_wsaData);
       if(err!=0)
       {
          m_pDebug->DebugToFile("Socket Init fail!\n");
       }
       else
       {
          m_bSocketInitFlag=true;
       }
    }
#else
#endif
    
    //初始化内存池
    m_pMemPool=new CNEOMemPoolWithLock(m_pDebug);
    if(!m_pMemPool)
    {
       m_pDebug->DebugToFile("CNEOBaseLibrary():m_pMemPool new fail\n");
       return;
    }
    
    //初始化日志系统
    m_pLog=new CNEOLog(m_pDebug,m_pMemPool,m_szLogPathName,m_szAppName);
    if(!m_pLog)
    {
        m_pDebug->DebugToFile("CNEOBaseLibrary():m_pLog new fail\n");
        return;
    }
    m_pMemPool->Register(m_pLog,"CNEOBaseLibrary::m_pLog");

    m_pTaskPool=new CNEOTaskPool(m_pDebug,m_pMemPool,m_pLog,nTaskPoolThreadMax);
    if(!m_pTaskPool)
    {
        m_pDebug->DebugToFile("CNEOBaseLibrary():m_pTaskPool new fail\n");
        return;
    }
    //此时可以利用内存池的注册机制
    m_pMemPool->Register(m_pTaskPool,"CNEOBaseLibrary::m_pTaskPool");

    m_pTaskRun=new CNEOTaskRun(m_pMemPool,m_pLog,m_pTaskPool);
    if(!m_pTaskRun)
    {
        m_pDebug->DebugToFile("CNEOBaseLibrary():m_pTaskRun new fail\n");
        return;
    }
    m_pMemPool->Register(m_pTaskRun,"CNEOBaseLibrary::m_pTaskRun");

    //启动线程打印信息任务
    TimeSetNow(m_tLastPrint);                 //计时因子/////////////////////////////////???????
    if(!m_pTaskRun->StartTask(InfoPrintTaskCallback,this))
    {
       m_pLog->_XGSysLog("CNEOBaseLibrary::start print info task fail\n");
    }
    m_pDebug->DebugToFile(">>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}
CNEOBaseLibrary::~CNEOBaseLibrary()
{
    m_pDebug->DebugToFile("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    NEO_PRINTF("1\n");
    m_pMemPool->SetCloseFlag();
    NEO_PRINTF("2\n");
    NEO_PRINTF("3\n");
    if(m_pTaskRun)
    {
       m_pMemPool->UnRegister(m_pTaskRun);
       delete m_pTaskRun;
       m_pTaskRun=NULL;
    }
    NEO_PRINTF("4\n");
    NEO_PRINTF("5\n");
    if(m_pTaskPool)
    {
       m_pMemPool->UnRegister(m_pTaskPool);
       delete m_pTaskPool;
       m_pTaskPool=NULL;
    }
    NEO_PRINTF("6\n");
    if(m_pLog)
    {
       m_pMemPool->UnRegister(m_pLog);
       delete m_pLog;
       m_pLog=NULL;
    }
    NEO_PRINTF("7\n");
    if(m_pMemPool)
    {
       delete m_pMemPool;
       m_pMemPool=NULL;
    }
    NEO_PRINTF("8\n");
    m_pDebug->DebugToFile("Bye World!\n");
    m_pDebug->DebugToFile("_____________________\n");
    if(m_pDebug)
    {
       delete m_pDebug;
       m_pDebug=NULL;
    }
    NEO_PRINTF("9\n");
#ifdef WIN32
    {
       if(m_bSocketInitFlag)
       {
          if(LOBYTE(&m_wsaData,wVersionRequested)!=2 || HIBYTE(&m_wsaData,wVersionRequested)!=2)
          {
             WSACleanup();
          }
          m_bSocketInitFlag=false;
       }
    }
#else
#endif
}
//#define MAIN_LOOP_DELAY 2
bool CNEOBaseLibrary::InfoPrintTaskCallback(void *pCallParam,int &nStatus)
{
    CNEOBaseLibrary *pThis=(CNEOBaseLibrary*)pCallParam;
    if(TimelsUp(pThis->m_tLastPrint,MAIN_LOOP_DELAY))
    {
       TimeSetNow(pThis->m_tLastPrint);
       NEO_PRINTF("*************************\n");
       pThis->m_pTaskPool->PrintInfo();      // 打印任务池信息
       pThis->m_pTaskRun->PrintInfo();
       pThis->m_pMemPool->PrintInfo();
       if(pThis->m_pPrintInfoCallback)
       {
          pThis->m_pPrintInfoCallback(pThis->m_pInfoOutCallbackParam);
       }
       NEO_PRINTF("*************************\n");
       NEO_PRINTF("\n");
    }
    return false;
}

CNEOBaseLibrary *CNEOBaseLibrary::getInstance(const char *szAppName,
    const char *szLogPath,
    const char *szTempPath,
    _BASE_LIBRARY_PRINT_INFO_CALLBACK pPrintInfoCallback, //info输出回调函数指针
    int nTaskPoolThreadMax, //任务池最大的线程数
    bool bDebugToTTYFlag,                 //debug输出到屏幕开关
    void* pPrintInfoCallbackParam,       //info回调函数指针
    _APP_INFO_OUT_CALLBACK pInfoOutCallback,// 应用程序回调函数
    void *pInfoOutCallbackParam)
{
	if(m_Instance==NULL)
	{
		m_Lock.Lock();
		{
			if(m_Instance==NULL)
			{
				m_Instance = new CNEOBaseLibrary(szAppName,szLogPath,szTempPath,pPrintInfoCallback,nTaskPoolThreadMax, \
					bDebugToTTYFlag,pPrintInfoCallbackParam,pInfoOutCallback,pInfoOutCallbackParam);
			}
		}
		m_Lock.UnLock();
	}
	return m_Instance;
}

}
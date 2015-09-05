#ifndef NEOBASELIB 

#define NEOBASELIB 

namespace NEOLIB {
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// 
//��Ļ����ص�����ԭ��
typedef void (*_BASE_LIBRARY_PRINT_INFO_CALLBACK)(void *pCallParam);
//Ӧ�ó�������ص�����
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
         _BASE_LIBRARY_PRINT_INFO_CALLBACK pPrintInfoCallback, //info����ص�����ָ��
        int nTaskPoolThreadMax=DEFAULT_THREAD_MAX, //����������߳���
        bool bDebugToTTYFlag=true,                 //debug�������Ļ����
        void* pPrintInfoCallbackParam=NULL,       //info�ص�����ָ��
        _APP_INFO_OUT_CALLBACK pInfoOutCallback=NULL,// Ӧ�ó���ص�����
        void *pInfoOutCallbackParam=NULL
        );

public:
    ~CNEOBaseLibrary();
private:
    //info��ӡ����
    static bool InfoPrintTaskCallback(void *pCallParam,int &nStatus);
    time_t m_tLastPrint;
    //��ӡ��Ϣ�ص�����
    _BASE_LIBRARY_PRINT_INFO_CALLBACK m_pPrintInfoCallback;
    void *m_pInfoOutCallbackParam;  
public:
    //Ӧ�����ı��ݱ���
    char m_szAppName[NEO_APPLICATION_NAME_SIZE];
    //��־·��
    char m_szLogPathName[NEO_APP_LOG_PATH_NAME_SIZE];
    //��ʱ�ļ�·��
    char m_szTempPathName[NEO_APP_TEMP_PATH_NAME_SIZE];
    //��־ģ��
    CNEOLog *m_pLog;
    //�ڴ��
    CNEOMemPoolWithLock *m_pMemPool;
    //�̳߳�
    CNEOTaskPool *m_pTaskPool;
    //�̳߳�������
    CNEOTaskRun *m_pTaskRun;
    //debugû����һ�Σ������ϴ�
    CNEOLowDebug *m_pDebug;
	//��ȫ��
	static CNEOBaseLibrary *getInstance(const char *szAppName,
								const char *szLogPath,
								const char *szTempPath,
								 _BASE_LIBRARY_PRINT_INFO_CALLBACK pPrintInfoCallback, //info����ص�����ָ��
								int nTaskPoolThreadMax=DEFAULT_THREAD_MAX, //����������߳���
								bool bDebugToTTYFlag=true,                 //debug�������Ļ����
								void* pPrintInfoCallbackParam=NULL,       //info�ص�����ָ��
								_APP_INFO_OUT_CALLBACK pInfoOutCallback=NULL,// Ӧ�ó���ص�����
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


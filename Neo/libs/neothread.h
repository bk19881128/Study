#ifndef NEOTHREAD

#define NEOTHREAD
namespace NEOLIB {
class CNEOLog;
class  CNEOThreadPool;
//class CNEOBaseLibrary;
class CNEOTaskRun;
class CNEOMemPoolWithLock;
class CThreadManager;
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//�̳߳ػص�����
//ÿ���ص�������һ������Ȩ
//���н������̲߳��˳����������״̬
typedef void(*_TPOOL_CALLBACK)(void *pCallParam,MBOOL &bThreadContinue);

//�̳߳ع������ݽṹ
typedef struct _THREAD_TOKEN_
{
	int m_nExitCode;             //���߳��������̳߳صı��
	MINT m_nState;                //�߳�״̬��MIINT
	THREAD m_hThread;             //�߳̾��
	THREADID m_nThreadID;         //�߳�ID
	_TPOOL_CALLBACK m_pCallback; //�ص�����
	void* m_pCallParam;          //�ص���������
	CNEOThreadPool *m_pThreadPoolObjext;//ָ���̳߳ض����ָ��
}SThreadToken;
const unsigned long SThreadTokenSize=sizeof(SThreadToken);//�ṹ�峤��
//////////////////////////////////////////////////////////////
//�ඨ��
class  CNEOThreadPool
{
public:
	CNEOThreadPool(CNEOLowDebug *pDebug);
	~CNEOThreadPool();
private:
	SThreadToken m_TToken[THIS_POOLTHREAD_MAX];//�̳߳����������̬����
	MBOOL m_bThreadContinue;                   //����thread�����ı�־
	MINT m_nThreadPoolThreadCount;             //thread������
	MINT m_nThreadPoolIdleThreadCount;         //���е��߳�����
	MUTEX m_RegisterLock;                      //�߳�ע���ٽ���
	CNEOLowDebug *m_pDebug;                    //debug�Ķ���ָ��
private:
	//������ʵ�Ĳ���ϵͳ�̺߳���
	static THREADFUNC(ThreadPoolThread,pParam);     //�̳߳ط����߳�
	//static unsigned WINAPI ThreadPoolThread(LPVOID pParam);
	static THREADFUNC(ThreadPoolCtrlThread,pParam); //�̳߳ع����߳�
	//static unsigned WINAPI ThreadPoolCtrlThread(LPVOID pParam);
private:
	//����û��ʹ�õ�Token
	int SearchForNotUseToken(void);
	//���һ�������߳�
	int GetAIdleThread(void);
	//��ɾ���ע�ᶯ��
	int ThreadPoolRegisterANewThread(_TPOOL_CALLBACK pCallback,void *pParam);
	int ThreadPoolRegisterANewThreadWhile(_TPOOL_CALLBACK pCallback,void *pParam);
public:
	//ע��һ�����߳�
	int ThreadPoolRegTask(_TPOOL_CALLBACK pCallback,void *pParam,bool bWaitForSuccess=true/*�Ƿ�ȴ�ע��ɹ�*/);
	bool TPALLThreadIsIdle(void);           //��������߳��Ƿ����
	bool ThreadPoolIsContinue(void);        //����߳�����״̬
	int getThreadPoolThreadCount(void);    
	int getThreadPoolIdleThreadCount(void);
	SThreadToken* getTToken(void);
};

void printThreadInfo(void *pCallParam,MBOOL &bThreadContinue);
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//�����
typedef bool(*_TASKPOOL_CALLBACK)(void *pCallParam,int &nStatus);
////////////////////////////////////////////////////////////////////////////
//#define TASK_POOL_TOKEN_MAX (16*1024)      //��ಢ��������
//#define DEFAULT_THREAD_MAX (30)             //Ĭ������߳���
/////////////////////////////////////////////////////////////////////////
//����غ������ݽṹ
typedef struct _TASK_POOL_TOKEN_
{
	_TASKPOOL_CALLBACK m_pCallback;                      //�ص�����ָ��
	void               *m_pUserParam;                     //���û����ݵĻص���������
	int                m_nUserStatus;                     //�����û����ݵ�״ֵ̬����ʼΪ0
}STaskPoolToken;
const unsigned long STaskPoolTokenSize=sizeof(STaskPoolToken);
////////////////////////////////////////////////////////////////////////
class  CNEOTaskPool
{
public:
	CNEOTaskPool(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,
		CNEOLog *m_pLog,int nMaxThread=DEFAULT_THREAD_MAX);

	~CNEOTaskPool();
private:
	CMRSWbool m_bThreadContinue;           
	CMRSWint m_nThreadCount;
	CNEOMemQueueWithLock *m_pTaskQueue;  //�����������
	CNEOThreadPool *m_pThreadPool;       //�̳߳�ָ��
	int m_nMaxThread;                    //����̵߳ı������
	CMRSWint m_nThreadID;
	CNEOLowDebug *m_pDebug;    
	CNEOMemPoolWithLock *m_pMemPool;
	CNEOLog *m_pLog;
	
private:
	//��ʵ���ڲ�ע�ắ��
	bool RegisterATaskDolt(STaskPoolToken *pToken,int nLimit=-1);
	//�������߳�
	bool TaskServiceThreadDolt(STaskPoolToken &Task);
	static void TaskServiceThread(void *pCallParam,
		MBOOL &bThreadContinue);
	//�������߳�
	static void TaskCtrlThread(void *pCallParam,
		MBOOL &bThreadContinue);
public:
	bool ICanWork(void);
	void PrintInfo();
	void XGSysLog(const char *szFormat,...);
	bool isTaskPoolContinue(void);
	int getMaxThread(void);
	int getThreadID(void);
	int getThreadCount(void);
public:
	//ע��һ��������
	bool RegisterATask(_TASKPOOL_CALLBACK pCallback,void *pUserParam=NULL);
};
/*
	only for test
*/
bool printTaskPoolInfo(void *pCallParam,int &nStatus);
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//����ؼ���
//#define NEO_TASK_RUN_MAX_TASK 16              //��ಽ����
typedef struct _NEO_TASK_RUN_INFO_
{
	int m_nTaskCount;                         //�ܹ����ٲ���
	void *m_pCallParam;                       //���õĻص���������
	//�����ص���������
	_TASKPOOL_CALLBACK m_CallbackArray[NEO_TASK_RUN_MAX_TASK];
}SNEOTaskRunInfo;
const unsigned long SNEOTaskRunInfoSize=sizeof(SNEOTaskRunInfo);
//����CNEOTaskRun�Ľṹ
typedef struct _NEOTeskRunTaskCallback_Param_
{
	SNEOTaskRunInfo m_Info;                  //����ṹ������
	//CNEOBaseLibrary *m_pNEOBaseLib;          //�����ۺϹ�����
	CNEOTaskRun *m_pThis;                    //�������������ָ��
	int m_nRunIndex;                         //��ǰִ�еĲ���
	char szAppName[256];                     //Ӧ����
}SNEOTestRunTaskCallbackParam;
const unsigned long SNEOTestRunTaskCallbackParamSize=sizeof(SNEOTestRunTaskCallbackParam);
//����������Ϣ
/*
	CNEOTaskRunInfo��һ�������࣬Ϊ
	CNEOTaskRun�ṩ���񣬸��������ע��
*/
class  CNEOTaskRunInfo
{
public:
	CNEOTaskRunInfo(SNEOTestRunTaskCallbackParam *pParam);
	CNEOTaskRunInfo(SNEOTaskRunInfo *pInfo);
	CNEOTaskRunInfo();
	~CNEOTaskRunInfo();
public:
	SNEOTaskRunInfo m_Info;
	SNEOTaskRunInfo *m_pInfo;
private:
	static void Init(SNEOTaskRunInfo *pInfo);
public:
	//��õ�ǰʵ��ʹ�õ�������
	SNEOTaskRunInfo *GetInfoPoint(void);
	//���ù��ò���ָ��
	void SetCallbackParam(void *pCallParam);
	//���µ�����ص�������ӵ��ڲ�������������ĩβ
	bool AddTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam);
	bool AddTask(_TASKPOOL_CALLBACK pCallback);
	//�����ݽṹ�����ݿ�������ǰ������ʹ�õ����ݽṹ����
	void CopyFrom(SNEOTaskRunInfo *pInfo);
};
/////////////////////////////////////////////////////////////////////////
//����������
class  CNEOTaskRun
{
public:
	//CNEOTaskRun(CNEOBaseLibrary *pNEOBaseLib);
	CNEOTaskRun(CNEOMemPoolWithLock *pMemPool,CNEOLog *pLog,CNEOTaskPool *pTaskPool);
	~CNEOTaskRun();
private:
	//����ص�����
	 static bool NEOTestRunTaskCallback(void *pCallParam,int &nStatus);
	CThreadManager m_ThreadManager;  
	//CNEOBaseLibrary *m_pNEOBaseLib;
	  //��־ģ��
    CNEOLog *m_pLog;
    //�ڴ��
    CNEOMemPoolWithLock *m_pMemPool;
    //�̳߳�
    CNEOTaskPool *m_pTaskPool;
public:
	void XGSysLog(const char *szFormat,...);
	//����һ������
	bool StartTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam=NULL,char *szAppName=NULL);
	//����Info���������������
	bool StartTask(SNEOTaskRunInfo *pInfoStruct,char *szAppName=NULL);
	bool StartTask(CNEOTaskRunInfo *pInfoObject,char *szAppName=NULL);
	//ֹͣ��������
	void StopAll(void);
	bool IsRunning(void);
	int GetThreadCount(void);
	void PrintInfo(void);
};

}
#endif
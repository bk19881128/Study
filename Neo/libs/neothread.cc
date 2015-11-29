//Ҫ����neoͷ�ļ�
//�������Ķ�����
//����debug��
//����memmanage�ࡣ
//��������Ķ�����
//���빤����
#include "neoindex.h"    

#include "neolock.h"    
#include "neodebug.h"         
#include "neosafefunc.h"                  
#include "neomemmanager.h"                          
#include "neoqueue.h"                               
//#include "neobaselib.h"
#include "neolog.h"
#include "neothread.h"
namespace NEOLIB {
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//�̳߳��ຯ��
CNEOThreadPool::CNEOThreadPool(CNEOLowDebug *pDebug):
    m_pDebug(pDebug)
{
    m_pDebug->DebugToFile("CNEOThreadPool Start!\n");
    //THREADID id = 0;
    THREAD t = 0;                                            //�����ڲ�����
    //ʹ�ô�c����
    MUTEXINIT(&m_RegisterLock);                          //��ʼ���ܵ��߳���
    MvarInit(m_bThreadContinue,true);
    MvarInit(m_nThreadPoolThreadCount,0);
    MvarInit(m_nThreadPoolIdleThreadCount,0);
    //��ʼ������
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
        m_TToken[i].m_hThread=0;
        m_TToken[i].m_nThreadID=0;
        //������ԣ�Ϊÿ�������߳������˳�����
        m_TToken[i].m_nExitCode=THREAD_POOL_EXIT_CODE+i;
        //null��ʾû�������߳�������Ҳ���ڿ�ת
        m_TToken[i].m_pCallback=NULL;
        m_TToken[i].m_pCallParam=NULL;
        //�̳߳��Լ���ָ��
        m_TToken[i].m_pThreadPoolObjext=this;
        //��ʼ������Ԫ״̬����
        MvarInit(m_TToken[i].m_nState,TPOOL_THREAD_STATE_NOT_RUN);
    }//for

    //�����������̣߳�����
    THREADCREATE(ThreadPoolCtrlThread,this,t,id);
    Sleep(OPEN_THREAD_DELAY);                         
}
//////////////////////////////////////////////////////////////////////////
//��������
CNEOThreadPool::~CNEOThreadPool()
{
    MvarSet(m_bThreadContinue,false);
    while(MvarGet(m_nThreadPoolThreadCount))
    {
       Sleep(MIN_SLEEP);//�ȴ��ر������߳�
    }
    //�ݻ������̲߳���ģ���״̬����
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
       MvarDestroy(m_TToken[i].m_nState);
    }
    MvarDestroy(m_bThreadContinue);
    MvarDestroy(m_nThreadPoolThreadCount);
    MvarDestroy(m_nThreadPoolIdleThreadCount);
    MUTEXDESTROY(&m_RegisterLock);
    m_pDebug->DebugToFile("CNEOThreadPool stop!\n");//��ʾ�˳���Ϣ
}
///////////////////////////////////////////////////////////////////////
int CNEOThreadPool:: SearchForNotUseToken(void)
{
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
       if(TPOOL_THREAD_STATE_NOT_RUN==MvarGet(m_TToken[i].m_nState))
           return i;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////////
//�������̺߳���
/*
1.ά���߳������Լ�Ҳ��һ��
2.����10�������߳�
*/
THREADFUNC(CNEOThreadPool::ThreadPoolCtrlThread,pParam)
//unsigned WINAPI CNEOThreadPool::ThreadPoolCtrlThread(LPVOID pParam)
{
    CNEOThreadPool *pThis=(CNEOThreadPool*)pParam;
    //�̳߳س���ʹ�ã��̼߳�����������+1�������Ƿ��ڹ��캯����
    MvarAdd(pThis->m_nThreadPoolThreadCount);
    int nIdleThread=0;                       //�����̼߳���
    int nNotRunThread=0;                     //δ�����̼߳���
    //��ѭ��
    while(MvarGet(pThis->m_bThreadContinue))
    {
       //��õ�ǰ�Ŀ����߳���
        nIdleThread=MvarGet(pThis->m_nThreadPoolIdleThreadCount);
        if(WHILE_THREAD_COUNT>nIdleThread)
        {
           //��������̲߳���10�������½��̣�����ǰ����Ҫ���ҵ����е������,��������
            nNotRunThread=pThis->SearchForNotUseToken();
            if(-1!=nNotRunThread)
            {
               // �����߳�
                THREADCREATE(ThreadPoolThread         //�������߳���
                    ,&(pThis->m_TToken[nNotRunThread])//���ݿ���Ϊ��������
                    ,pThis->m_TToken[nNotRunThread].m_hThread
                    ,pThis->m_TToken[nNotRunThread].m_nThreadID);
                //δ�����������ٹ�
            }//if
        }//if
        Sleep(OPEN_THREAD_DELAY);
    }//while
    MvarDec(pThis->m_nThreadPoolThreadCount);         //�˳�ʱ���̼߳���-1
#ifdef WIN32
    return THREAD_POOL_EXIT_CODE-1;
#else
    return NULL;
#endif
}
/////////////////////////////////////////////////////////////////////////
//�������̣߳�Ϊ�����ṩ���̣߳�
/*
1.ά���߳�������
2.ά�������߳���
3.ά��״̬��
  1.not run������ΪIdle��
  2.idle��ͬbusy�Ĵ���
  3.busy����ִ������
*/
THREADFUNC(CNEOThreadPool::ThreadPoolThread,pParam)
//unsigned WINAPI CNEOThreadPool::ThreadPoolThread(LPVOID pParam)
{
    //���������ָ�� pThis->m_pThreadPoolObjext
    SThreadToken *pPoint=(SThreadToken*)pParam;
    //������������ΪIdle
    MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_IDLE);
    //�̼߳���+1
    MvarAdd(pPoint->m_pThreadPoolObjext->m_nThreadPoolThreadCount);
    //Idle�̼߳���+1
    MvarAdd(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount);
    while(MvarGet(pPoint->m_pThreadPoolObjext->m_bThreadContinue))
    {
       //ȥ����
        switch(MvarGet(pPoint->m_nState))
        {
        case TPOOL_THREAD_STATE_NOT_RUN:
            //��ʾû���߳�Ϊ�������񡣵����߳��Ѿ�����
            MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_IDLE);
            //ע��û��break
        case TPOOL_THREAD_STATE_IDLE:
        
        case TPOOL_THREAD_STATE_BUSY:
            //û�а�IDLe����������Ϊ-1��
            //��ΪRegister���������������
            if(pPoint->m_pCallback)//����Ƿ���������
            {
               //��ִ��Ȩ�����µ�����
                pPoint->m_pCallback(pPoint->m_pCallParam,pPoint->m_pThreadPoolObjext->m_bThreadContinue);
                //�����߳���+1
                MvarAdd(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount);

            }//if
            break;
        default:
            break;
        };//switch
        //�������߳�����
        if(WHILE_THREAD_COUNT<MvarGet(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount))
            break;                  //��������̳߳����޶��������ѭ��
        //���й�����ɣ����Լ���ΪIdel״̬
        if(TPOOL_THREAD_STATE_IDLE!=MvarGet(pPoint->m_nState))
            MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_IDLE);
        pPoint->m_pCallback = NULL;
        pPoint->m_pCallParam = NULL;

        Sleep(DEFAULT_THREAD_SLEEP);//˯�ߣ��ȴ��´�����
    }//while
    //�˳�����
    //Idle������-1
    MvarDec(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount);
    //�̼߳�����-1
    MvarDec(pPoint->m_pThreadPoolObjext->m_nThreadPoolThreadCount);
    //�����������״̬����Ϊ��ȷ��ֵ
    MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_NOT_RUN);
#ifdef WIN32
    return pPoint->m_nExitCode;
#else
    return NULL;
#endif
}
//////////////////////////////////////////////////////////////////////////
int CNEOThreadPool::GetAIdleThread(void)
{
    int nRet=-1;
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
       if(TPOOL_THREAD_STATE_IDLE==MvarGet(m_TToken[i].m_nState))
       {
          nRet=i;
          break;
       }
    }
    return nRet;
}
//////////////////////////////////////////////////////////////////////////
//ע��һ��������
int CNEOThreadPool::ThreadPoolRegisterANewThread(_TPOOL_CALLBACK pCallback,void *pParam)
{
    int nRet=_THREADPOOL_PLEASE_WAIT;
    MUTEXLOCK(&m_RegisterLock);//����
    int nIdleThread=GetAIdleThread();      // ȡ��IDle���̱߳��
    if(0>nIdleThread)
    {
       //û���ҵ�IDEL�߳�
        if(THIS_POOLTHREAD_MAX==MvarGet(m_nThreadPoolThreadCount))
        {
           //û�п����̣߳����߳��Ѿ��ﵽ���ޣ�����Overflow
            nRet=_THREADPOOL_OVERFLOW;
        }
        else
        {
           //�ȴ������߳�
            nRet=_THREADPOOL_PLEASE_WAIT;
        }
    }//if
    else 
    {
       //�ҵ������̣߳��������(���ص纯�������̣߳����߳�ִ��)
        m_TToken[nIdleThread].m_pCallback=pCallback;
        m_TToken[nIdleThread].m_pCallParam=pParam;
        MvarSet(m_TToken[nIdleThread].m_nState,
            TPOOL_THREAD_STATE_BUSY);
        MvarDec(m_nThreadPoolIdleThreadCount);
        nRet=_THREADPOOL_OK;
    }
    MUTEXUNLOCK(&m_RegisterLock);     //����
    return nRet;
}
////////////////////////////////////////////////////////////////////////////��֤ע��ɹ�
int CNEOThreadPool::ThreadPoolRegisterANewThreadWhile(_TPOOL_CALLBACK pCallback,void *pParam)
{
    int nRet;
    while(1)
    {
       //������һ��������ʼע��
        nRet=ThreadPoolRegisterANewThread(pCallback,pParam);
        //ע��ɹ����������
        if(_THREADPOOL_PLEASE_WAIT!=nRet)
            break;
        Sleep(OPEN_THREAD_DELAY);
    }
    return nRet;
}
////////////////////////////////////////////////////////////////////////////�������溯����΢�����ṩע�Ṧ��
int CNEOThreadPool::ThreadPoolRegTask(_TPOOL_CALLBACK pCallback,void *pParam,bool bWaitForSuccess/*�Ƿ�ȴ�ע��ɹ�*/)
{
    if(bWaitForSuccess)
        return ThreadPoolRegisterANewThreadWhile(pCallback,pParam);
    else
        return ThreadPoolRegisterANewThread(pCallback,pParam);
}
//////////////////////////////////////////////////////////////////////////
bool CNEOThreadPool::TPALLThreadIsIdle(void)
{
    bool bRet=true;                               //ȫ�����У��򷵻���
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
        if(TPOOL_THREAD_STATE_IDLE!=m_TToken[i].m_nState.m_nValue)
        {
            bRet=false;
            break;
        }
    }
    return bRet;
}
/////////////////////////////////////////////////////////////////////////
bool CNEOThreadPool::ThreadPoolIsContinue(void)
{
    return (bool)MvarGet(m_bThreadContinue);
}

int CNEOThreadPool::getThreadPoolThreadCount(void)
{
    return MvarGet(m_nThreadPoolThreadCount);
}
int CNEOThreadPool::getThreadPoolIdleThreadCount(void)
{
    return MvarGet(m_nThreadPoolIdleThreadCount);
}

SThreadToken* CNEOThreadPool::getTToken(void)
{
    return m_TToken;
}

/* for test
    �û�д���淶
*/
void printThreadInfo(void *pCallParam,MBOOL &bThreadContinue)
{
    CNEOThreadPool *pThis=(CNEOThreadPool*)pCallParam;
    SThreadToken *pPoint=pThis->getTToken();

    printf("m_nThreadPoolThreadCount=%d,m_nThreadPoolIdleThreadCount=%d\r\n",\
        pThis->getThreadPoolThreadCount(),pThis->getThreadPoolIdleThreadCount()\
        );
    int i = 0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
        if(TPOOL_THREAD_STATE_NOT_RUN != (pPoint[i].m_nState.m_nValue))
        {
            printf("m_hThread=%u,m_nExitCode=%d,m_nState=%d,m_nThreadID=%d,m_pCallback=%p,m_pCallParam=%p,m_pThreadPoolObjext=%p,\r\n",\
                    (unsigned int)pPoint[i].m_hThread,pPoint[i].m_nExitCode,pPoint[i].m_nState.m_nValue,pPoint[i].m_nThreadID,\
                    pPoint[i].m_pCallback,pPoint[i].m_pCallParam,pPoint[i].m_pThreadPoolObjext
                    );
        }
    }

    //workaround fix warning
    MBOOL workaround;
    workaround = bThreadContinue;
    bThreadContinue = workaround;
}
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//�����
CNEOTaskPool::CNEOTaskPool(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,CNEOLog *pLog,int nMaxThread):
    m_bThreadContinue(),m_nThreadCount(),m_nThreadID(),\
    m_pDebug(pDebug),m_pMemPool(pMemPool),m_pLog(pLog)
{
    m_nMaxThread=nMaxThread;
    XGSysLog("CNEOTaskPool:start!\n");
    m_bThreadContinue.Set(true);
    m_nThreadCount.Set(0);
    m_nThreadID.Set(0);
    //ʵ�����������
    m_pTaskQueue=new CNEOMemQueueWithLock(m_pDebug,pMemPool,"NEO TaskPool");//Ӧ����
    if(m_pTaskQueue)
    {
       //ע�ᵽ�ڴ�ؽ��й���
        m_pMemPool->Register(m_pTaskQueue,"CNEOTaskPool::m_pTaskQueue");
    }
    m_pThreadPool=new CNEOThreadPool(m_pDebug);
    if(m_pThreadPool)
    {
       //ע�ᵽ�ڴ�ؽ��й���
        m_pMemPool->Register(m_pThreadPool,"CNEOTaskPool::m_pThreadPool");
    }
    if(ICanWork())
    {
       if(!m_pThreadPool->ThreadPoolRegTask(TaskCtrlThread,this))
       {
          XGSysLog("CNEOTaskPool::start ctrl thread %d fail!\n");
       }
       else 
           m_nThreadCount.Add();               //ע��ɹ�
    }
}

CNEOTaskPool::~CNEOTaskPool()
{
    //�˳�
    m_bThreadContinue.Set(false);
    while(m_nThreadCount.Get())
    {
       Sleep(100);
    }
    if(m_pThreadPool)
    {
       m_pMemPool->UnRegister(m_pThreadPool);
       delete m_pThreadPool;
       m_pThreadPool=NULL;
    }
    if(m_pTaskQueue)
    {
       m_pMemPool->UnRegister(m_pTaskQueue);
       delete m_pTaskQueue;
       m_pTaskQueue=NULL;
    }
    XGSysLog("CNEOTaskPool:stop!\n");
}
void CNEOTaskPool::XGSysLog(const char *szFormat,...)
{
    char szBuf[LOG_ITEM_LENGTH_MAX] = "\0";
    int nMaxLength=LOG_ITEM_LENGTH_MAX;
    int nListCount=0;
    va_list pArgList;
    va_start(pArgList,szFormat);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,nMaxLength-nListCount,szFormat,pArgList);
    va_end(pArgList);
    if(nListCount>(nMaxLength-1))
        nListCount=nMaxLength-1;
    *(szBuf+nListCount)='\0';
    m_pLog->_XGSysLog("%s\n",szBuf);
    return;
}

bool CNEOTaskPool::isTaskPoolContinue(void)
{
    return m_bThreadContinue.Get();
}

int CNEOTaskPool::getMaxThread(void)
{
    return m_nMaxThread;
}

int CNEOTaskPool::getThreadID(void)
{
    return m_nThreadID.Get();
}

int CNEOTaskPool::getThreadCount(void)
{
    return m_nThreadCount.Get();
}

bool CNEOTaskPool::ICanWork()
{
    if(!m_pThreadPool)
        return false;
    if(!m_pTaskQueue)
        return false;
    return true;
}
void CNEOTaskPool::PrintInfo()
{
    NEO_PRINTF("task pool :thread count=%d,task in queue=%d\n",m_nThreadCount.Get(),m_pTaskQueue->GetTokenCount());
}
bool CNEOTaskPool::RegisterATask(_TASKPOOL_CALLBACK pCallback,void *pUserParam)
{
    STaskPoolToken Token;
    if(!ICanWork())
        return false;
    if(!pCallback)
        return false;
    Token.m_pCallback=pCallback;
    Token.m_pUserParam=pUserParam;
    Token.m_nUserStatus=0;
    return RegisterATaskDolt(&Token,m_nMaxThread);//��������ĺ��������ע��
}
bool CNEOTaskPool::RegisterATaskDolt(STaskPoolToken *pToken,int nLimit)
{
    bool bRet=false;
    if(STaskPoolTokenSize==m_pTaskQueue->AddLast((char *)pToken,STaskPoolTokenSize) && nLimit)
        bRet=true;
    return bRet;
}
//�����̵߳Ļص�����,�����ܵ��߳�����������Ӧ�����ķ����߳�
/*
    1.�������������̣߳�30���߳�
    2.ά�������̸߳���
    3.�������֮�󣬼����̸߳���
*/
void CNEOTaskPool::TaskCtrlThread(void *pCallParam,
        MBOOL &bThreadContinue)
{
    CNEOTaskPool *pThis=(CNEOTaskPool*)pCallParam;
    int i=0;
    for(i=0;i<pThis->m_nMaxThread;i++)
    {
       if(!pThis->m_pThreadPool->ThreadPoolRegTask(TaskServiceThread,pThis))
       {
          pThis->XGSysLog("CNEOTaskPool::start service thread %d fail!\n",i);
          break;
       }
       else
       {
          pThis->m_nThreadCount.Add();
       }
    }//for
    pThis->m_nThreadCount.Dec();

    //workaround fix warning
    MBOOL workaround;
    workaround = bThreadContinue;
    bThreadContinue = workaround;
}
/*
  �ػ��̻߳�����30�������̣߳�
  1.�����̳߳ؽ�������ѭ����
  2.�ܹ�����task pool�ı�ǣ���ʱ�˳���
  3.�����������ȡ����Ȼ��ִ�У�
  4.�ص�������userstatus����������ע��δִ����ɵ����񣬲�����״ִ̬����һ���׶�
*/
void CNEOTaskPool::TaskServiceThread(void *pCallParam,
        MBOOL &bThreadContinue)
{
    int nQueueRet=0;
    STaskPoolToken Task;
    char *szTask=(char*)&Task;                //ǿ������ת��
    //��ñ�����ָ��
    CNEOTaskPool *pThis=(CNEOTaskPool *)pCallParam;
    int nID=pThis->m_nThreadID.Add()-1;
	//pThis->XGSysLog("CNEOTaskPool::TaskServiceThread():start %d!\n",nID);

    while(MvarGet(bThreadContinue))          // ��׼���̳߳��غ�ѭ��
    {
       if(!pThis->m_bThreadContinue.Get())
           goto CNEOTaskPool_TaskServiceThread_End;
       //�Ӷ����е�����һ�����񣬲�ִ��
       nQueueRet=pThis->m_pTaskQueue->GetAndDeleteFirst(szTask,STaskPoolTokenSize);
       if(STaskPoolTokenSize==nQueueRet)//���������ʾ������
       {
          pThis->TaskServiceThreadDolt(Task);//Ϊ�����ṩ����
       }
       Sleep(MIN_SLEEP);
    }//while
CNEOTaskPool_TaskServiceThread_End:
    pThis->m_nThreadCount.Dec();
    pThis->XGSysLog("CNEOTaskPool::TaskServiceThread():stop %d!\n",nID);
}
//�������߳�
bool CNEOTaskPool::TaskServiceThreadDolt(STaskPoolToken &sTask)
{
    bool bCallbackRet=sTask.m_pCallback(sTask.m_pUserParam,sTask.m_nUserStatus);
    if(!bCallbackRet)
        return bCallbackRet;           //���ؼ٣���ʾ�������
    bCallbackRet=RegisterATaskDolt(&sTask);//�������ʾ������δ��ɣ���ͼ����ע��
    if(!bCallbackRet)
    {
       m_pDebug->DebugToFile("CNEOTaskPool::TaskServiceThreadDolt:a task need continue but add to queue fail!task lost!\n");
       m_pDebug->DebugToFileBin((char *)&sTask,STaskPoolTokenSize);
    }
    return bCallbackRet;
}
/*
    �û�����ʾ��
*/
bool printTaskPoolInfo(void *pCallParam,int &nStatus)
{
    bool ret = true;
    CNEOTaskPool *pThis=(CNEOTaskPool *)pCallParam;
    printf("CNEOTaskPool������������������������\
           m_bThreadContinue=%d,m_nMaxThread=%d,m_nThreadID=%d,m_nThreadCount=%d\r\n",\
        pThis->isTaskPoolContinue(),pThis->getMaxThread(),pThis->getThreadID(),pThis->getThreadCount());

    switch(nStatus)
    {
        case 0:
            printf("case 1 user status=%d\r\n",nStatus);
            nStatus = 1;
            break;
        case 1:
            printf("case 2 user status=%d\r\n",nStatus);
            ret = false;
            break;
        case 2:
            break;
        default:
            break;
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//����صļ���
CNEOTaskRunInfo::CNEOTaskRunInfo(SNEOTestRunTaskCallbackParam *pParam)
{
    m_pInfo=&(pParam->m_Info);
    Init(m_pInfo);
}
CNEOTaskRunInfo::CNEOTaskRunInfo(SNEOTaskRunInfo *pInfo)
{
    m_pInfo=pInfo;
    Init(m_pInfo);
}
CNEOTaskRunInfo::CNEOTaskRunInfo()
{
    m_pInfo=NULL;
    Init(&m_Info);
}
CNEOTaskRunInfo::~CNEOTaskRunInfo()
{

}
void CNEOTaskRunInfo::Init(SNEOTaskRunInfo *pInfo)//��ʼ������
{
    pInfo->m_nTaskCount=0;
    pInfo->m_pCallParam=NULL;
    int i=0;
    for(i=0;i<NEO_TASK_RUN_MAX_TASK;i++)//���16���ص�����ָ��
    {
       pInfo->m_CallbackArray[i]=NULL;
    }
}
//��õ�ǰʵ��ʹ�õ�������
SNEOTaskRunInfo *CNEOTaskRunInfo::GetInfoPoint(void)
{
    if(m_pInfo)
        return m_pInfo;
    else 
        return &m_Info;
}
//���ù��ò���ָ��
void CNEOTaskRunInfo::SetCallbackParam(void *pCallParam)
{
    if(m_pInfo)
        m_pInfo->m_pCallParam=pCallParam;
    else 
        m_Info.m_pCallParam=pCallParam;
}
//���µ�����ص�������ӵ��ڲ�������������ĩβ
bool CNEOTaskRunInfo::AddTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam)
{
    if(pCallParam)
        SetCallbackParam(pCallParam);
    return AddTask(pCallback);
}
bool CNEOTaskRunInfo::AddTask(_TASKPOOL_CALLBACK pCallback)
{
    if(m_pInfo)
    {
       //���ص������Ƿ�����
        if(NEO_TASK_RUN_MAX_TASK<=m_pInfo->m_nTaskCount)
            return false;
        //��ӵ�����β
        m_pInfo->m_CallbackArray[m_pInfo->m_nTaskCount]=pCallback;
        m_pInfo->m_nTaskCount++;
    }
    else
    {
       if(NEO_TASK_RUN_MAX_TASK<=m_Info.m_nTaskCount)
            return false;
       m_Info.m_CallbackArray[m_Info.m_nTaskCount]=pCallback;
       m_Info.m_nTaskCount++;
    }

    return true;
}
//�����ݽṹ�����ݿ�������ǰ������ʹ�õ����ݽṹ����
void CNEOTaskRunInfo::CopyFrom(SNEOTaskRunInfo *pInfo)
{
    char *pMyInfo=NULL;
    if(m_pInfo)
        pMyInfo=(char *)m_pInfo;
    else 
        pMyInfo=(char *)&m_Info;
    memcpy(pMyInfo,(char*)pInfo,SNEOTaskRunInfoSize);
}
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/*CNEOTaskRun::CNEOTaskRun(CNEOBaseLibrary *pNEOBaseLib)
{
    m_pNEOBaseLib=pNEOBaseLib;
}*/

CNEOTaskRun::CNEOTaskRun(CNEOMemPoolWithLock *pMemPool,CNEOLog *pLog,CNEOTaskPool *pTaskPool)
{
	m_pMemPool = pMemPool;
	m_pLog = pLog;
	m_pTaskPool = pTaskPool;
}

CNEOTaskRun::~CNEOTaskRun()
{
    StopAll();
}
/////////////////////////////////////////////////////////////////////////
//����һ������
void CNEOTaskRun::XGSysLog(const char *szFormat,...)
{
    char szBuf[LOG_ITEM_LENGTH_MAX] = "\0";
    int nMaxLength=LOG_ITEM_LENGTH_MAX;
    int nListCount=0;
    va_list pArgList;
    va_start(pArgList,szFormat);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,nMaxLength-nListCount,szFormat,pArgList);
    va_end(pArgList);
    if(nListCount>(nMaxLength-1))
        nListCount=nMaxLength-1;
    *(szBuf+nListCount)='\0';
    m_pLog->_XGSysLog("%s\n",szBuf);
    return;
}

/*
    ������ʱtask info�� ������ע�ᵽtaskinfo�������У�
    ����starttask
*/
bool CNEOTaskRun::StartTask(_TASKPOOL_CALLBACK pCallback,
    void *pCallParam,char *szAppName)
{
    //���ע������
    CNEOTaskRunInfo InfoObj;
    InfoObj.AddTask(pCallback,pCallParam);  //ֱ�ӽ�������ӵ���������
    return StartTask(&InfoObj,szAppName);
}
//����Info���������������
/*
    ��task pool ע�����񣬴��ݲ���ΪSNEOTestRunTaskCallbackParam
*/
bool CNEOTaskRun::StartTask(SNEOTaskRunInfo *pInfoStruct,char *szAppName)
{
    bool bRet;
    if( !m_ThreadManager.ThreadContinue())//������Ϊ����
        m_ThreadManager.Open();
    //Զ�˴��Σ���̬����
    SNEOTestRunTaskCallbackParam *pParam=(SNEOTestRunTaskCallbackParam*)m_pMemPool->Malloc(
        SNEOTestRunTaskCallbackParamSize,"CNEOTaskRun::pParam");
    if(pParam)
    {
       //pParam->m_pNEOBaseLib=m_pNEOBaseLib;
       pParam->m_pThis=this;
       pParam->m_nRunIndex=0;              //״̬���ع�
       if(szAppName)
           SafeStrcpy(pParam->szAppName,szAppName,256);
       else
           NEO_CLEAN_CHAR_BUFFER(pParam->szAppName);

       CNEOTaskRunInfo InfoObj(&(pParam->m_Info));
       InfoObj.CopyFrom(pInfoStruct);
       //ע������
       bRet=m_pTaskPool->RegisterATask(NEOTestRunTaskCallback,pParam);
       if(bRet)
       {
          m_ThreadManager.AddThread();
          if(szAppName)
              XGSysLog("%s start ...\n",pParam->szAppName);
       }
    }
    return bRet;
}
bool CNEOTaskRun::StartTask(CNEOTaskRunInfo *pInfoObject,char *szAppName)
{
    return StartTask(pInfoObject->GetInfoPoint(),szAppName);
}
//ֹͣ��������
void CNEOTaskRun::StopAll(void)
{
    m_ThreadManager.CloseAll();
}
bool CNEOTaskRun::IsRunning(void)
{
    return m_ThreadManager.ThreadContinue();
}
int CNEOTaskRun::GetThreadCount(void)
{
    return m_ThreadManager.GetThreadCount();
}
void CNEOTaskRun::PrintInfo(void)
{
    NEO_PRINTF("task run :task count=%d\n",m_ThreadManager.GetThreadCount());
}
//����ص�����
/*
    nStatus��task run�Ĳ�������

*/
bool CNEOTaskRun::NEOTestRunTaskCallback(void *pCallParam,int &nStatus)
{
    bool bCallbackRet=false;            //��¼�û�����Ƭ�εĵ��ý��
    bool bGotoNextStatus=true;          //������һ״̬
    SNEOTestRunTaskCallbackParam *pParam=(SNEOTestRunTaskCallbackParam*)pCallParam;
    if(!pParam)
        return false;
    CNEOTaskRun *pThis=pParam->m_pThis;
    switch(nStatus)
    {
    case 0:                             //סִ�д���
        if(pParam->m_Info.m_nTaskCount>pParam->m_nRunIndex)
        {
            //ֻҪ�����δ��ѵ��ϣ�һֱ�ڱ�Ƭ��ִ��
            bGotoNextStatus=false;
            /*
                ��������������true����ʾ�������
                ����false ��ʾ������һ�׶Σ�init loop end��
                ������״̬��m_nRunIndex ��ʾ������Ľ׶�
                nStatus���������һ�������
            */
            bCallbackRet=pParam->m_Info.m_CallbackArray[pParam->m_nRunIndex](pParam->m_Info.m_pCallParam,pParam->m_nRunIndex);
            if(!bCallbackRet)
                pParam->m_nRunIndex++;//������һ��
            if(!pThis->m_ThreadManager.ThreadContinue())
                pParam->m_nRunIndex++;
        }
        else 
            bGotoNextStatus=true;
        break;
    default:
        if(0<strlen(pParam->szAppName))
            pThis->XGSysLog("%s stop!\n",pParam->szAppName);
        pThis->m_ThreadManager.DecAThread();
        pThis->m_pMemPool->Free(pParam);
        return false;
    }
    if(bGotoNextStatus)
        nStatus++;
    return true;
}
//#else
//#endif
}

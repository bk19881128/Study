//要求有neo头文件
//导入锁的定义类
//导入debug类
//导入memmanage类。
//导入加锁的队列类
//导入工具类
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
//线程池类函数
CNEOThreadPool::CNEOThreadPool(CNEOLowDebug *pDebug):
    m_pDebug(pDebug)
{
    m_pDebug->DebugToFile("CNEOThreadPool Start!\n");
    //THREADID id = 0;
    THREAD t = 0;                                            //函数内部变量
    //使用纯c的锁
    MUTEXINIT(&m_RegisterLock);                          //初始化总得线程锁
    MvarInit(m_bThreadContinue,true);
    MvarInit(m_nThreadPoolThreadCount,0);
    MvarInit(m_nThreadPoolIdleThreadCount,0);
    //初始化数组
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
        m_TToken[i].m_hThread=0;
        m_TToken[i].m_nThreadID=0;
        //方便调试，为每个服务线程设置退出代码
        m_TToken[i].m_nExitCode=THREAD_POOL_EXIT_CODE+i;
        //null表示没有任务。线程启动。也是在空转
        m_TToken[i].m_pCallback=NULL;
        m_TToken[i].m_pCallParam=NULL;
        //线程池自己的指针
        m_TToken[i].m_pThreadPoolObjext=this;
        //初始化任务单元状态变量
        MvarInit(m_TToken[i].m_nState,TPOOL_THREAD_STATE_NOT_RUN);
    }//for

    //创建管理者线程，启动
    THREADCREATE(ThreadPoolCtrlThread,this,t,id);
    Sleep(OPEN_THREAD_DELAY);                         
}
//////////////////////////////////////////////////////////////////////////
//析构函数
CNEOThreadPool::~CNEOThreadPool()
{
    MvarSet(m_bThreadContinue,false);
    while(MvarGet(m_nThreadPoolThreadCount))
    {
       Sleep(MIN_SLEEP);//等待关闭所有线程
    }
    //摧毁所有线程参数模块的状态变量
    int i=0;
    for(i=0;i<THIS_POOLTHREAD_MAX;i++)
    {
       MvarDestroy(m_TToken[i].m_nState);
    }
    MvarDestroy(m_bThreadContinue);
    MvarDestroy(m_nThreadPoolThreadCount);
    MvarDestroy(m_nThreadPoolIdleThreadCount);
    MUTEXDESTROY(&m_RegisterLock);
    m_pDebug->DebugToFile("CNEOThreadPool stop!\n");//显示退出信息
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
//管理者线程函数
/*
1.维护线程数，自己也算一个
2.启动10个空闲线程
*/
THREADFUNC(CNEOThreadPool::ThreadPoolCtrlThread,pParam)
//unsigned WINAPI CNEOThreadPool::ThreadPoolCtrlThread(LPVOID pParam)
{
    CNEOThreadPool *pThis=(CNEOThreadPool*)pParam;
    //线程池长期使用，线程计数器在其中+1，而不是放在构造函数中
    MvarAdd(pThis->m_nThreadPoolThreadCount);
    int nIdleThread=0;                       //空闲线程计数
    int nNotRunThread=0;                     //未运行线程计数
    //死循环
    while(MvarGet(pThis->m_bThreadContinue))
    {
       //获得当前的空闲线程数
        nIdleThread=MvarGet(pThis->m_nThreadPoolIdleThreadCount);
        if(WHILE_THREAD_COUNT>nIdleThread)
        {
           //如果备用线程不够10，启动新进程，启动前，需要先找到空闲的任务块,否则不启动
            nNotRunThread=pThis->SearchForNotUseToken();
            if(-1!=nNotRunThread)
            {
               // 启动线程
                THREADCREATE(ThreadPoolThread         //服务者线程名
                    ,&(pThis->m_TToken[nNotRunThread])//数据块作为参数传递
                    ,pThis->m_TToken[nNotRunThread].m_hThread
                    ,pThis->m_TToken[nNotRunThread].m_nThreadID);
                //未启动则下面再管
            }//if
        }//if
        Sleep(OPEN_THREAD_DELAY);
    }//while
    MvarDec(pThis->m_nThreadPoolThreadCount);         //退出时，线程计数-1
#ifdef WIN32
    return THREAD_POOL_EXIT_CODE-1;
#else
    return NULL;
#endif
}
/////////////////////////////////////////////////////////////////////////
//服务者线程（为任务提供的线程）
/*
1.维护线程数计数
2.维护空闲线程数
3.维护状态：
  1.not run：则设为Idle；
  2.idle，同busy的处理；
  3.busy，则执行任务
*/
THREADFUNC(CNEOThreadPool::ThreadPoolThread,pParam)
//unsigned WINAPI CNEOThreadPool::ThreadPoolThread(LPVOID pParam)
{
    //保存任务快指针 pThis->m_pThreadPoolObjext
    SThreadToken *pPoint=(SThreadToken*)pParam;
    //刚启动，设置为Idle
    MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_IDLE);
    //线程计数+1
    MvarAdd(pPoint->m_pThreadPoolObjext->m_nThreadPoolThreadCount);
    //Idle线程计数+1
    MvarAdd(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount);
    while(MvarGet(pPoint->m_pThreadPoolObjext->m_bThreadContinue))
    {
       //去任务
        switch(MvarGet(pPoint->m_nState))
        {
        case TPOOL_THREAD_STATE_NOT_RUN:
            //表示没有线程为任务块服务。但本线程已经启动
            MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_IDLE);
            //注意没有break
        case TPOOL_THREAD_STATE_IDLE:
        
        case TPOOL_THREAD_STATE_BUSY:
            //没有把IDLe计数器设置为-1，
            //因为Register函数做了这个动作
            if(pPoint->m_pCallback)//检查是否真有任务
            {
               //将执行权交给新的任务
                pPoint->m_pCallback(pPoint->m_pCallParam,pPoint->m_pThreadPoolObjext->m_bThreadContinue);
                //空闲线程数+1
                MvarAdd(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount);

            }//if
            break;
        default:
            break;
        };//switch
        //检查空闲线程总数
        if(WHILE_THREAD_COUNT<MvarGet(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount))
            break;                  //如果备用线程超出限额，则跳出死循环
        //所有工作完成，把自己置为Idel状态
        if(TPOOL_THREAD_STATE_IDLE!=MvarGet(pPoint->m_nState))
            MvarSet(pPoint->m_nState,TPOOL_THREAD_STATE_IDLE);
        pPoint->m_pCallback = NULL;
        pPoint->m_pCallParam = NULL;

        Sleep(DEFAULT_THREAD_SLEEP);//睡眠，等待下次任务
    }//while
    //退出流程
    //Idle计数器-1
    MvarDec(pPoint->m_pThreadPoolObjext->m_nThreadPoolIdleThreadCount);
    //线程计数器-1
    MvarDec(pPoint->m_pThreadPoolObjext->m_nThreadPoolThreadCount);
    //把任务区块的状态设置为正确的值
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
//注册一个新任务
int CNEOThreadPool::ThreadPoolRegisterANewThread(_TPOOL_CALLBACK pCallback,void *pParam)
{
    int nRet=_THREADPOOL_PLEASE_WAIT;
    MUTEXLOCK(&m_RegisterLock);//加锁
    int nIdleThread=GetAIdleThread();      // 取得IDle的线程编号
    if(0>nIdleThread)
    {
       //没有找到IDEL线程
        if(THIS_POOLTHREAD_MAX==MvarGet(m_nThreadPoolThreadCount))
        {
           //没有空闲线程，且线程已经达到上限，返回Overflow
            nRet=_THREADPOOL_OVERFLOW;
        }
        else
        {
           //等待开启线程
            nRet=_THREADPOOL_PLEASE_WAIT;
        }
    }//if
    else 
    {
       //找到空闲线程，添加任务(将回电函数传给线程，让线程执行)
        m_TToken[nIdleThread].m_pCallback=pCallback;
        m_TToken[nIdleThread].m_pCallParam=pParam;
        MvarSet(m_TToken[nIdleThread].m_nState,
            TPOOL_THREAD_STATE_BUSY);
        MvarDec(m_nThreadPoolIdleThreadCount);
        nRet=_THREADPOOL_OK;
    }
    MUTEXUNLOCK(&m_RegisterLock);     //解锁
    return nRet;
}
////////////////////////////////////////////////////////////////////////////保证注册成功
int CNEOThreadPool::ThreadPoolRegisterANewThreadWhile(_TPOOL_CALLBACK pCallback,void *pParam)
{
    int nRet;
    while(1)
    {
       //调用上一函数，开始注册
        nRet=ThreadPoolRegisterANewThread(pCallback,pParam);
        //注册成功，或者溢出
        if(_THREADPOOL_PLEASE_WAIT!=nRet)
            break;
        Sleep(OPEN_THREAD_DELAY);
    }
    return nRet;
}
////////////////////////////////////////////////////////////////////////////调用上面函数，微程序提供注册功能
int CNEOThreadPool::ThreadPoolRegTask(_TPOOL_CALLBACK pCallback,void *pParam,bool bWaitForSuccess/*是否等待注册成功*/)
{
    if(bWaitForSuccess)
        return ThreadPoolRegisterANewThreadWhile(pCallback,pParam);
    else
        return ThreadPoolRegisterANewThread(pCallback,pParam);
}
//////////////////////////////////////////////////////////////////////////
bool CNEOThreadPool::TPALLThreadIsIdle(void)
{
    bool bRet=true;                               //全部空闲，则返回真
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
    用户写作规范
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
//任务池
CNEOTaskPool::CNEOTaskPool(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,CNEOLog *pLog,int nMaxThread):
    m_bThreadContinue(),m_nThreadCount(),m_nThreadID(),\
    m_pDebug(pDebug),m_pMemPool(pMemPool),m_pLog(pLog)
{
    m_nMaxThread=nMaxThread;
    XGSysLog("CNEOTaskPool:start!\n");
    m_bThreadContinue.Set(true);
    m_nThreadCount.Set(0);
    m_nThreadID.Set(0);
    //实例化任务队列
    m_pTaskQueue=new CNEOMemQueueWithLock(m_pDebug,pMemPool,"NEO TaskPool");//应用名
    if(m_pTaskQueue)
    {
       //注册到内存池进行管理
        m_pMemPool->Register(m_pTaskQueue,"CNEOTaskPool::m_pTaskQueue");
    }
    m_pThreadPool=new CNEOThreadPool(m_pDebug);
    if(m_pThreadPool)
    {
       //注册到内存池进行管理
        m_pMemPool->Register(m_pThreadPool,"CNEOTaskPool::m_pThreadPool");
    }
    if(ICanWork())
    {
       if(!m_pThreadPool->ThreadPoolRegTask(TaskCtrlThread,this))
       {
          XGSysLog("CNEOTaskPool::start ctrl thread %d fail!\n");
       }
       else 
           m_nThreadCount.Add();               //注册成功
    }
}

CNEOTaskPool::~CNEOTaskPool()
{
    //退出
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
    return RegisterATaskDolt(&Token,m_nMaxThread);//调用上面的函数，完成注册
}
bool CNEOTaskPool::RegisterATaskDolt(STaskPoolToken *pToken,int nLimit)
{
    bool bRet=false;
    if(STaskPoolTokenSize==m_pTaskQueue->AddLast((char *)pToken,STaskPoolTokenSize) && nLimit)
        bRet=true;
    return bRet;
}
//管理线程的回调函数,根据总的线程数，启动对应数量的服务线程
/*
    1.用于启动服务线程，30个线程
    2.维护启动线程个数
    3.启动完成之后，减少线程个数
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
  守护线程会启动30个服务线程：
  1.随着线程池进行无限循环；
  2.能够根据task pool的标记，随时退出，
  3.从任务队列中取任务，然后执行，
  4.回调函数有userstatus，可以重新注册未执行完成的任务，并根据状态执行下一个阶段
*/
void CNEOTaskPool::TaskServiceThread(void *pCallParam,
        MBOOL &bThreadContinue)
{
    int nQueueRet=0;
    STaskPoolToken Task;
    char *szTask=(char*)&Task;                //强制类型转换
    //获得本对象指针
    CNEOTaskPool *pThis=(CNEOTaskPool *)pCallParam;
    int nID=pThis->m_nThreadID.Add()-1;
	//pThis->XGSysLog("CNEOTaskPool::TaskServiceThread():start %d!\n",nID);

    while(MvarGet(bThreadContinue))          // 标准的线程池守候循环
    {
       if(!pThis->m_bThreadContinue.Get())
           goto CNEOTaskPool_TaskServiceThread_End;
       //从队列中弹出第一个任务，并执行
       nQueueRet=pThis->m_pTaskQueue->GetAndDeleteFirst(szTask,STaskPoolTokenSize);
       if(STaskPoolTokenSize==nQueueRet)//如果弹出表示有任务
       {
          pThis->TaskServiceThreadDolt(Task);//为任务提供服务
       }
       Sleep(MIN_SLEEP);
    }//while
CNEOTaskPool_TaskServiceThread_End:
    pThis->m_nThreadCount.Dec();
    pThis->XGSysLog("CNEOTaskPool::TaskServiceThread():stop %d!\n",nID);
}
//服务者线程
bool CNEOTaskPool::TaskServiceThreadDolt(STaskPoolToken &sTask)
{
    bool bCallbackRet=sTask.m_pCallback(sTask.m_pUserParam,sTask.m_nUserStatus);
    if(!bCallbackRet)
        return bCallbackRet;           //返回假，表示任务结束
    bCallbackRet=RegisterATaskDolt(&sTask);//返回真表示任务尚未完成，试图重新注册
    if(!bCallbackRet)
    {
       m_pDebug->DebugToFile("CNEOTaskPool::TaskServiceThreadDolt:a task need continue but add to queue fail!task lost!\n");
       m_pDebug->DebugToFileBin((char *)&sTask,STaskPoolTokenSize);
    }
    return bCallbackRet;
}
/*
    用户函数示例
*/
bool printTaskPoolInfo(void *pCallParam,int &nStatus)
{
    bool ret = true;
    CNEOTaskPool *pThis=(CNEOTaskPool *)pCallParam;
    printf("CNEOTaskPool》》》》》》》》》》》》\
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
//任务池的简化类
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
void CNEOTaskRunInfo::Init(SNEOTaskRunInfo *pInfo)//初始化动作
{
    pInfo->m_nTaskCount=0;
    pInfo->m_pCallParam=NULL;
    int i=0;
    for(i=0;i<NEO_TASK_RUN_MAX_TASK;i++)//清空16个回调函数指针
    {
       pInfo->m_CallbackArray[i]=NULL;
    }
}
//获得当前实际使用的数据区
SNEOTaskRunInfo *CNEOTaskRunInfo::GetInfoPoint(void)
{
    if(m_pInfo)
        return m_pInfo;
    else 
        return &m_Info;
}
//设置共用参数指针
void CNEOTaskRunInfo::SetCallbackParam(void *pCallParam)
{
    if(m_pInfo)
        m_pInfo->m_pCallParam=pCallParam;
    else 
        m_Info.m_pCallParam=pCallParam;
}
//将新的任务回调函数添加到内部数据区的数组末尾
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
       //检查回调函数是否超上限
        if(NEO_TASK_RUN_MAX_TASK<=m_pInfo->m_nTaskCount)
            return false;
        //添加到数组尾
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
//将数据结构的内容拷贝到当前本对象使用的数据结构体中
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
//启动一个任务
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
    创建临时task info， 将任务注册到taskinfo的数组中，
    调用starttask
*/
bool CNEOTaskRun::StartTask(_TASKPOOL_CALLBACK pCallback,
    void *pCallParam,char *szAppName)
{
    //完成注册任务
    CNEOTaskRunInfo InfoObj;
    InfoObj.AddTask(pCallback,pCallParam);  //直接将任务添加到任务描述
    return StartTask(&InfoObj,szAppName);
}
//利用Info描述启动多次任务
/*
    向task pool 注册任务，传递参数为SNEOTestRunTaskCallbackParam
*/
bool CNEOTaskRun::StartTask(SNEOTaskRunInfo *pInfoStruct,char *szAppName)
{
    bool bRet;
    if( !m_ThreadManager.ThreadContinue())//本对象为启动
        m_ThreadManager.Open();
    //远端传参，动态分配
    SNEOTestRunTaskCallbackParam *pParam=(SNEOTestRunTaskCallbackParam*)m_pMemPool->Malloc(
        SNEOTestRunTaskCallbackParamSize,"CNEOTaskRun::pParam");
    if(pParam)
    {
       //pParam->m_pNEOBaseLib=m_pNEOBaseLib;
       pParam->m_pThis=this;
       pParam->m_nRunIndex=0;              //状态机回归
       if(szAppName)
           SafeStrcpy(pParam->szAppName,szAppName,256);
       else
           NEO_CLEAN_CHAR_BUFFER(pParam->szAppName);

       CNEOTaskRunInfo InfoObj(&(pParam->m_Info));
       InfoObj.CopyFrom(pInfoStruct);
       //注册任务
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
//停止所有任务
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
//任务回调函数
/*
    nStatus是task run的步骤依据

*/
bool CNEOTaskRun::NEOTestRunTaskCallback(void *pCallParam,int &nStatus)
{
    bool bCallbackRet=false;            //记录用户调用片段的调用结果
    bool bGotoNextStatus=true;          //跳到下一状态
    SNEOTestRunTaskCallbackParam *pParam=(SNEOTestRunTaskCallbackParam*)pCallParam;
    if(!pParam)
        return false;
    CNEOTaskRun *pThis=pParam->m_pThis;
    switch(nStatus)
    {
    case 0:                             //住执行代码
        if(pParam->m_Info.m_nTaskCount>pParam->m_nRunIndex)
        {
            //只要任务池未轮训完毕，一直在本片段执行
            bGotoNextStatus=false;
            /*
                任务函数，过返回true，表示任务结束
                返回false 表示进入下一阶段（init loop end）
                有两个状态，m_nRunIndex 表示本任务的阶段
                nStatus，大概是另一个任务吧
            */
            bCallbackRet=pParam->m_Info.m_CallbackArray[pParam->m_nRunIndex](pParam->m_Info.m_pCallParam,pParam->m_nRunIndex);
            if(!bCallbackRet)
                pParam->m_nRunIndex++;//跳到下一段
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

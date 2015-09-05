
#include "neolock.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//基本加锁类
namespace NEOLIB {

CMutexLock::CMutexLock(void)
{
    MUTEXINIT(&m_Lock);
}
CMutexLock::~CMutexLock(void)
{
    MUTEXDESTROY(&m_Lock);
}


void CMutexLock::Lock()
{
    MUTEXLOCK(&m_Lock);
}
void CMutexLock::UnLock()
{
    MUTEXUNLOCK(&m_Lock);
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////解决跨线程变量挣用
//c语言的加锁函数,
int MvarInit(MINT &(mValue)/*保护整形变量的值*/,int nValue)
{
    MUTEXINIT(&mValue.m_MyLock);
    mValue.m_nValue=nValue;
    return nValue;
}
void MvarDestroy(MINT &(mValue))
{
    //无意义的加锁，防止在摧毁是由干扰
    MUTEXLOCK(&mValue.m_MyLock);
    MUTEXUNLOCK(&mValue.m_MyLock);
    /////////////////////////////////
    MUTEXDESTROY(&mValue.m_MyLock);
}
//设置一个线程变量值
int MvarSet(MINT &(mValue),int nValue)
{
    MUTEXLOCK(&mValue.m_MyLock);
    mValue.m_nValue=nValue;
    MUTEXUNLOCK(&mValue.m_MyLock);
    return nValue;
}
//得到线程安全变量的值
int MvarGet(MINT &(mValue))
{
    int nValue;
    MUTEXLOCK(&mValue.m_MyLock);
    nValue=mValue.m_nValue;
    MUTEXUNLOCK(&mValue.m_MyLock);
    return nValue;
}
//线程安全变量做加法和解法
int MvarAdd(MINT &(mValue),int nValue)
{
    int nRet;
    MUTEXLOCK(&mValue.m_MyLock);
    mValue.m_nValue+=nValue;
    nRet=mValue.m_nValue;
    MUTEXUNLOCK(&mValue.m_MyLock);
    return nRet;
}
int MvarDec(MINT &(mValue),int nValue)
{
    int nRet;
    MUTEXLOCK(&mValue.m_MyLock);
    mValue.m_nValue-=nValue;
    nRet=mValue.m_nValue;
    MUTEXUNLOCK(&mValue.m_MyLock);
    return nRet;
}
//以上函数的c++类实现

CMint::CMint(int nValue)
{
    MvarInit(m_nValue,nValue);
}
CMint::~CMint(void)
{
    MvarDestroy(m_nValue);
}
int CMint::Get(void)
{
    return MvarGet(m_nValue);
}
int CMint::Set(int nValue)
{
    return MvarSet(m_nValue,nValue);
}
int CMint::Add(int nValue)
{
    return MvarAdd(m_nValue,nValue);
}
int CMint::Dec(int nValue)
{
    return MvarDec(m_nValue,nValue);
}

//对bool型变量的加锁

CMbool::CMbool(int nValue)
{
    MvarInit(m_nValue,nValue);
}
CMbool::~CMbool(void)
{
    MvarDestroy(m_nValue);
}

int CMbool::Get(void)
{
    return MvarGet(m_nValue);
}
int CMbool::Set(int nValue)
{
    return MvarSet(m_nValue,nValue);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//单写多读锁,只是单独的锁
void MRSWLock_Create(SNeoMultiReadSingleWriteLock *pLock)
{
    MUTEXINIT(&(pLock->m_Lock));
    pLock->m_nReadCount=0;
    pLock->m_bWriteFlag=false;
}
void MRSWLock_Destroy(SNeoMultiReadSingleWriteLock *pLock)
{
    MUTEXLOCK(&(pLock->m_Lock));
    MUTEXUNLOCK(&(pLock->m_Lock));
    /////////////////////////////////
    MUTEXDESTROY(&(pLock->m_Lock));
}
//锁状态获知
bool MRSWLock_GetWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    bool bRet=false;
    MUTEXLOCK(&(pLock->m_Lock));
    {
       bRet=pLock->m_bWriteFlag;
    }
    MUTEXUNLOCK(&(pLock->m_Lock));
    return bRet;
}
int  MRSWLock_GetRead(SNeoMultiReadSingleWriteLock *pLock)
{
    int iRet=0;
    MUTEXLOCK(&(pLock->m_Lock));
    {
       iRet=pLock->m_nReadCount;
    }
    MUTEXUNLOCK(&(pLock->m_Lock));
    return iRet;
}
//进入写操作
void MRSWLock_EnterWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    while(1)
    {
       MUTEXLOCK(&(pLock->m_Lock));//加锁进入锁域
       {
           //判断是否可以抢夺写的权利
          if(!pLock->m_bWriteFlag)
          {
             pLock->m_bWriteFlag=true;
             MUTEXUNLOCK(&(pLock->m_Lock));
             goto MRSWLock_EnterWrite_End;
          }
       }
       MUTEXUNLOCK(&(pLock->m_Lock));
       NEOMinSleep();//特殊的睡眠函数
    }//while
MRSWLock_EnterWrite_End:
    //等待其他的读完毕
    while(MRSWLock_GetRead(pLock))
    {
       NEOMinSleep();
    }
}
//退出写
void MRSWLock_DisableWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    MUTEXLOCK(&(pLock->m_Lock));
    {
       pLock->m_bWriteFlag=false;
    }
    MUTEXUNLOCK(&(pLock->m_Lock));
}
//进入读
int MRSWLock_AddRead(SNeoMultiReadSingleWriteLock *pLock)
{
    while(1)
    {
       MUTEXLOCK(&(pLock->m_Lock));//加锁进入锁域
       {
           //判断是否可以抢夺写的权利
          if(!pLock->m_bWriteFlag)
          {
             pLock->m_nReadCount++;
             MUTEXUNLOCK(&(pLock->m_Lock));
             return MRSWLock_GetRead(pLock);
          }
       }
       MUTEXUNLOCK(&(pLock->m_Lock));
       NEOMinSleep();//特殊的睡眠函数
    }//while
}
//退出读
int MRSWLock_DecRead(SNeoMultiReadSingleWriteLock *pLock)
{
    int nRet=0;
    MUTEXLOCK(&(pLock->m_Lock));
    {
       if(0<(pLock->m_nReadCount))
          pLock->m_nReadCount--;
       nRet=pLock->m_nReadCount;
    }
    MUTEXUNLOCK(&(pLock->m_Lock));
    return  nRet;
}
//读转写
void MRSWLock_ReadToWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    while(1)
    {
       MUTEXLOCK(&(pLock->m_Lock));//加锁进入锁域
       {
           //判断是否可以抢夺写的权利
          if(!pLock->m_bWriteFlag)
          {
             pLock->m_bWriteFlag=true;
             if(0<(pLock->m_nReadCount))
                 pLock->m_nReadCount--;
             MUTEXUNLOCK(&(pLock->m_Lock));
             goto MRSWLock_ReadToWrite_End;
          }
       }
       MUTEXUNLOCK(&(pLock->m_Lock));
       NEOMinSleep();//特殊的睡眠函数
    }//while
MRSWLock_ReadToWrite_End:
    //等待其他的读完毕
    while(MRSWLock_GetRead(pLock))
    {
       NEOMinSleep();
    }
}
//以上函数对应的c++封装
CMultiReadSingleWriteLock::CMultiReadSingleWriteLock()
{
    MRSWLock_Create(&m_Lock);
}
CMultiReadSingleWriteLock::~CMultiReadSingleWriteLock()
{
    MRSWLock_Destroy(&m_Lock);
}

void CMultiReadSingleWriteLock::EnableWrite(void)
{
    MRSWLock_EnterWrite(&m_Lock);
}
void CMultiReadSingleWriteLock::DisableWrite(void)
{
    MRSWLock_DisableWrite(&m_Lock);
}
void CMultiReadSingleWriteLock::ReadToWrite(void)
{
    MRSWLock_ReadToWrite(&m_Lock);
}
void CMultiReadSingleWriteLock::DecRead(void)
{
    MRSWLock_DecRead(&m_Lock);
}
void CMultiReadSingleWriteLock::AddRead(void)
{
    MRSWLock_AddRead(&m_Lock);
}
bool CMultiReadSingleWriteLock::GetWrite(void)
{
    return MRSWLock_GetWrite(&m_Lock);
}
int CMultiReadSingleWriteLock::GetRead()
{
    return MRSWLock_GetRead(&m_Lock);
}

//用以上的锁保护整形和bool型变量的类

CMRSWint::CMRSWint()
{
    m_Lock.EnableWrite();
    m_nValue=0;
    m_Lock.DisableWrite();
}
CMRSWint::~CMRSWint(){}

int CMRSWint::Get(void)//得到变量
{
    int nRet=0;
    m_Lock.AddRead();
    {
        nRet=m_nValue;
    }
    m_Lock.DecRead();
    return nRet;
}
int CMRSWint::Set(int nValue)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        m_nValue=nValue;
        nRet=m_nValue;
    }
    m_Lock.DisableWrite();
    return nRet;
}
int CMRSWint::Add(int nValue)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        m_nValue+=nValue;
        nRet=m_nValue;
    }
    m_Lock.DisableWrite();
    return nRet;
}
int CMRSWint::Dec(int nValue)
{
        int nRet=0;
    m_Lock.EnableWrite();
    {
        m_nValue-=nValue;
        nRet=m_nValue;
    }
    m_Lock.DisableWrite();
    return nRet;
}

CMRSWbool::CMRSWbool():
    m_nValue()
{}
CMRSWbool::~CMRSWbool(){}
int CMRSWbool::Get(void)//得到变量
{
    return (bool)m_nValue.Get();
}
int CMRSWbool::Set(bool bFlag)
{
    return m_nValue.Set((int)bFlag);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//不可重入锁

CNonReentrant::CNonReentrant()
{
    m_bAlreadyRunFlag=false;
}

bool CNonReentrant::Set(bool bRunFlag)
{
    bool bRet=false;
    if(bRunFlag)
    {
        m_Lock.Lock();
        {
            if(!m_bAlreadyRunFlag)
            {
            m_bAlreadyRunFlag=true;
            bRet=true;//表示设置成功
            }
        }
        m_Lock.UnLock();
    }
    else
    {
        m_Lock.Lock();
        {
            m_bAlreadyRunFlag=false;
            bRet=true;
        }
        m_Lock.UnLock();
    }
    return bRet;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//线程控制锁

CThreadManager::CThreadManager():
    m_bThreadContinue(),m_nThreadCount(),m_nThreadID()
{}
CThreadManager::~CThreadManager(){}
void CThreadManager::Open(void)
{
    CloseAll();
    m_bThreadContinue.Set(true);
    m_nThreadCount.Set(0);
}
void CThreadManager::CloseAll(void)
{
    m_bThreadContinue.Set(false);
    while(m_nThreadCount.Get())
    {
        NEOMinSleep();
    }
}
int CThreadManager::AddThread(void)
{
    return m_nThreadCount.Add();
}
void CThreadManager::DecAThread(void)
{
    m_nThreadCount.Dec();
}
//查询线程维持变量的值
bool CThreadManager::ThreadContinue(void)
{
    return m_bThreadContinue.Get();
}
//获得计数器的值
int CThreadManager::GetThreadCount(void)
{
    return m_nThreadCount.Get();
}
//分配一个线程ID供debug用
int CThreadManager::GetID(void)
{
    return m_nThreadID.Add()-1;
}

}
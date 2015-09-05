
#include "neolock.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//����������
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
/////////////////////////////////////////////////////////������̱߳�������
//c���Եļ�������,
int MvarInit(MINT &(mValue)/*�������α�����ֵ*/,int nValue)
{
    MUTEXINIT(&mValue.m_MyLock);
    mValue.m_nValue=nValue;
    return nValue;
}
void MvarDestroy(MINT &(mValue))
{
    //������ļ�������ֹ�ڴݻ����ɸ���
    MUTEXLOCK(&mValue.m_MyLock);
    MUTEXUNLOCK(&mValue.m_MyLock);
    /////////////////////////////////
    MUTEXDESTROY(&mValue.m_MyLock);
}
//����һ���̱߳���ֵ
int MvarSet(MINT &(mValue),int nValue)
{
    MUTEXLOCK(&mValue.m_MyLock);
    mValue.m_nValue=nValue;
    MUTEXUNLOCK(&mValue.m_MyLock);
    return nValue;
}
//�õ��̰߳�ȫ������ֵ
int MvarGet(MINT &(mValue))
{
    int nValue;
    MUTEXLOCK(&mValue.m_MyLock);
    nValue=mValue.m_nValue;
    MUTEXUNLOCK(&mValue.m_MyLock);
    return nValue;
}
//�̰߳�ȫ�������ӷ��ͽⷨ
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
//���Ϻ�����c++��ʵ��

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

//��bool�ͱ����ļ���

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

//��д�����,ֻ�ǵ�������
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
//��״̬��֪
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
//����д����
void MRSWLock_EnterWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    while(1)
    {
       MUTEXLOCK(&(pLock->m_Lock));//������������
       {
           //�ж��Ƿ��������д��Ȩ��
          if(!pLock->m_bWriteFlag)
          {
             pLock->m_bWriteFlag=true;
             MUTEXUNLOCK(&(pLock->m_Lock));
             goto MRSWLock_EnterWrite_End;
          }
       }
       MUTEXUNLOCK(&(pLock->m_Lock));
       NEOMinSleep();//�����˯�ߺ���
    }//while
MRSWLock_EnterWrite_End:
    //�ȴ������Ķ����
    while(MRSWLock_GetRead(pLock))
    {
       NEOMinSleep();
    }
}
//�˳�д
void MRSWLock_DisableWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    MUTEXLOCK(&(pLock->m_Lock));
    {
       pLock->m_bWriteFlag=false;
    }
    MUTEXUNLOCK(&(pLock->m_Lock));
}
//�����
int MRSWLock_AddRead(SNeoMultiReadSingleWriteLock *pLock)
{
    while(1)
    {
       MUTEXLOCK(&(pLock->m_Lock));//������������
       {
           //�ж��Ƿ��������д��Ȩ��
          if(!pLock->m_bWriteFlag)
          {
             pLock->m_nReadCount++;
             MUTEXUNLOCK(&(pLock->m_Lock));
             return MRSWLock_GetRead(pLock);
          }
       }
       MUTEXUNLOCK(&(pLock->m_Lock));
       NEOMinSleep();//�����˯�ߺ���
    }//while
}
//�˳���
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
//��תд
void MRSWLock_ReadToWrite(SNeoMultiReadSingleWriteLock *pLock)
{
    while(1)
    {
       MUTEXLOCK(&(pLock->m_Lock));//������������
       {
           //�ж��Ƿ��������д��Ȩ��
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
       NEOMinSleep();//�����˯�ߺ���
    }//while
MRSWLock_ReadToWrite_End:
    //�ȴ������Ķ����
    while(MRSWLock_GetRead(pLock))
    {
       NEOMinSleep();
    }
}
//���Ϻ�����Ӧ��c++��װ
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

//�����ϵ����������κ�bool�ͱ�������

CMRSWint::CMRSWint()
{
    m_Lock.EnableWrite();
    m_nValue=0;
    m_Lock.DisableWrite();
}
CMRSWint::~CMRSWint(){}

int CMRSWint::Get(void)//�õ�����
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
int CMRSWbool::Get(void)//�õ�����
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
//����������

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
            bRet=true;//��ʾ���óɹ�
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
//�߳̿�����

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
//��ѯ�߳�ά�ֱ�����ֵ
bool CThreadManager::ThreadContinue(void)
{
    return m_bThreadContinue.Get();
}
//��ü�������ֵ
int CThreadManager::GetThreadCount(void)
{
    return m_nThreadCount.Get();
}
//����һ���߳�ID��debug��
int CThreadManager::GetID(void)
{
    return m_nThreadID.Add()-1;
}

}
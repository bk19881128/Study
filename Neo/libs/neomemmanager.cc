//Ҫ��������ͷ�ļ��������ü�����,
//���뵼�����socket��ͷ�ļ�
//����debug�ļ��ĺ���
#include "neomemmanager.h"

//#ifdef WIN32_NEO
namespace NEOLIB {
//�ڴ��Ԫ�ع���,��Ҫ����֮���Ĺ���
CNEOMemStackToken::CNEOMemStackToken(int nBlockSize/*Ҫ����Ļ����ڴ���С*/,CNEOLowDebug *pDebug):
    m_pDebug(pDebug),m_Lock()
{
    //m_pDebug=pDebug;
    m_ulBlockSize=(unsigned long)nBlockSize;
    m_nBlockOutSide.Set(0);
    m_nBlockInSide.Set(0);
    m_Lock.EnableWrite();
    {
        m_pFirstSon=NULL;
        m_pNext=NULL;
    }
    m_Lock.DisableWrite();
}
CNEOMemStackToken::~CNEOMemStackToken()
{
    if(m_nBlockOutSide.Get())
    {
        m_pDebug->DebugToFile("Tony Mem Stack Lost %d*%d\n",m_ulBlockSize,m_nBlockOutSide.Get());
    }
    m_Lock.EnableWrite();//�ݹ�����
    {
        if(m_pFirstSon)
            DestroySon(m_pFirstSon);
        m_pFirstSon=NULL;
        if(m_pNext)
            delete m_pNext;
        m_pNext=NULL;
    }
    m_Lock.DisableWrite();
}

void * CNEOMemStackToken::Malloc(int nSize,/*ͳ������Ա�debug����*/CMRSWint &nAllBlockCount,CMRSWint &nMemUsed)//��������ڴ棬��������һ���������ڴ����ṹ
{
    void *pRet=NULL;//׼�����ص�ָ��
    SNeoMemroryBlockHead *pNew;
    if(NEO_MEM_BLOCK_SIZE(nSize)<m_ulBlockSize)
    {
        m_Lock.EnableWrite();
        {
            //�ж�������е��ڴ��
            if(!m_pFirstSon)
            {
                pNew=(SNeoMemroryBlockHead*)malloc(m_ulBlockSize);
                if(pNew)
                {
                    m_nBlockOutSide.Add();//����ڴ�����ϱ��ֳ�ȥ
                    nMemUsed.Add(m_ulBlockSize);//ͳ���ڴ���
                    //��ʼ���ڴ���Ϣ
                    pNew->m_ulBlockSize=m_ulBlockSize;
                    pNew->m_pNext=NULL;
                    pRet=NEO_MEM_BLOCK_DATA(pNew);
                    nAllBlockCount.Add();
                }//if
                //����ʧ���������淵��nRet=NULL
            }//if
            else//�п��е��ڴ��ֱ����ȡ
            {
                pNew=m_pFirstSon;
                m_pFirstSon=m_pFirstSon->m_pNext;
                pNew->m_pNext=NULL;//��ʹ�õ�ָ�븳ֵ
                pRet=NEO_MEM_BLOCK_DATA(pNew);
                m_nBlockOutSide.Add();
                m_nBlockInSide.Dec();
            }
        }
        m_Lock.DisableWrite();
    }
    else//�ߴ粻����Ҫ���
    {
        m_Lock.EnableWrite();
        {
            if(!m_pNext)
            {
                m_pNext=new CNEOMemStackToken(m_ulBlockSize*2,m_pDebug);
            }
        }
        m_Lock.DisableWrite();
        //ģʽת��
        m_Lock.AddRead();
        {
            //
            if(m_pNext)
                pRet=m_pNext->Malloc(nSize,nAllBlockCount,nMemUsed);
        }
        m_Lock.DecRead();
    }
    return pRet;
}
bool CNEOMemStackToken::Free(void *pPoint,bool bCloseFlag)
{
    bool bRet=false;//׼������ֵ�ı���
    SNeoMemroryBlockHead *pOld=NEO_MEM_BLOCK_HEAD(pPoint);//��ͷ���ڴ��ַ
    if(m_ulBlockSize==pOld->m_ulBlockSize)
    {
        if(NEO_MEM_STACK_BLOCK_MAX<=m_ulBlockSize)
        {
            free(pOld);
            m_nBlockOutSide.Dec();
        }
        else if(bCloseFlag)
        {
            //bCloseFlagΪ�棬���������ջ��ֱ���ͷ�
            free(pOld);
            m_nBlockOutSide.Dec();
        }//else if
        else
        {
            //����������֦ջ��׼�������ڴ�ջ
            m_Lock.EnableWrite();
            {
            //�¼�����ڴ����ڵ�һ����ԭ�е��ڴ�鱻���ӵ���һ��
                pOld->m_pNext=m_pFirstSon;
                m_pFirstSon=pOld;
            }
            m_Lock.DisableWrite();
            m_nBlockOutSide.Dec();
            m_nBlockInSide.Add();
        }//wlse
        bRet=true;
    }//if
    else
    {
        //�Լ�����
        m_Lock.AddRead();
        {
            //�˴��ݹ����
            if(m_pNext)
                bRet=m_pNext->Free(pPoint,bCloseFlag);
        }
        m_Lock.DecRead();
    }//else
    return bRet;
}

void CNEOMemStackToken::PrintStack(void)//��ӡ�������������һ����������
{
    //�ݹ麯����һ�δ�ӡ��֦���ϵ������ڴ浥Ԫ����
    if(m_nBlockInSide.Get()+m_nBlockOutSide.Get())
    {
        //��ֵ��ӡ����ֵ�����
        NEO_PRINTF("[%ld]stack:all=%d,out=%d,in=%d\n",m_ulBlockSize,
            m_nBlockInSide.Get()+m_nBlockOutSide.Get(),
            m_nBlockOutSide.Get(),m_nBlockInSide.Get());
    }
    m_Lock.AddRead();
    {
        if(m_pNext)
        {
            m_pNext->PrintStack();//�ݹ�
        }
    }
    m_Lock.DecRead();
}
//ϵͳ�Ƴ�ʱ�ݹ�����������ڴ��,�ݹ�������֮��
void CNEOMemStackToken::DestroySon(SNeoMemroryBlockHead *pSon)
{
    SNeoMemroryBlockHead *pObjNow=pSon;//�м����
    SNeoMemroryBlockHead *pOnjNext=NULL;
    while(1)
    {
        if(!pObjNow)
            break;
        pOnjNext=pObjNow->m_pNext;
        free(pObjNow);
        m_nBlockInSide.Dec();
        pObjNow=pOnjNext;
    }
}

//�ۺϣ�ʵ����֦���Ĺ���

CNEOMemStack::CNEOMemStack(CNEOLowDebug *pDebug):
    m_pMaxPoint(0),m_nAllBlockCount(),m_nMemoryUse(),
		m_CloseFlag(),m_pDebug(pDebug)
{
    m_CloseFlag.Set(false);//�رձ�־���
    //m_pDebug=pDebug;
    m_pMaxPoint.Set(0);
    m_nAllBlockCount.Set(0);
    m_nMemoryUse.Set(0);
    m_pHead=new CNEOMemStackToken(NEO_MEM_STACK_BLOCK_MIN,m_pDebug);// ������֦�ϵ�һ���ڵ�
    //�����ڼ���֦��ֻ����������
}
CNEOMemStack::~CNEOMemStack()
{
    //�˳�ʱ���㱨ʹ�õ����ָ��
    m_pDebug->DebugToFile("Mem Stack:Max Point=0x%p\n",m_pMaxPoint.Get());
    //�����֦�����ݹ�
    if(m_pHead)
    {
        delete m_pHead;
        m_pHead=NULL;
    }
}

//�ض���ָ��ָ����ڴ浥Ԫ�Ŀռ��С��������ָ����¿ռ�ĳߴ�
//����ɹ���������Ч����ָ�룬ָ���ض���Ĵ�С֮���¿ռ䣬ʧ�ܷ���null
//���Խ��Ϳռ�����ݱ��ݵ��¿ռ䣬���ⲻһ��ȡ�����¿ռ�Ĵ�С
void *CNEOMemStack::ReMalloc(void *pPoint,//��ָ��
    int nNewSize,//�µ����ݳߴ�
    bool bCopyOldDataFlag)//�Ƿ񱸷ݾ�����
{
    void *pRet=NULL;
    SNeoMemroryBlockHead *pOldToken=NULL;//�ɵ�p0
    int nOldLen=0;                       //�͵ó���
    if(0>=nNewSize)
    {
        m_pDebug->DebugToFile("CNEOMemStackToken::ReMalloc():ERROR!nNewSize=%d\n",nNewSize);
        goto CNEOMemStack_ReMalloc_Free_End;
    }
    pOldToken=NEO_MEM_BLOCK_HEAD(pPoint);
    nOldLen=pOldToken->m_ulBlockSize;
    //�Ƚ��µĳ������ڴ���ܳ��ȵĴ�С
    if(NEO_MEM_BLOCK_SIZE(nNewSize)<=(unsigned long)nOldLen)
    {
        pRet=pPoint;
        goto CNEOMemStack_ReMalloc_End;
    }//if
    //û�з���Ҫ�����������
    pRet=m_pHead->Malloc(nNewSize,m_nAllBlockCount,m_nMemoryUse);
    //��������
    if((pRet)&&(pPoint))
        if(bCopyOldDataFlag)
            memcpy(pRet,pPoint,nOldLen);
CNEOMemStack_ReMalloc_Free_End:
    m_pHead->Free(pPoint,m_CloseFlag.Get());
CNEOMemStack_ReMalloc_End:
    return pRet;
}
//Malloc��free�������ڴ����Ԫ�ķ���
void *CNEOMemStack::Malloc(int nSize)
{
    void *pRet=NULL;
    if(0>=nSize)
    {
        m_pDebug->DebugToFile("CNEOMemStack::Malloc():ERROR!nSize=%d\n",nSize);
        return pRet;
    }
    if(m_pHead)
    {
        //�ݹ����
        pRet=m_pHead->Malloc(nSize,m_nAllBlockCount,m_nMemoryUse);
        //ͳ������ָ��
        if(m_pMaxPoint.Get()<pRet)
            m_pMaxPoint.Set(pRet);
    }
    return pRet;
}
bool CNEOMemStack::Free(void *pPoint)
{
    bool bRet=false;
    if(m_pHead)//�ݹ������֦�ڵ��ϵ�Free
    {
        bRet=m_pHead->Free(pPoint,m_CloseFlag.Get());
    }
    return bRet;
}
//�ڲ���Ϣ�������
void CNEOMemStack::PrintStack(void)
{
    if(m_pHead)
        m_pHead->PrintStack();//�ݹ��ӡ�����������
}
void CNEOMemStack::PrintInfo(void)
{
    NEO_PRINTF("block=%d,use=%dbytes,biggest=%p\n",
        m_nAllBlockCount.Get(),
        m_nMemoryUse.Get(),
        m_pMaxPoint.Get());
}
//���ùرձ�־��Ϊ��ʱ����ϵͳfree
void CNEOMemStack::SetCloseFlag(bool bFlag)
{
    m_CloseFlag.Set(bFlag);
}


//�ڴ�ָ��ע��ģ��

CMemRegister::CMemRegister(CNEOLowDebug *pDebug):
    m_pDebug(pDebug)
{
    //m_pDebug=pDebug;
    m_nUseMax=0;
    m_pMaxPoint=NULL;
    m_nPointCount=0;
    //��null����ʾ�ṹ���Ƿ�ʹ��
    //��ʼ��
    int i=0;
    for(i=0;i<NEO_MEM_REGISTER_MAX;i++)
    {
        m_RegisterArray[i].m_pPoint=NULL;
        NEO_CLEAN_CHAR_BUFFER(m_RegisterArray[i].m_szInfo);
    }
}
CMemRegister::~CMemRegister()
{
    int i=0;
    m_Lock.Lock();
    {
        m_pDebug->DebugToFile("CMemRegister Max Register Point=0x%p\n",m_pMaxPoint);
        //�������ָ�룬��0ֵ����
        for(i=0;i<m_nUseMax;i++)
        {
            if(m_RegisterArray[i].m_pPoint)
            {
            m_pDebug->DebugToFile("******Mem Lost:[%p]-%s\n",
                m_RegisterArray[i].m_pPoint,
                m_RegisterArray[i].m_szInfo);
            }
        }
    }
    m_Lock.UnLock();
}

//�ڲ����ߣ���������
void CMemRegister::RegisterCopy(SNEOMemRegister *pDest//Ŀ�Ŀ���ָ��
    ,void *pPoint                       //��������ָ��
    ,const char *szInfo)                      //������������
{
    pDest->m_pPoint=pPoint;
    if(szInfo)
    {
        //����ĺ���
        SafeStrcpy(pDest->m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
    }
    else
    {
        NEO_CLEAN_CHAR_BUFFER(szInfo);
    }
}
//���һ��ָ�뼰��˵��
void CMemRegister::Add(void *pPoint,const char *szInfo)
{
    int i=0;
    m_Lock.Lock();
    {
        //�������ָ��
        if(pPoint>m_pMaxPoint)
            m_pMaxPoint=pPoint;
        //����Ѱ��DEl���µĿ�������
        for(i=0;i<m_nUseMax;i++)
        {
            //�ṹ���ص�ָ��ΪNULL���ʾδʹ��
            if(!m_RegisterArray[i].m_pPoint)
            {
                //ͳ��ʹ�õ�ָ������
                m_nPointCount++;
                //������Ϣ
                RegisterCopy(&(m_RegisterArray[i]),pPoint,szInfo);
                goto CMemRegister_Add_End;
            }
        }//for
        //�޿�����������ĩβ����
        if(NEO_MEM_REGISTER_MAX<=m_nUseMax)//�鿴�Ƿ�Խ��
        {
            m_pDebug->DebugToFile("***ERROR***CMemRegister is full\n");
            goto CMemRegister_Add_End;
        }
        //ע�ᵽĩβ
        RegisterCopy(&(m_RegisterArray[m_nUseMax]),pPoint,szInfo);
        m_nPointCount++;
        m_nUseMax++;
        
    }
CMemRegister_Add_End:
    m_Lock.UnLock();
}
//ɾ��һ��ָ�루�ڴ汻�ͷţ�ʧЧ��
void CMemRegister::Del(void *pPoint)
{
    int i=0;
    m_Lock.Lock();
    {
        //Ѱ�������ڴ�����
        for(i=0;i<m_nUseMax;i++)
        {
            if(pPoint==m_RegisterArray[i].m_pPoint)
            {
                m_nPointCount--;
                m_RegisterArray[i].m_pPoint=NULL;
                NEO_CLEAN_CHAR_BUFFER(m_RegisterArray[i].m_szInfo);
                goto CMemRegister_Del_End;
            }
        }
    }
CMemRegister_Del_End:
    m_Lock.UnLock();
}
//remalloc��ʱ�����ָ��
void CMemRegister::Modeify(void *pOld,void *pNew)
{
    int i=0;
    m_Lock.Lock();
    {
        if(pOld>m_pMaxPoint)
            m_pMaxPoint=pOld;
        //Ѱ�������ڴ�����
        for(i=0;i<m_nUseMax;i++)
        {
            if(pOld==m_RegisterArray[i].m_pPoint)
            {
                //���޸�ָ�룬���޸�˵��
                m_RegisterArray[i].m_pPoint=pNew;
                goto CMemRegister_Modeify_End;
            }
        }
    }
    //��ָ�벻���ڱ���
    m_pDebug->DebugToFile("***ERROR***CMemRegister����modeify(): I can't found point!\n");
CMemRegister_Modeify_End:
    m_Lock.UnLock();
}
//��ӡ��Ϣ
void CMemRegister::PrintInfo(void)
{
    m_Lock.Lock();
    {
        NEO_PRINTF("mem pool:%d/%d,biggest=%p\n",
            m_nPointCount,
            m_nUseMax+1,
            m_pMaxPoint);
    }
    m_Lock.UnLock();
}

//socketע��ģ��
inline bool SocketIsOK(WIN_LINUX_SOCKET nSocket)
{
    //if(0>nSocket) return false;
    //if(nSocket>65535)return false;
    if(WIN_LINUX_InvalidSocket==nSocket)
        return false;
    return true;
}

CSocketRegister::CSocketRegister(CNEOLowDebug *pDebug):
    m_pDebug(pDebug)
{
    m_nMaxSocket=WIN_LINUX_InvalidSocket;
    m_nSocketUseCount=0;
    m_nUseMax=0;
    int i=0;
    for(i=0;i<NEO_MEM_REGISTER_MAX;i++)
    {
        m_RegisterArray[i].m_nSocket=WIN_LINUX_InvalidSocket;
        NEO_CLEAN_CHAR_BUFFER(m_RegisterArray[i].m_szInfo);
    }
}
CSocketRegister::~CSocketRegister()
{
    int i=0;
    m_Lock.Lock();
    {
        m_pDebug->DebugToFile("CSocketRegister.Max Socket Count=%d,Max Socket=%d\nSSSS",
            m_nUseMax,m_nMaxSocket);
        for(i=0;i<m_nUseMax;i++)
        {
            if(SocketIsOK(m_RegisterArray[i].m_nSocket))
            {
                //���������socket��ʹ�ã��򱨾������ͷ�
                m_pDebug->DebugToFile("***Socket Lost:[%d]-%s\n",
                    m_RegisterArray[i].m_nSocket,
                    m_RegisterArray[i].m_szInfo);
                WIN_LINUX_CloseSocket(m_RegisterArray[i].m_nSocket);
            }
        }
    }
    m_Lock.UnLock();
}

void CSocketRegister::Add(WIN_LINUX_SOCKET s,char *szInfo)
{
    int i=0;
    m_Lock.Lock();
    {
        //ͳ������socket
        if(!SocketIsOK(m_nMaxSocket))
            m_nMaxSocket=s;
        else if(s>m_nMaxSocket)
            m_nMaxSocket=s;
        //����ͼ�޸�
        for(i=0;i<m_nUseMax;i++)
        { 
            if(m_RegisterArray[i].m_nSocket==s)
            {
                if(szInfo)
                    SafeStrcpy(m_RegisterArray[i].m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
                //�޸Ĳ�����
                goto CSocketRegister_Add_End;
            }
        }//for
        //����ͼ���룬Ѱ�����п��е�socket
        for(i=0;i<m_nUseMax;i++)
        {
            if(!SocketIsOK(m_nMaxSocket))
            {
                m_RegisterArray[i].m_nSocket=s;
                if(szInfo)
                    SafeStrcpy(m_RegisterArray[i].m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
                m_nSocketUseCount++;
                goto CSocketRegister_Add_End;
            }
        }//for
        //���׷�ӵ�ĩβ
        if(NEO_MEM_REGISTER_MAX>m_nUseMax)
        {
            m_RegisterArray[i].m_nSocket=s;
            if(szInfo)
                    SafeStrcpy(m_RegisterArray[i].m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
            m_nUseMax++;
            m_nSocketUseCount++;
        }
        else//ע�������ˣ�����Ҫ���󻺴�
        {
            m_pDebug->DebugToFile("CSocketRegister ::Add():Pool is full!\n");
        }
    }
CSocketRegister_Add_End:
    m_Lock.UnLock();
}
bool CSocketRegister::Del(WIN_LINUX_SOCKET s)
{
    bool bRet=false;
    int i=0;
    m_Lock.Lock();
    {
        for(i=0;i<m_nUseMax;i++)
        {
            if(m_RegisterArray[i].m_nSocket==s)
            {
            m_RegisterArray[i].m_nSocket=WIN_LINUX_InvalidSocket;
            NEO_CLEAN_CHAR_BUFFER(m_RegisterArray[i].m_szInfo);
            m_nSocketUseCount--;
            bRet=true;
            goto CSocketRegister_Del_End;
            }
        }
    }
CSocketRegister_Del_End:
    m_Lock.UnLock();
    return bRet;
}
void CSocketRegister::PrintInfo(void)
{
    m_Lock.Lock();
    {
        NEO_PRINTF("socket pool:%d/%d,biggest=%d\n",
            m_nSocketUseCount,
            m_nUseMax+1,
            m_nMaxSocket);
    }
    m_Lock.UnLock();
}

//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//�ڴ����

CNEOMemPoolWithLock::CNEOMemPoolWithLock(CNEOLowDebug *pDebug,
    bool bOpenResgisterFlag)://����ע�Ṧ�ܵı��
    m_pDebug(pDebug)
{
    //m_pDebug=pDebug;
    m_pMemPool=new CNEOMemStack(m_pDebug);
    m_pRegister=NULL;
    m_pSocketRegister=NULL;

    ///////////////
    if(bOpenResgisterFlag)
    {
        //���ݱ�־�������Ƿ�ʵ��������ע�����
        m_pRegister=new CMemRegister(m_pDebug);
        m_pSocketRegister=new CSocketRegister(m_pDebug);
    }
    //��ӡ�ڴ����ȷ������־
    m_pDebug->DebugToFile("NEO Mem Pool Open,register flag=%d\n",bOpenResgisterFlag);
}
CNEOMemPoolWithLock::~CNEOMemPoolWithLock()
{
    //�ݻٸ����Ӷ���
    if(m_pRegister)
    {
        delete m_pRegister;
        m_pRegister=NULL;
    }
    if(m_pSocketRegister)
    {
        delete m_pSocketRegister;
        m_pSocketRegister=NULL;
    }
    if(m_pMemPool)
    {
        delete m_pMemPool;
        m_pMemPool=NULL;
    }
    //��ӡ��ȷ�����ı�־
        m_pDebug->DebugToFile("NEO Mem Pool Close\n");
}
//ָ�����

void CNEOMemPoolWithLock::Register(void *pPoint,const char *szInfo)
{
    if(m_pRegister)
    {
        m_pRegister->Add(pPoint,szInfo);
    }
}
void CNEOMemPoolWithLock::UnRegister(void *pPoint)
{
    if(m_pRegister)
    {
        m_pRegister->Del(pPoint);
    }
}
//socket   ����

void CNEOMemPoolWithLock::RegisterSocket(WIN_LINUX_SOCKET s,char *szInfo)
{
    if(m_pSocketRegister)
    {
        m_pSocketRegister->Add(s,szInfo);
    }
}
//��ע��ӹر�
void CNEOMemPoolWithLock::CloseSocket(WIN_LINUX_SOCKET &s)
{
    if(SocketIsOK(s))
    {
        if(m_pSocketRegister)
        {
            if(!m_pSocketRegister->Del(s))
            {
            m_pDebug->DebugToFile("CNEOMemPoolWithLock::CloseSocket():Socket%d is not registed !But I have close it yet!\n",s);
            }
            WIN_LINUX_CloseSocket(s);
            s=WIN_LINUX_InvalidSocket;
        }
    }
}
//��������
//���ñ�־�������е�free��ֱ��free, ���ڱ�����stack��
void CNEOMemPoolWithLock::SetCloseFlag(bool bFlag)
{
    if(m_pMemPool)
        m_pMemPool->SetCloseFlag(bFlag);
}
//���·���ָ��Ŀռ䣬Ĭ�Ͽ���ԭʼ���ݵ��¿ռ�
void *CNEOMemPoolWithLock::ReMalloc(void *pPoint,int nNewSize,bool bCopyOldDataFlag)
{
    void *pRet=NULL;
    if(m_pMemPool)
    {
        //�����ڴ�ջ�ض����ڴ������С
        pRet=m_pMemPool->ReMalloc(pPoint,nNewSize,bCopyOldDataFlag);
        if(m_pRegister)
        {
            if(pRet)
                m_pRegister->Modeify(pPoint,pRet);
            else//ʧ�ܣ�������ָ���Ѿ��ݻ٣�ʵ�ַ�ע��
                m_pRegister->Del(pPoint);
        }
    }
    return pRet;
}
//����һ���ڴ�
void *CNEOMemPoolWithLock::Malloc(int nSize,const char *szInfo)
{
    void *pRet=NULL;
    if(m_pMemPool)
    {
        pRet=m_pMemPool->Malloc(nSize);
        if(pRet)//���ָ����Ч����ע��
            Register(pRet,szInfo);
    }
    return pRet;
}
void CNEOMemPoolWithLock::Free(void *pBlock)
{
    if(m_pMemPool)
        m_pMemPool->Free(pBlock);
    UnRegister(pBlock);
}
//��ʾ�����ڴ���������
void CNEOMemPoolWithLock::PrintTree(void)
{
    if(m_pMemPool)
        m_pMemPool->PrintStack();
}
//�ؼ���Ϣ��ʾ
void CNEOMemPoolWithLock::PrintInfo(void)
{
    /////////////////
    /////////////////
    if(m_pRegister)
        m_pRegister->PrintInfo();
    if(m_pMemPool)
        m_pMemPool->PrintInfo();
}

//#else
//#endif
}
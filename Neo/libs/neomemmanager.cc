//要导入锁的头文件，以引用加锁类,
//必须导入包涵socket的头文件
//导入debug文见的函数
#include "neomemmanager.h"

//#ifdef WIN32_NEO
namespace NEOLIB {
//内存块元素管理,主要是右之树的管理
CNEOMemStackToken::CNEOMemStackToken(int nBlockSize/*要管理的基本内存块大小*/,CNEOLowDebug *pDebug):
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
    m_Lock.EnableWrite();//递归销毁
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

void * CNEOMemStackToken::Malloc(int nSize,/*统计设计以便debug调用*/CMRSWint &nAllBlockCount,CMRSWint &nMemUsed)//负责分配内存，根据需求一步步创建内存数结构
{
    void *pRet=NULL;//准备返回的指针
    SNeoMemroryBlockHead *pNew;
    if(NEO_MEM_BLOCK_SIZE(nSize)<m_ulBlockSize)
    {
        m_Lock.EnableWrite();
        {
            //判断有误空闲的内存块
            if(!m_pFirstSon)
            {
                pNew=(SNeoMemroryBlockHead*)malloc(m_ulBlockSize);
                if(pNew)
                {
                    m_nBlockOutSide.Add();//这个内存块马上被分出去
                    nMemUsed.Add(m_ulBlockSize);//统计内存数
                    //初始化内存信息
                    pNew->m_ulBlockSize=m_ulBlockSize;
                    pNew->m_pNext=NULL;
                    pRet=NEO_MEM_BLOCK_DATA(pNew);
                    nAllBlockCount.Add();
                }//if
                //创建失败则在下面返回nRet=NULL
            }//if
            else//有空闲的内存块直接提取
            {
                pNew=m_pFirstSon;
                m_pFirstSon=m_pFirstSon->m_pNext;
                pNew->m_pNext=NULL;//不使用的指针赋值
                pRet=NEO_MEM_BLOCK_DATA(pNew);
                m_nBlockOutSide.Add();
                m_nBlockInSide.Dec();
            }
        }
        m_Lock.DisableWrite();
    }
    else//尺寸不符合要求的
    {
        m_Lock.EnableWrite();
        {
            if(!m_pNext)
            {
                m_pNext=new CNEOMemStackToken(m_ulBlockSize*2,m_pDebug);
            }
        }
        m_Lock.DisableWrite();
        //模式转换
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
    bool bRet=false;//准备返回值的变量
    SNeoMemroryBlockHead *pOld=NEO_MEM_BLOCK_HEAD(pPoint);//加头的内存地址
    if(m_ulBlockSize==pOld->m_ulBlockSize)
    {
        if(NEO_MEM_STACK_BLOCK_MAX<=m_ulBlockSize)
        {
            free(pOld);
            m_nBlockOutSide.Dec();
        }
        else if(bCloseFlag)
        {
            //bCloseFlag为真，不再推入堆栈，直接释放
            free(pOld);
            m_nBlockOutSide.Dec();
        }//else if
        else
        {
            //正常推入右枝栈，准备重用内存栈
            m_Lock.EnableWrite();
            {
            //新加入的内存块放在第一个，原有的内存块被连接到下一个
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
        //自己解释
        m_Lock.AddRead();
        {
            //此处递归操作
            if(m_pNext)
                bRet=m_pNext->Free(pPoint,bCloseFlag);
        }
        m_Lock.DecRead();
    }//else
    return bRet;
}

void CNEOMemStackToken::PrintStack(void)//打印函数帮助输出是一个辅助函数
{
    //递归函数，一次打印左枝上上的所有内存单元内容
    if(m_nBlockInSide.Get()+m_nBlockOutSide.Get())
    {
        //有值打印，无值不输出
        NEO_PRINTF("[%ld]stack:all=%d,out=%d,in=%d\n",m_ulBlockSize,
            m_nBlockInSide.Get()+m_nBlockOutSide.Get(),
            m_nBlockOutSide.Get(),m_nBlockInSide.Get());
    }
    m_Lock.AddRead();
    {
        if(m_pNext)
        {
            m_pNext->PrintStack();//递归
        }
    }
    m_Lock.DecRead();
}
//系统推出时递归销毁申请的内存块,递归销毁右之树
void CNEOMemStackToken::DestroySon(SNeoMemroryBlockHead *pSon)
{
    SNeoMemroryBlockHead *pObjNow=pSon;//中间变量
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

//聚合，实现左枝树的管理

CNEOMemStack::CNEOMemStack(CNEOLowDebug *pDebug):
    m_pMaxPoint(0),m_nAllBlockCount(),m_nMemoryUse(),
		m_CloseFlag(),m_pDebug(pDebug)
{
    m_CloseFlag.Set(false);//关闭标志清空
    //m_pDebug=pDebug;
    m_pMaxPoint.Set(0);
    m_nAllBlockCount.Set(0);
    m_nMemoryUse.Set(0);
    m_pHead=new CNEOMemStackToken(NEO_MEM_STACK_BLOCK_MIN,m_pDebug);// 构建左枝上第一个节点
    //运行期间左枝上只创建不销毁
}
CNEOMemStack::~CNEOMemStack()
{
    //退出时。汇报使用的最大指针
    m_pDebug->DebugToFile("Mem Stack:Max Point=0x%p\n",m_pMaxPoint.Get());
    //清除左枝树，递归
    if(m_pHead)
    {
        delete m_pHead;
        m_pHead=NULL;
    }
}

//重定义指针指向的内存单元的空间大小，给出旧指针和新空间的尺寸
//如果成功，返回有效地新指针，指向重定义的大小之后新空间，失败返回null
//可以将就空间的数据备份到新空间，但这不一定取决于新空间的大小
void *CNEOMemStack::ReMalloc(void *pPoint,//旧指针
    int nNewSize,//新的数据尺寸
    bool bCopyOldDataFlag)//是否备份旧数据
{
    void *pRet=NULL;
    SNeoMemroryBlockHead *pOldToken=NULL;//旧的p0
    int nOldLen=0;                       //就得长度
    if(0>=nNewSize)
    {
        m_pDebug->DebugToFile("CNEOMemStackToken::ReMalloc():ERROR!nNewSize=%d\n",nNewSize);
        goto CNEOMemStack_ReMalloc_Free_End;
    }
    pOldToken=NEO_MEM_BLOCK_HEAD(pPoint);
    nOldLen=pOldToken->m_ulBlockSize;
    //比较新的长度与内存块总长度的大小
    if(NEO_MEM_BLOCK_SIZE(nNewSize)<=(unsigned long)nOldLen)
    {
        pRet=pPoint;
        goto CNEOMemStack_ReMalloc_End;
    }//if
    //没有符合要求的重新申请
    pRet=m_pHead->Malloc(nNewSize,m_nAllBlockCount,m_nMemoryUse);
    //拷贝数据
    if((pRet)&&(pPoint))
        if(bCopyOldDataFlag)
            memcpy(pRet,pPoint,nOldLen);
CNEOMemStack_ReMalloc_Free_End:
    m_pHead->Free(pPoint,m_CloseFlag.Get());
CNEOMemStack_ReMalloc_End:
    return pRet;
}
//Malloc和free，调用内存管理单元的方法
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
        //递归分配
        pRet=m_pHead->Malloc(nSize,m_nAllBlockCount,m_nMemoryUse);
        //统计最大的指针
        if(m_pMaxPoint.Get()<pRet)
            m_pMaxPoint.Set(pRet);
    }
    return pRet;
}
bool CNEOMemStack::Free(void *pPoint)
{
    bool bRet=false;
    if(m_pHead)//递归调用左枝节点上的Free
    {
        bRet=m_pHead->Free(pPoint,m_CloseFlag.Get());
    }
    return bRet;
}
//内部信息输出函数
void CNEOMemStack::PrintStack(void)
{
    if(m_pHead)
        m_pHead->PrintStack();//递归打印整个树的情况
}
void CNEOMemStack::PrintInfo(void)
{
    NEO_PRINTF("block=%d,use=%dbytes,biggest=%p\n",
        m_nAllBlockCount.Get(),
        m_nMemoryUse.Get(),
        m_pMaxPoint.Get());
}
//设置关闭标志，为真时调用系统free
void CNEOMemStack::SetCloseFlag(bool bFlag)
{
    m_CloseFlag.Set(bFlag);
}


//内存指针注册模块

CMemRegister::CMemRegister(CNEOLowDebug *pDebug):
    m_pDebug(pDebug)
{
    //m_pDebug=pDebug;
    m_nUseMax=0;
    m_pMaxPoint=NULL;
    m_nPointCount=0;
    //以null来表示结构体是否被使用
    //初始化
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
        //检查所有指针，非0值报警
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

//内部工具，拷贝数据
void CMemRegister::RegisterCopy(SNEOMemRegister *pDest//目的拷贝指针
    ,void *pPoint                       //待拷贝的指针
    ,const char *szInfo)                      //待拷贝的描述
{
    pDest->m_pPoint=pPoint;
    if(szInfo)
    {
        //引入的函数
        SafeStrcpy(pDest->m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
    }
    else
    {
        NEO_CLEAN_CHAR_BUFFER(szInfo);
    }
}
//添加一个指针及其说明
void CMemRegister::Add(void *pPoint,const char *szInfo)
{
    int i=0;
    m_Lock.Lock();
    {
        //更新最大指针
        if(pPoint>m_pMaxPoint)
            m_pMaxPoint=pPoint;
        //遍历寻找DEl导致的空闲区域
        for(i=0;i<m_nUseMax;i++)
        {
            //结构体重的指针为NULL则表示未使用
            if(!m_RegisterArray[i].m_pPoint)
            {
                //统计使用的指针总数
                m_nPointCount++;
                //传入信息
                RegisterCopy(&(m_RegisterArray[i]),pPoint,szInfo);
                goto CMemRegister_Add_End;
            }
        }//for
        //无空闲区域则在末尾增减
        if(NEO_MEM_REGISTER_MAX<=m_nUseMax)//查看是否越界
        {
            m_pDebug->DebugToFile("***ERROR***CMemRegister is full\n");
            goto CMemRegister_Add_End;
        }
        //注册到末尾
        RegisterCopy(&(m_RegisterArray[m_nUseMax]),pPoint,szInfo);
        m_nPointCount++;
        m_nUseMax++;
        
    }
CMemRegister_Add_End:
    m_Lock.UnLock();
}
//删除一个指针（内存被释放，失效）
void CMemRegister::Del(void *pPoint)
{
    int i=0;
    m_Lock.Lock();
    {
        //寻找再用内存区域
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
//remalloc的时候更新指针
void CMemRegister::Modeify(void *pOld,void *pNew)
{
    int i=0;
    m_Lock.Lock();
    {
        if(pOld>m_pMaxPoint)
            m_pMaxPoint=pOld;
        //寻找再用内存区域
        for(i=0;i<m_nUseMax;i++)
        {
            if(pOld==m_RegisterArray[i].m_pPoint)
            {
                //仅修改指针，不修改说明
                m_RegisterArray[i].m_pPoint=pNew;
                goto CMemRegister_Modeify_End;
            }
        }
    }
    //若指针不存在报错
    m_pDebug->DebugToFile("***ERROR***CMemRegister：：modeify(): I can't found point!\n");
CMemRegister_Modeify_End:
    m_Lock.UnLock();
}
//打印信息
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

//socket注册模块
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
                //如果发现有socket在使用，则报警，并释放
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
        //统计最大的socket
        if(!SocketIsOK(m_nMaxSocket))
            m_nMaxSocket=s;
        else if(s>m_nMaxSocket)
            m_nMaxSocket=s;
        //先试图修改
        for(i=0;i<m_nUseMax;i++)
        { 
            if(m_RegisterArray[i].m_nSocket==s)
            {
                if(szInfo)
                    SafeStrcpy(m_RegisterArray[i].m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
                //修改不增加
                goto CSocketRegister_Add_End;
            }
        }//for
        //再试图插入，寻找其中空闲的socket
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
        //最后追加到末尾
        if(NEO_MEM_REGISTER_MAX>m_nUseMax)
        {
            m_RegisterArray[i].m_nSocket=s;
            if(szInfo)
                    SafeStrcpy(m_RegisterArray[i].m_szInfo,szInfo,NEO_MEM_BLOCK_INFO_MAX_SIZE);
            m_nUseMax++;
            m_nSocketUseCount++;
        }
        else//注册区满了，必须要扩大缓存
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
//内存池类

CNEOMemPoolWithLock::CNEOMemPoolWithLock(CNEOLowDebug *pDebug,
    bool bOpenResgisterFlag)://开启注册功能的标记
    m_pDebug(pDebug)
{
    //m_pDebug=pDebug;
    m_pMemPool=new CNEOMemStack(m_pDebug);
    m_pRegister=NULL;
    m_pSocketRegister=NULL;

    ///////////////
    if(bOpenResgisterFlag)
    {
        //根据标志，决定是否实例化各个注册对象
        m_pRegister=new CMemRegister(m_pDebug);
        m_pSocketRegister=new CSocketRegister(m_pDebug);
    }
    //打印内存池正确启动标志
    m_pDebug->DebugToFile("NEO Mem Pool Open,register flag=%d\n",bOpenResgisterFlag);
}
CNEOMemPoolWithLock::~CNEOMemPoolWithLock()
{
    //摧毁各个子对象
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
    //打印正确结束的标志
        m_pDebug->DebugToFile("NEO Mem Pool Close\n");
}
//指针管理

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
//socket   管理

void CNEOMemPoolWithLock::RegisterSocket(WIN_LINUX_SOCKET s,char *szInfo)
{
    if(m_pSocketRegister)
    {
        m_pSocketRegister->Add(s,szInfo);
    }
}
//反注册加关闭
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
//公共管理
//设置标志，让所有的free都直接free, 不在保留在stack中
void CNEOMemPoolWithLock::SetCloseFlag(bool bFlag)
{
    if(m_pMemPool)
        m_pMemPool->SetCloseFlag(bFlag);
}
//重新分配指针的空间，默认拷贝原始数据到新空间
void *CNEOMemPoolWithLock::ReMalloc(void *pPoint,int nNewSize,bool bCopyOldDataFlag)
{
    void *pRet=NULL;
    if(m_pMemPool)
    {
        //调用内存栈重定义内存区域大小
        pRet=m_pMemPool->ReMalloc(pPoint,nNewSize,bCopyOldDataFlag);
        if(m_pRegister)
        {
            if(pRet)
                m_pRegister->Modeify(pPoint,pRet);
            else//失败，由于老指针已经摧毁，实现反注册
                m_pRegister->Del(pPoint);
        }
    }
    return pRet;
}
//分配一块内存
void *CNEOMemPoolWithLock::Malloc(int nSize,const char *szInfo)
{
    void *pRet=NULL;
    if(m_pMemPool)
    {
        pRet=m_pMemPool->Malloc(nSize);
        if(pRet)//如果指针有效，则注册
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
//显示整棵内存树的内容
void CNEOMemPoolWithLock::PrintTree(void)
{
    if(m_pMemPool)
        m_pMemPool->PrintStack();
}
//关键信息显示
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
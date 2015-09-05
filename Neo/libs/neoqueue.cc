//要去导入内存池的类


//#ifdef WIN32_NEO
#include "neoindex.h" 

#include "neomemmanager.h"     
#include "neodebug.h"      
#include "neolock.h"         
#include "neosafefunc.h"
#include "neoqueue.h"

namespace NEOLIB {
//主要为了实现报文拼接和信令构建（依赖内存池）
//动态buffer

CNEODynamicBuffer::CNEODynamicBuffer(CNEOMemPoolWithLock *pMemPool)
    :m_pMemPool(pMemPool),m_pData(NULL),m_nDataLen(0)
{
    //m_pMemPool=pMemPool;
    //m_pData=NULL;
    //m_nDataLen=0;
}
CNEODynamicBuffer::~CNEODynamicBuffer()
{
    SetSize(0);
}

///////////////////////////////
//尺寸设置函数
bool CNEODynamicBuffer::SetSize(int nSize) //设置新的大小
{
    if(!m_pMemPool)
        return false;
    m_nDataLen=nSize;
    if(0==m_nDataLen)
    {
        if(m_pData)
        {
            m_pMemPool->Free(m_pData);//内存池释放
            m_pData=NULL;             //释放后立即赋初值
        }
        return true;                 //这也是操作成功
    }
    //新设置的缓冲期长度，一定不为0
    if(!m_pData)                    //若原指针为空，表示没有数据，需要Malloc
    {
        m_pData=(char *)m_pMemPool->Malloc(m_nDataLen,"CNEODynamicBuffer::m_pData");//申请的内存块长度
        if(!m_pData)//判断申请是否成功
        {
            m_nDataLen=0;
            return false;
        }
        else 
            return true;
    }//if
    else //已经有一个内存区的情况
    {
        m_pData=(char *)m_pMemPool->ReMalloc(m_pData,m_nDataLen);
        if(!m_pData)
        {
            m_nDataLen=0;
            return false;
        }
        else
            return true;
    }//else
}
/*该函数会向mempool申请额外的空间，但是会立即释放
  主要原因是避免memcpy 前向拷贝带来的错误*/
bool CNEODynamicBuffer::InsertSpaceToHead(int nAddBytes)//在前面插入空白
{
    bool bRet=false;
    int nNewSize=0;                   //新的 空间变量大小
    char *pBuffer=NULL;               //二进制缓冲区指针
    if(!m_pMemPool)
        goto CNEODynamicBuffer_InsertSpaceToHead_End;
    nNewSize=m_nDataLen+nAddBytes;    //求得新的长度
    //请注意这里，申请一个临时缓冲区，其长度等于原长度+增加的长度
    pBuffer=(char *)m_pMemPool->Malloc(nNewSize,"CNEODynamicBuffer::InsertSpaceToHead();pBuffer");
    if(!pBuffer)                      //申请失败
        goto CNEODynamicBuffer_InsertSpaceToHead_End;
    //如缓冲区为空，则不拷贝
    if((m_pData)&&(m_nDataLen))
    {
        //将原有缓冲区内容拷贝到临时缓冲区后半部分，与新增加的字节构成一个整体
        memcpy(pBuffer+nAddBytes,m_pData,m_nDataLen);
    }
    bRet=BinCopyFrom(pBuffer,nNewSize);
    
CNEODynamicBuffer_InsertSpaceToHead_End:
    //释放临时缓冲区
    if(pBuffer)
    {
        m_pMemPool->Free(pBuffer);
        pBuffer=NULL;
    }
    return bRet;
}
bool CNEODynamicBuffer::AddSpaceToTail(int nAddBytes)//在后面插入空白
{
    return SetSize(m_nDataLen+nAddBytes);
}
void CNEODynamicBuffer::CutHead(int nBytes)// 从前面剪掉一段数据
{
    if(m_nDataLen<=nBytes)
        SetSize(0);
    else
    {
        //从后向前移动拷贝
        memcpy(m_pData,m_pData+nBytes,m_nDataLen-nBytes);
        m_nDataLen-=nBytes;
    }
}
void CNEODynamicBuffer::CutTail(int nBytes)//从后面剪掉一段数据
{
    if(m_nDataLen<=nBytes)
        SetSize(0);
    else
        m_nDataLen-=nBytes;
}
////////////////////////////////////////
//数值转换函数
bool CNEODynamicBuffer::SetInt(int n)//讲一个整数以二进制方式拷贝到缓冲区带网络字节序
{
    int nSave=htonl(n);
    //拷贝到本地缓冲区
    return BinCopyFrom((char *)&nSave,sizeof(int));
}


int CNEODynamicBuffer::GetInt(void)//以整数方式获得缓冲区的数值
{
    if(!m_pData)
        return 0;
    if((sizeof(int)>(size_t)m_nDataLen))
        return 0;
    return ntohl(*(int *)m_pData);//用这种方式取得前四字节
}
bool CNEODynamicBuffer::SetShort(short n)//讲一个短整数以二进制方式拷贝到缓冲区带网络字节序
{
    short sSave=htons(n);
    return BinCopyFrom((char *)&sSave,sizeof(short));
}

short CNEODynamicBuffer::GetShort(void)//端口号
{
    if(!m_pData)
        return 0;
    if(sizeof(short)>(size_t)m_nDataLen)
        return 0;
    return ntohs(*(short *)m_pData);
}
bool CNEODynamicBuffer::SetChar(char n)//将一个字节以二进制方式拷贝到缓冲区
{
    return BinCopyFrom(&n,sizeof(char));
}
char CNEODynamicBuffer::GetChar(void)//以字节方式获得缓冲区的数值
{
    if(!m_pData)
        return 0;
    if(sizeof(char)>(size_t)m_nDataLen)
        return 0;
    return *(char *)m_pData;
}
//////////////////////////////////////////
//二进制数据追加函数
//追加数据到最后，返回新的长度
int CNEODynamicBuffer::AddData(char *szData,int nDataLen)
{
    if((!m_pData)||(0>=m_nDataLen))
    {
        return BinCopyFrom(szData,nDataLen);
    }
    int nOldSize=m_nDataLen;
    int nNewSize=m_nDataLen+nDataLen;
    if(!SetSize(nNewSize))
        return 0;
    memcpy(m_pData+nOldSize,szData,nDataLen);
    return m_nDataLen;
}
//插入数据的最前面，返回新的数据长度
int CNEODynamicBuffer::InsertDataToHead(char *szData,int nDataLen)
{
    if((!m_pData)||(0>=m_nDataLen))
    {
        return BinCopyFrom(szData,nDataLen);
    }
    if(!InsertSpaceToHead(nDataLen))
        return 0;
    memcpy(m_pData,szData,nDataLen);
    return m_nDataLen;
}
/////////////////////////////////////////
//二进制数据拷贝函数
//拷贝到一块目标缓冲区域，受传入的缓冲区长度限制
int CNEODynamicBuffer::BinCopyTo(char *szBuffer,int nBufferSize)
{
    if(!m_pData)
        return 0;
    if(!m_nDataLen)
        return 0;
    if(nBufferSize<m_nDataLen)
        return 0;
    memcpy(szBuffer,m_pData,m_nDataLen);
    return m_nDataLen;
}
//从一块来源缓冲区拷贝数据到本对象中
int CNEODynamicBuffer::BinCopyFrom(char *szData,int nDataLen)
{
    if((!szData)||(0>=nDataLen))
    {
        SetSize(0);
        return 0;
    }
    if(!SetSize(nDataLen))
        return 0;
    memcpy(m_pData,szData,m_nDataLen);
    return m_nDataLen;
}

//从另外一个Buffer对象拷贝数据到本对象
int CNEODynamicBuffer::BinCopyFrom(CNEODynamicBuffer *pBuffer)
{
    return BinCopyFrom(pBuffer->m_pData,pBuffer->m_nDataLen);
}
/////////////////////////////////////////////
//文本数据拷贝构建函数
int CNEODynamicBuffer::StrCopyFrom(char *szString)
{
    int n=strlen(szString);
    n++;//流出“\0”
    return BinCopyFrom(szString,n);
}
int CNEODynamicBuffer::Printf(const char *szFormat,...)
{
    char szBuf[NEO_BUFFER_STRING_MAX];
    int nListCount=0;
    va_list pArgList;
    va_start(pArgList,szFormat);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,NEO_BUFFER_STRING_MAX-nListCount,szFormat,pArgList);
    va_end(pArgList);
    if(nListCount>(NEO_BUFFER_STRING_MAX-1))
        nListCount=NEO_BUFFER_STRING_MAX-1;
    *(szBuf+nListCount)='\0';
    return StrCopyFrom(szBuf);
}
/////////////////////////////////////////////
//数据比较函数
int CNEODynamicBuffer::memcmp(char *szData,int nDataLen)
{
    if(!m_pData)
        return -1;
    if(!m_nDataLen)
        return -1;
    if(!szData)
        return -1;
    if(m_nDataLen<nDataLen)
        return -1;
    int nRet=::memcmp(m_pData,szData,nDataLen);
    return nRet;
}
int CNEODynamicBuffer::strcmp(char *szString)
{
    if(!m_pData)
        return -1;
    if(!m_nDataLen)
        return -1;
    if(!szString)
        return -1;
    int nRet=::strcmp(m_pData,szString);
    return nRet;
}

char* CNEODynamicBuffer::GetBuffer(void)
{
    return m_pData;
}
int CNEODynamicBuffer::GetBufferLength(void)
{
    return m_nDataLen;
}

//静态Buffer

CNEOStaticBuffer::CNEOStaticBuffer(CNEOMemPoolWithLock *pMemPool)
    :m_pMemPool(pMemPool),m_nDataLen(0)
{
    //m_pMemPool=pMemPool;
    //m_nDataLen=0;
}
CNEOStaticBuffer::~CNEOStaticBuffer(){}                      
///////////////////////////////
//尺寸设置函数
bool CNEOStaticBuffer::SetSize(int nSize) //设置新的大小
{
    if(NEO_SAFE_BUFFER_MAX_SIZE<nSize)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::SetSize():ERROR!nSize=%d\n",nSize);////////neoqueue.cpp中包含neomem.h,neomem.h 中并不包含neodebug.h
        return false;
    }
    m_nDataLen=nSize;
    return true;
}
bool CNEOStaticBuffer::InsertSpaceToHead(int nAddBytes)//在前面插入空白
{
    if(0>=m_nDataLen)
    {
        //如果没有原始数据，则视为设置缓冲区大小
        m_nDataLen=nAddBytes;
        return true;
    }
    if(nAddBytes>(NEO_SAFE_BUFFER_MAX_SIZE-m_nDataLen))
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::InsertSpaceToHead();ERROR!nAddBytes=%d,m_nDataLen=%d,too big!\n",nAddBytes,m_nDataLen);
        return false;
    }
    {
    //此处的大括号，是限定变量szBuffer的作用范围
        char szBuffer[NEO_SAFE_BUFFER_MAX_SIZE];
        memset(szBuffer,'\0',NEO_SAFE_BUFFER_MAX_SIZE);
        //第一次，把原有数据拷贝到新区
        memcpy(szBuffer+nAddBytes,m_pData,m_nDataLen);
        m_nDataLen+=nAddBytes;
        //第二次，将缓冲区数据拷贝回本对象缓冲区
        memcpy(m_pData,szBuffer,m_nDataLen);
    }
    return true;
}
bool CNEOStaticBuffer::AddSpaceToTail(int nAddBytes)//在后面插入空白
{
    if(0>=m_nDataLen)
    {
        m_nDataLen=nAddBytes;
        return true;
    }
    if(nAddBytes>(NEO_SAFE_BUFFER_MAX_SIZE-m_nDataLen))
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::AddSpaceToTail();ERROR!nAddBytes=%d,m_nDataLen=%d,too big!\n",nAddBytes,m_nDataLen);
        return false;
    }
    m_nDataLen+=nAddBytes;
    return true;
}
void CNEOStaticBuffer::CutHead(int nBytes)// 从前面剪掉一段数据
{
    if(0>=m_nDataLen)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutHead();ERROR!m_nDataLen=%d,too small!\n",m_nDataLen);
        m_nDataLen=0;
        return;
    }
    if(nBytes>m_nDataLen)
    {
        //从后向前移动拷贝
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutHead();ERROR!m_nDataLen=%d,nBytes=%d,too small!\n",m_nDataLen,nBytes);
        m_nDataLen=0;
        return;
    }
    m_nDataLen-=nBytes;
    //从后向前拷贝，挤出原有数据
    memcpy(m_pData,m_pData+nBytes,m_nDataLen);
    return;
}
void CNEOStaticBuffer::CutTail(int nBytes)//从后面剪掉一段数据
{
        if(0>=m_nDataLen)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutTail();ERROR!m_nDataLen=%d,too small!\n",m_nDataLen);
        m_nDataLen=0;
        return;
    }
    if(nBytes>m_nDataLen)
    {
        //从后向前移动拷贝
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutTail();ERROR!m_nDataLen=%d,nBytes=%d,too small!\n",m_nDataLen,nBytes);
        m_nDataLen=0;
        return;
    }
    m_nDataLen-=nBytes;
    return;
}
////////////////////////////////////////
//数值转换函数
bool CNEOStaticBuffer::SetInt(int n)//讲一个整数以二进制方式拷贝到缓冲区带网络字节序
{
    int nSave=htonl(n);
    //拷贝到本地缓冲区
    return BinCopyFrom((char *)&nSave,sizeof(int));
}
int CNEOStaticBuffer::GetInt(void)//以整数方式获得缓冲区的数值
{
    if(0>=m_nDataLen)
        return 0;
    int *pData=(int *)m_pData;
    int nRet=*pData;
    return ntohl(nRet);//用这种方式取得前四字节
}
bool CNEOStaticBuffer::SetShort(short n)//讲一个短整数以二进制方式拷贝到缓冲区带网络字节序
{
    short sSave=htons(n);
    return BinCopyFrom((char *)&sSave,sizeof(short));
}

short CNEOStaticBuffer::GetShort(void)//端口号
{
    if(0>=m_nDataLen)
        return 0;
    short *pData=(short *)m_pData;
    short sRet=*pData;
    return ntohs(sRet);//用这种方式取得前四字节
}
bool CNEOStaticBuffer::SetChar(char n)//将一个字节以二进制方式拷贝到缓冲区
{
    *m_pData=n;
    m_nDataLen=sizeof(char);
    return true;
}
char CNEOStaticBuffer::GetChar(void)//以字节方式获得缓冲区的数值
{
    return *m_pData;
}
//////////////////////////////////////////
//二进制数据追加函数
//追加数据到最后，返回新的长度
int CNEOStaticBuffer::AddData(char *szData,int nDataLen)
{
    int nNewSize=m_nDataLen+nDataLen;
    if(NEO_SAFE_BUFFER_MAX_SIZE<nNewSize)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::AddData();ERROR!m_nDataLen=%d,nDataLen=%d,too big!\n",m_nDataLen,nDataLen);
        return 0;
    }
    memcpy(m_pData+m_nDataLen,szData,nDataLen);
    m_nDataLen=nNewSize;
    return m_nDataLen;
}
//插入数据的最前面，返回新的数据长度
int CNEOStaticBuffer::InsertDataToHead(char *szData,int nDataLen)
{
    if(!InsertSpaceToHead(nDataLen))
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::InsertDataToHead():ERROR!m_nDataLen=%d,nDataLen=%d\n",m_nDataLen,nDataLen);
        memcpy(m_pData,szData,nDataLen);
        return 0;
    }
    memcpy(m_pData,szData,nDataLen);
    return m_nDataLen;
}
/////////////////////////////////////////
//二进制数据拷贝函数
//拷贝到一块目标缓冲区域，受传入的缓冲区长度限制
int CNEOStaticBuffer::BinCopyTo(char *szBuffer,int nBufferSize)
{
    if(m_nDataLen>nBufferSize)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::BinCopyTo();ERROR!nBufferSize=%d,m_nDataLen=%d,too small!\n",nBufferSize,m_nDataLen);
        return 0;
    }
    if(!szBuffer)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::BinCopyTo();ERROR!szBuffer=NULL\n");
        return 0;
    }
    //执行真是的拷贝逻辑
    memcpy(szBuffer,m_pData,m_nDataLen);
    return m_nDataLen;
}
//从一块来源缓冲区拷贝数据到本对象中
int CNEOStaticBuffer::BinCopyFrom(char *szData,int nDataLen)
{
    if(NEO_SAFE_BUFFER_MAX_SIZE<nDataLen)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::BinCopyFrom();ERROR!nDataLen=%d,too big\n",nDataLen);
        return 0;
    }
    if(!szData)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::BinCopyFrom();ERROR!szBuffer=NULL\n");
        return 0;
    }
    if(0>=nDataLen)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::BinCopyFrom();ERROR!0>=nDataLen\n");
        return 0;
    }
    //拷贝动作
    memcpy(m_pData,szData,nDataLen);
    m_nDataLen=nDataLen;
    return m_nDataLen;
}
//从另外一个Buffer对象拷贝数据到本对象
int CNEOStaticBuffer::BinCopyFrom(CNEODynamicBuffer *pBuffer)
{
    return BinCopyFrom(pBuffer->m_pData,pBuffer->m_nDataLen);
}
/////////////////////////////////////////////
//文本数据拷贝构建函数
int CNEOStaticBuffer::StrCopyFrom(char *szString)
{
    int n=strlen(szString);
    n++;//流出“\n”
    return BinCopyFrom(szString,n);
}
int CNEOStaticBuffer::Printf(char *szFormat,...)
{
    char szBuf[NEO_SAFE_BUFFER_MAX_SIZE];
    int nListCount=0;
    va_list pArgList;
    va_start(pArgList,szFormat);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,NEO_SAFE_BUFFER_MAX_SIZE-nListCount,szFormat,pArgList);
    va_end(pArgList);
    if(nListCount>(NEO_SAFE_BUFFER_MAX_SIZE-1))
        nListCount=NEO_SAFE_BUFFER_MAX_SIZE-1;
    *(szBuf+nListCount)='\0';
    return StrCopyFrom(szBuf);
}
/////////////////////////////////////////////
//数据比较函数
int CNEOStaticBuffer::memcmp(char *szData,int nDataLen)
{
    if(0>=m_nDataLen)
        return -1;
    if(!szData)
        return -1;
    if(m_nDataLen<nDataLen)
        return -1;
    int nRet=::memcmp(m_pData,szData,nDataLen);
    return nRet;
}
int CNEOStaticBuffer::strcmp(char *szString)
{
    if(0>=m_nDataLen)
        return -1;
    if(!szString)
        return -1;
    int nRet=::strcmp(m_pData,szString);
    return nRet;
}

char* CNEOStaticBuffer::GetBuffer(void)
{
    return m_pData;
}
int CNEOStaticBuffer::GetBufferLength(void)
{
    return m_nDataLen;
}

////////////////////////////////////////////
//服务函数
bool CNEOStaticBuffer::IHaveData(void)
{
    if(0>=m_nDataLen)
        return false;
    return true;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//基本队列模型

CNEOPopBuffer::CNEOPopBuffer(char *szBuffer,           //缓冲区指针
    int nBufferSize,                    //缓冲区尺寸
    bool bInitFlag)                //是否初始化标志
{
    m_pHead=NULL;
    m_pBuffer=NULL;
    m_nBufferSize=nBufferSize;
    Set(szBuffer,nBufferSize);
    if(bInitFlag)//需要初始化
        Clean();//则清空队列
}
CNEOPopBuffer::~CNEOPopBuffer(){}
//实现粘合的方法,外部传入的缓冲区域内部缓冲区的指针挂接
void CNEOPopBuffer::Set(char *szBuffer,int nBufferSize)
{
    m_pBuffer=szBuffer;
    m_nBufferSize=nBufferSize;
    m_pHead=(SNEOPopBufferHead*)m_pBuffer;
}
//清空整个区域，数据清零，缓冲区不释放
void CNEOPopBuffer::Clean(void)
{
    if(m_pHead)
    {
        m_pHead->m_nTokenCount=0;
        m_pHead->m_nAllByteSCount=SNEOPopBufferHeadSize;
    }
}
//内部信息打印函数
void CNEOPopBuffer::PrintInside()
{
    if(!ICanWork())
    {
        NEO_PRINTF("CNEOPopBuffer::PrintInside();ERROR!m_pBuffer=null\n");
        return;
    }
    //定义开始的指针
    char *pTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);
    // 定义元素的头指针
    SNEOPopBufferTokenHead *pTokenHead=(SNEOPopBufferTokenHead*)pTokenBegin;
    char *pTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pTokenBegin);
    int i=0;
    //预打印整个队列的信息
    NEO_PRINTF("CNEOPopBuffer::PrintInside():Token:%d Bytes:%d\n",m_pHead->m_nTokenCount,m_pHead->m_nAllByteSCount);
    for(i=0;i<m_pHead->m_nTokenCount;i++)
    {
        NEO_PRINTF("[%d]-%s\n",pTokenHead->m_nDataLen,pTokenData);
		//Debug_bin(pTokenData,pTokenHead->m_nDataLen);
		pTokenBegin+=NEO_POP_BUFFER_TOKEN_LENGTH(pTokenHead->m_nDataLen);
        pTokenHead=(SNEOPopBufferTokenHead*)pTokenBegin;
        pTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pTokenBegin);
    }
}
//能否正确工作的标志函数
bool CNEOPopBuffer::ICanWork(void)
{
    if(!m_pBuffer)
        return false;
    if(!m_nBufferSize)
        return false;
    if(!m_pHead)
        return false;
    return true;
}
//队列最经典的功能，追加到末尾，
int CNEOPopBuffer::AddLast(const char *szData,int nDataLength)
{
    int nRet=0;
    if(!szData)
        goto CNEOPopBuffer_AddLast_End;
    if(0>=nDataLength)
        goto CNEOPopBuffer_AddLast_End;
    if(!ICanWork())
        goto CNEOPopBuffer_AddLast_End;
    if(!ICanSave(nDataLength))
        goto CNEOPopBuffer_AddLast_End;
    {//作用域限定
        char *pTokenBegin=m_pBuffer+m_pHead->m_nAllByteSCount;//用m_nAllByteSCount求出队列最尾的指针
        SNEOPopBufferTokenHead *pTokenHead=(SNEOPopBufferTokenHead*)pTokenBegin;//强制指针类型为Token头指针；
        char *pTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pTokenBegin);//求出Token数据区的指针
        //具体添加动作
        pTokenHead->m_nDataLen=nDataLength;//长度赋值
        memcpy(pTokenData,szData,nDataLength);
        m_pHead->m_nTokenCount++;//元素个数加一
        m_pHead->m_nAllByteSCount+=NEO_POP_BUFFER_TOKEN_LENGTH(nDataLength);
        nRet=nDataLength;//返回值的纯数据长度
    }
CNEOPopBuffer_AddLast_End:
    return nRet;
}
int CNEOPopBuffer::AddLast(CNEODynamicBuffer *pBuffer)
{
    if(!pBuffer)
        return 0;
    return AddLast(pBuffer->GetBuffer(),pBuffer->GetBufferLength());
}
int CNEOPopBuffer::AddLast(CNEOStaticBuffer *pBuffer)
{
    if(!pBuffer)
        return 0;
    return AddLast(pBuffer->GetBuffer(),pBuffer->GetBufferLength());
}
//获得当前内部元素个数
int CNEOPopBuffer::GetTokenCount(void)
{
    if(!ICanWork())
        return 0;
    return m_pHead->m_nTokenCount;
}
//获得所有的字节数
int CNEOPopBuffer::GetAllBytes(void)
{
    if(!ICanWork())
        return 0;
    return m_pHead->m_nAllByteSCount;
}
//判断内部剩余空间是否够用,是否够新加入的数据用
bool CNEOPopBuffer::ICanSave(int nDataLen)
{
    int nLeaveBytes=0;
    if(!ICanWork())
        return false;
    nLeaveBytes=m_nBufferSize-m_pHead->m_nAllByteSCount;
    if(NEO_POP_BUFFER_TOKEN_LENGTH(nDataLen)>(unsigned long)nLeaveBytes)
        return false;
    else
        return true;
}
//获得第一个元素的长度
int CNEOPopBuffer::GetFirstTokenLength(void)
{
    if(!ICanWork())
        return 0;
    char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);//计算第一个token起始点
    SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//强制转换
    return pFirstTokenHead->m_nDataLen;
}
//获取第一个元素
int CNEOPopBuffer::GetFirst(char *szBuffer,int nBufferSize)
{
    int nRet=0;
    if(!ICanWork())
        goto CNEOPopBuffer_GetFirst_End;
    //判定队列是否为空
    if(!GetTokenCount())
        goto CNEOPopBuffer_GetFirst_End;
    //判定给定的参数是否合法
    if(GetFirstTokenLength()>nBufferSize)
        goto CNEOPopBuffer_GetFirst_End;
    {//作用域限定
        char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);//计算第一个token起始点
        SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//强制转换
        char *pFirstTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pFirstTokenBegin);
        memcpy(szBuffer,pFirstTokenData,pFirstTokenHead->m_nDataLen);//拷贝数据到缓冲区
        nRet=pFirstTokenHead->m_nDataLen;
    }
CNEOPopBuffer_GetFirst_End:
    return nRet;
}
int CNEOPopBuffer::GetFirst(CNEODynamicBuffer *pBuffer)
{
    if(!ICanWork())
        return 0;
    if(!pBuffer->SetSize(GetFirstTokenLength()))
        return 0;
    if(!pBuffer->m_nDataLen)
        return 0;
    return GetFirst(pBuffer->GetBuffer(),pBuffer->GetBufferLength());
}
int CNEOPopBuffer::GetFirst(CNEOStaticBuffer *pBuffer)
{
    if(!ICanWork())
        return 0;
    if(!pBuffer->SetSize(GetFirstTokenLength()))
        return 0;
    if(!pBuffer->m_nDataLen)
        return 0;
    return GetFirst(pBuffer->GetBuffer(),pBuffer->GetBufferLength());
}
//删除一个元素
bool CNEOPopBuffer::DeleteFirst(void)
{
    bool bRet=false;
    if(!ICanWork())
        goto CNEOPopBuffer_DeleteFirst_End;
    //判定队列是否为空
    if(!GetTokenCount())
        goto CNEOPopBuffer_DeleteFirst_End;
    {//作用域限定
        char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);//计算第一个token起始点
        SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//强制转换
        int nFirstTokenLen=NEO_POP_BUFFER_TOKEN_LENGTH(pFirstTokenHead->m_nDataLen);//第一个token总长度
        char *pSecondTokenBegin=pFirstTokenBegin+nFirstTokenLen;//求出第二个token的起点
        int nCopyBytesCount=m_pHead->m_nAllByteSCount-SNEOPopBufferHeadSize-nFirstTokenLen;//需要移动的字节数
        memcpy(pFirstTokenBegin,pSecondTokenBegin,nCopyBytesCount);
        m_pHead->m_nAllByteSCount-=nFirstTokenLen;
        m_pHead->m_nTokenCount--;
        bRet=true;
    }
CNEOPopBuffer_DeleteFirst_End:
    return bRet;
}
//从队列总弹出第一个元素
int CNEOPopBuffer::GetAndDeleteFirst(char *szBuffer,int nBufferSize)
{
    if(!ICanWork())
        return 0;
    int nRet=GetFirst(szBuffer,nBufferSize);//获得第一个元素
    DeleteFirst();
    return nRet;
}
int CNEOPopBuffer::GetAndDeleteFirst(CNEODynamicBuffer *pBuffer)
{
    if(!ICanWork())
        return 0;
    int nRet=GetFirst(pBuffer);
    DeleteFirst();
    return nRet;
}
int CNEOPopBuffer::GetAndDeleteFirst(CNEOStaticBuffer *pBuffer)
{
    if(!ICanWork())
        return 0;
    int nRet=GetFirst(pBuffer);
    DeleteFirst();
    return nRet;
}
//枚举遍历所有数据，提交回调函数处理，并删除数据
int CNEOPopBuffer::MoveAllData(_NEO_ENUM_DATA_CALLBACK pCallBack,void *pCallParam)
{
    int nMovedTokenCount=0;
    bool bCallbackRet=true;
    //if(!pCallBack)
    //  goto CNEOPopBuffer_MoveAllData_End;
    if(!ICanWork())
        goto CNEOPopBuffer_MoveAllData_End;
    while(m_pHead->m_nTokenCount)
    {
        char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);
        SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//强制转换
        char *pFirstTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pFirstTokenBegin);
        if(pCallBack)
        {
            bCallbackRet=pCallBack(pFirstTokenData,pFirstTokenHead->m_nDataLen,pCallParam);
            if(!bCallbackRet)
                break;
        }
        DeleteFirst();
        nMovedTokenCount++;
        
    }
CNEOPopBuffer_MoveAllData_End:
    return nMovedTokenCount;
}
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//动态内存分配，队列采用列表的(依赖内存池)
CNEOMemQueue::CNEOMemQueue(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,
    const char *szAppName,                    // 应用程序名，代表队列名
    int nMaxToken  //最大token上限
    ):m_nMaxToken(nMaxToken),m_nTokenCount(0),
    m_pHead(NULL),m_pLast(NULL),
    m_pDebug(pDebug),m_pMemPool(pMemPool)
{
    //m_nMaxToken=nMaxToken;
    //m_pLast=NULL;
    //m_pDebug=pDebug;
    //m_pMemPool=pMemPool;
    SafeStrcpy(m_szAppName,szAppName,NEO_APPLICATION_NAME_SIZE);
    //m_pHead=NULL;
    //m_nTokenCount=0;
}
CNEOMemQueue::~CNEOMemQueue()
{
    if(m_pHead)
    {
        DeleteATokenAndHisNext(m_pHead);//递归释放
        m_pHead=NULL;
    }
}
bool CNEOMemQueue::ICanWork(void)
{
    if(!m_pDebug)
        return false;
    if(!m_pMemPool)
        return false;
    return true;
}
void CNEOMemQueue::CleanAll(void)              //清除所有token
{
    if(!ICanWork())
        return;
    while(DeleteFirst()){}       //循环删除第一个对象知道队列为空
}
int CNEOMemQueue::GetFirstLength(void)        // 获得第一个token数据长度
{
    int nRet=0;
    if(m_pHead)                  //ICAnWork为否，m_pHead必为NULL
    {
        nRet=m_pHead->m_nDataLen;
    }
    return nRet;
}
int CNEOMemQueue::GetTokenCount(void)        //获得token总数
{
    return m_nTokenCount;
}
void CNEOMemQueue::PrintInside(void)         //打印搜有队列内部token数据
{
    if(!ICanWork())
        return;
    NEO_PRINTF("Queue:Token Count=%d,Head=0x%p,Last=0x%p\n",
        m_nTokenCount,m_pHead,m_pLast);
    if(m_pHead)
        PrintAToken(m_pHead);//从队列头开始递归
}
int CNEOMemQueue::AddLast(const char *szData       //数据指针
    ,int nDataLen,             //数据长度
    int nLimit)              //防止递归长度过神
{          
    int nRet=0;
    if(!ICanWork())
        goto CNEOMemQueue_AddLast_End;
    if(0>=nLimit)               //应用限制值
    {
        // 这是无限制
        if(m_nMaxToken<=m_nTokenCount)//无限制时，以m_nMaxToken作为边界限制
            goto CNEOMemQueue_AddLast_End;
    }//if
    else
    {
        //这是限制
        if(nLimit<=m_nTokenCount) //如果有nlimit，则使用这个参数限制
            goto CNEOMemQueue_AddLast_End;
    }
    if(!m_pHead)
    {
        m_pHead=GetAToken();//链头的第一次初始化
        if(!m_pHead)
        {
            m_pDebug->DebugToFile("%s::AddLast():malloc m_pHead fail \n",m_szAppName);
            goto CNEOMemQueue_AddLast_End;
        }

    }
    if(m_pLast)//加速因子作用，直接跳入
    {
        nRet=AddLastToThisToken(m_pLast,szData,nDataLen);
    }
    else if(m_pHead)//加速因子无值，传统模式，遍历插入
    {
        nRet=AddLastToThisToken(m_pHead,szData,nDataLen);
    }
CNEOMemQueue_AddLast_End:
    return nRet;
}
int CNEOMemQueue::GetFirst(char *szBuffer,int nBufferSize)
{
    int nRet=0;
    if(!ICanWork())
        goto CNEOMemQueue_GetFirst_End;
    if(!m_pHead)                       //检查链表是否有数据
        goto CNEOMemQueue_GetFirst_End;
    if(!m_pHead->m_pBuffer)           //检查第一个Token是否有数据
    {
        m_pDebug->DebugToFile("%s::GetFirst():m_pHead->m_pBuffer=NULL\n",m_szAppName);
        goto CNEOMemQueue_GetFirst_End;
    }
    if(m_pHead->m_nDataLen>nBufferSize)//检查给定的缓冲区是否足够
    {
        m_pDebug->DebugToFile("%s::GetFirst():m_pHead->m_nDataLen>nBufferSize\n",m_szAppName);
        goto CNEOMemQueue_GetFirst_End;
    }
    memcpy(szBuffer,m_pHead->m_pBuffer,m_pHead->m_nDataLen);
    nRet=m_pHead->m_nDataLen;
CNEOMemQueue_GetFirst_End:
    return nRet;
}
int CNEOMemQueue::GetFirst(CNEOStaticBuffer *pBuffer)
{
    if(!ICanWork())
        return 0;
    if(!pBuffer)
        return 0;
    return pBuffer->BinCopyFrom(m_pHead->m_pBuffer,m_pHead->m_nDataLen);
}
int CNEOMemQueue::GetFirst(CNEODynamicBuffer *pBuffer)
{
    if(!ICanWork())
        return 0;
    if(!pBuffer)
        return 0;
    return pBuffer->BinCopyFrom(m_pHead->m_pBuffer,m_pHead->m_nDataLen);
}
bool CNEOMemQueue::DeleteFirst(void)
{
    bool bRet=false;
    SNEOQueueTokenHead *pSecond=NULL;//指向第二个Token的临时指针
    if(!ICanWork())
        goto CNEOMemQueue_DeleteFirst_End;
    if(!m_pHead)
        goto CNEOMemQueue_DeleteFirst_End;
    pSecond=m_pHead->m_pNext;
    m_pHead->m_pNext=NULL;
    bRet=DeleteATokenAndHisNext(m_pHead);
    if(bRet)
    {
        m_pHead=pSecond;
        if(!m_pHead)//若若队列呗删空，则清楚加速因子
            m_pLast=NULL;
    }
    else//删除失败
    {
        m_pDebug->DebugToFile("%s::DeleteFirst():delete m_pHead fail\n",m_szAppName);
        m_pHead->m_pNext=pSecond;
    }
CNEOMemQueue_DeleteFirst_End:
    return bRet;
}
int CNEOMemQueue::GetAndDeleteFirst(char *szBuffer,int nBufferSize)
{
    int nRet=GetFirst(szBuffer,nBufferSize);
    if(nRet)
        DeleteFirst();
    return nRet;
}
int CNEOMemQueue::GetAndDeleteFirst(CNEOStaticBuffer *pBuffer)
{
    int nRet=GetFirst(pBuffer);
    if(nRet)
        DeleteFirst();
    return nRet;
}
int CNEOMemQueue::GetAndDeleteFirst(CNEODynamicBuffer *pBuffer)
{
    int nRet=GetFirst(pBuffer);
    if(nRet)
        DeleteFirst();
    return nRet;
}
    
//从前面弹出一批数据，
int CNEOMemQueue::PopFromFirst(char *szBuffer,int nBufferSize)
{
    int nCopyBytes=0;
    CNEOPopBuffer PopBuffer(szBuffer,nBufferSize);
    if(!ICanWork())
        goto CNEOMemQueue_PopFromFirst_End;
    if(m_pHead)
    {
        nCopyBytes=PopFromFirst4NEOPopBuffer(&PopBuffer);
        if(nCopyBytes)
            nCopyBytes=PopBuffer.GetAllBytes();
    }
CNEOMemQueue_PopFromFirst_End:
    return nCopyBytes;
}
//从popbuffer的数据区弹出所有数据，追加到队列尾
int CNEOMemQueue::PushToLast(char *szData,int nDataLen)
{
    int nRet=0;
    CNEOPopBuffer cPopBuffer(szData,nDataLen,false);
    if(!ICanWork())
        goto CNEOMemQueue_PushToLast_End;

    //此处调用popbuffer的函数删除说有数据，并使用回调函数处理数据
    //回调函数在此处传入CNEOMemQueue对象（本对象),并调用该对象的addlast，
    //依次将对象推入memqueue
    cPopBuffer.MoveAllData(PushDataCallback,this);
CNEOMemQueue_PushToLast_End:
    return nRet;
}
    //队列数据写入磁盘
void CNEOMemQueue::WriteToFile(const char *szFileName)
{
    FILE *fp=NULL;
    if(!ICanWork())
        return;
    if(!m_pHead)
        return;
    if(!GetTokenCount())
        return;
    fp=fopen(szFileName,"wb");
    if(fp)
    {
        fwrite((void *)&m_nTokenCount,sizeof(int),1,fp);
        WriteATokenToFile(m_pHead,fp);
        fclose(fp);
    }
}
//队列数据从磁盘读出
int CNEOMemQueue::ReadFromFile(const char *szFileName)
{
    FILE *fp=NULL;
    int n=0;
    int i=0;
    int nReadTokenCount=0;
    int nDataLen=0;
    char *pTempBuffer=NULL;
    char szNameBuffer[256];
    if(!ICanWork())
        goto CNEOMemQueue_ReadFromFile_End;
    SafePrintf(szNameBuffer,256,"%s::ReadFromFile::pTempBuffer",m_szAppName);////////////////////////////????
    pTempBuffer=(char *)m_pMemPool->Malloc(1,szNameBuffer);
    fp=fopen(szFileName,"rb");
    if(!fp)
        goto CNEOMemQueue_ReadFromFile_End;
    n=fread((void*)&nReadTokenCount,sizeof(int),1,fp);
    if(1>n)
    {
        m_pDebug->DebugToFile("%s::ReadFromFile():Read token count fail!\n",m_szAppName);
        goto CNEOMemQueue_ReadFromFile_End;
    }
    for(i=0;i<nReadTokenCount;i++)
    {
        n=fread((void *)&(nDataLen),sizeof(int),1,fp);
        if(1>n)
        {
            m_pDebug->DebugToFile("%s::ReadFromFile():%d/%d,read data len fail!\n",m_szAppName,i,nReadTokenCount);
            goto CNEOMemQueue_ReadFromFile_End;
        }
        if(0>nDataLen)
        {
            m_pDebug->DebugToFile("%s::ReadFromFile():%d/%d,nDataLen=%d<0!\n",m_szAppName,i,nReadTokenCount,nDataLen);
            goto CNEOMemQueue_ReadFromFile_End;
        }
        pTempBuffer=(char *)m_pMemPool->ReMalloc(pTempBuffer,nDataLen,false);
        if(!pTempBuffer)
        {
            m_pDebug->DebugToFile("%s::ReadFromFile():remalloc pTempBuffer fail!\n",m_szAppName);
            goto CNEOMemQueue_ReadFromFile_End;
        }
        //开始读入 token 数据
        n=fread((void *)pTempBuffer,sizeof(char),nDataLen,fp);
        if(nDataLen>n)
        {
            m_pDebug->DebugToFile("%s::ReadFromFile():read data  fail!\n",m_szAppName);
            goto CNEOMemQueue_ReadFromFile_End;
        }
        //读入成功
        if(!AddLast(pTempBuffer,nDataLen))
            break;
    }
CNEOMemQueue_ReadFromFile_End:
    if(pTempBuffer)
    {
        m_pMemPool->Free(pTempBuffer);
        pTempBuffer=NULL;
    }
    if(fp)
    {
        fclose(fp);
        fp=NULL;
    }
    return nReadTokenCount;
}
void CNEOMemQueue::PrintAToken(SNEOQueueTokenHead *pToken)
{
    if(!pToken)
        return;
    NEO_PRINTF("QUEUE TOKEN :pToken:%p,\
                Buffer=%p,Length=%d,m_pNext=%p\n",
                pToken,pToken->m_pBuffer,
                pToken->m_nDataLen,pToken->m_pNext);
    if(pToken->m_pBuffer)
        Debug_bin(pToken->m_pBuffer,pToken->m_nDataLen);                         ///需要导入Debug文件
    if(pToken->m_pNext)//递归
        PrintAToken(pToken->m_pNext);

}
void CNEOMemQueue::WriteATokenToFile(SNEOQueueTokenHead *pToken,FILE *fp)
{
    if(!ICanWork())
    {
        return;
    }
    if(!fp)
        return;
    if(!pToken)
        return;
    if(!pToken->m_pBuffer)
        return;
    if(!pToken->m_nDataLen)
        return;
    //写入磁盘
    fwrite((void *)&(pToken->m_nDataLen),sizeof(int),1,fp);
    fwrite((void *)pToken->m_pBuffer,sizeof(char),pToken->m_nDataLen,fp);
    if(pToken->m_pNext)//递归到下一个token
        WriteATokenToFile(pToken->m_pNext,fp);
}
int CNEOMemQueue::PopFromFirst4NEOPopBuffer(CNEOPopBuffer *pPopBuffer)
{
    int nRet=0;
    if(!ICanWork())
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    if(!m_pHead)
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    if(!m_pHead->m_pBuffer)
    {
        m_pDebug->DebugToFile("%s::PopFromFirst4NEOPopBuffer():Mm_pHead->m_pBuffer=NULL!\n",m_szAppName);
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    if(!m_pHead->m_nDataLen)
    {
        m_pDebug->DebugToFile("%s::PopFromFirst4NEOPopBuffer():Mm_pHead->m_nDataLen=NULL!\n",m_szAppName);
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    if(!pPopBuffer)
    {
        m_pDebug->DebugToFile("%s::PopFromFirst4NEOPopBuffer():pPopBuffer=NULL\n",m_szAppName);
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    nRet=pPopBuffer->AddLast(m_pHead->m_pBuffer,m_pHead->m_nDataLen);
    if(m_pHead->m_nDataLen!=nRet)
    {
        //这是buffer 装满了
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    if(!DeleteFirst())
    {
        m_pDebug->DebugToFile("%s::PopFromFirst4NEOPopBuffer():DeleteFirst fail\n",m_szAppName);
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    if(m_pHead)//递归，继续弹出
        nRet+=PopFromFirst4NEOPopBuffer(pPopBuffer);
CNEOMemQueue_PopFromFirst4NEOPopBuffer_End:
    return nRet;
}
//申请用来存储数据的内存
int CNEOMemQueue::AddLastToThisToken(SNEOQueueTokenHead *pToken,const char *szData,int nDataLen)
{
    int nRet=0;
    char szNameBuffer[256];
    if(!ICanWork())
        goto CNEOMemQueue_AddLastToThisToken_End;
    //指针不为空，说明存在数据，跳到next指针处
    if(!pToken->m_pBuffer)
    {
        //如果本Token不包含有效数据，则保存到自己
        SafePrintf(szNameBuffer,256,"%s::pToken->m_pBuffer",m_szAppName);  //格式化说明文字
        pToken->m_pBuffer=(char *)m_pMemPool->Malloc(nDataLen,szNameBuffer);//向内存池申请数据块
        if(!pToken->m_pBuffer)
        {
        //申请失败
            m_pDebug->DebugToFile("%s::AddLastToThisToken():Malloc new Token fail!\n",m_szAppName);
            goto CNEOMemQueue_AddLastToThisToken_End;
        }
        memcpy(pToken->m_pBuffer,szData,nDataLen); //拷贝业务数据到内存块
        pToken->m_nDataLen=nDataLen;
        nRet=nDataLen;
        m_pLast=pToken;
        goto CNEOMemQueue_AddLastToThisToken_End;      
    }//if
    else
    {
        //next为空，则在next下面插入
        if(!pToken->m_pNext)
        {
            //如果下家的链指针为空，就利用GetAToken创建头
            pToken->m_pNext=GetAToken();
            if(!pToken->m_pNext)
            {
                //创建失败报警
                m_pDebug->DebugToFile("%s::AddLastToThisToken():malloc pToken->m_pNext fail!!\n",m_szAppName);
                goto CNEOMemQueue_AddLastToThisToken_End;
            }
        }
        if(pToken->m_pNext)//递归调用
            nRet=AddLastToThisToken(pToken->m_pNext,szData,nDataLen);
    }
CNEOMemQueue_AddLastToThisToken_End:
    return nRet;
}
//申请一个token头
SNEOQueueTokenHead *CNEOMemQueue::GetAToken(void)//返回指针，不遵守谁申请谁释放的原则
{
    SNEOQueueTokenHead *pToken=NULL;
    char *pTokenBuffer=NULL;                 //临时指针
    char szNameBuffer[NEO_APPLICATION_NAME_SIZE];//说明文字缓冲区
    if(!ICanWork())
        goto CNEOMemQueue_GetAToken_End;
    SafePrintf(szNameBuffer,256,"%s::Token_Head",m_szAppName);

    //开始申请Token头内存块
    pTokenBuffer=(char *)m_pMemPool->Malloc(SNEOQueueTokenHeadSize,szNameBuffer);
    if(!pTokenBuffer)
    {
        m_pDebug->DebugToFile("%s::GetAToken:Malloc new Token fail!\n",m_szAppName);
        goto CNEOMemQueue_GetAToken_End;
    }
    //强制指针转换
    pToken=(SNEOQueueTokenHead *)pTokenBuffer;
    pToken->m_nDataLen=0;
    pToken->m_pNext=NULL;
    pToken->m_pBuffer=NULL;
    m_nTokenCount++;
CNEOMemQueue_GetAToken_End:
    return pToken;
}
bool CNEOMemQueue::DeleteATokenAndHisNext(SNEOQueueTokenHead *pToken)
{
    bool bRet=false;
    if(!ICanWork())
        goto CNEOMemQueue_DeleteATokenAndHisNext_End;
    if(pToken->m_pNext)//递归删除
    {
        DeleteATokenAndHisNext(pToken->m_pNext);
        pToken->m_pNext=NULL;
    }
    if(pToken->m_pBuffer)//开始删除本对象
    {
        m_pMemPool->Free(pToken->m_pBuffer);//向内存缓冲池释放数据缓冲
        pToken->m_nDataLen=0;
        pToken->m_pBuffer=NULL;
    }
    m_pMemPool->Free((void *)pToken);
    m_nTokenCount--;
    bRet=true;
CNEOMemQueue_DeleteATokenAndHisNext_End:
    m_pLast=NULL;
    return bRet;
}
//回调函数申明
bool CNEOMemQueue::PushDataCallback(char *szData,int nDataLen,void *pCallParam)
{
    CNEOMemQueue *pThis=(CNEOMemQueue *)pCallParam;
    int nRet=pThis->AddLast(szData,nDataLen);
    if(nDataLen==nRet)
        return true;
    else
    {
        pThis->m_pDebug->DebugToFile("%s::PushDataCallback(): I am full!\n",pThis->m_szAppName);
        return false;
    }
}
//带安全锁的类
CNEOMemQueueWithLock::CNEOMemQueueWithLock(CNEOLowDebug *pDebug,
    CNEOMemPoolWithLock *pMemPool,
    const char *szAppName,                    // 应用程序名，代表队列名
    int nMaxToken
    )
{
    SafeStrcpy(m_szAppName,szAppName,NEO_APPLICATION_NAME_SIZE);
    m_pMemPool=pMemPool;
    m_pQueue=new CNEOMemQueue(pDebug,pMemPool,m_szAppName,nMaxToken);
    if(m_pQueue)
    {
        char szNameBuffer[256];
        SafePrintf(szNameBuffer,256,"%s::m_pQueue",m_szAppName);
        m_pMemPool->Register(m_pQueue,szNameBuffer);
    }
}
CNEOMemQueueWithLock::~CNEOMemQueueWithLock()
{
    m_Lock.EnableWrite();
    {
        if(m_pQueue)
        {
            m_pMemPool->UnRegister(m_pQueue);
            delete m_pQueue;
            m_pQueue=NULL;
        }
    }
    m_Lock.DisableWrite();
}
bool CNEOMemQueueWithLock::ICanWork(void)
{
    if(!m_pMemPool)
        return false;
    if(!m_pQueue)
        return false;
    bool bRet=false;
    m_Lock.AddRead();
    {
        bRet=m_pQueue->ICanWork();
    }
    m_Lock.DecRead();
    return bRet;
}
int CNEOMemQueueWithLock::AddLast(const char *szData,int nDataLen)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        nRet=m_pQueue->AddLast(szData,nDataLen);
    }
    m_Lock.DisableWrite();
    return nRet;
}
int CNEOMemQueueWithLock::GetFirst(CNEODynamicBuffer *pBuffer)
{
    int nRet=0;
    m_Lock.AddRead();
    {
        nRet=m_pQueue->GetFirst(pBuffer);
    }
    m_Lock.DecRead();
    return nRet;
}
int CNEOMemQueueWithLock::GetFirst(char *szBuffer,int nBufferSize)
{
    int nRet=0;
    m_Lock.AddRead();
    {
        nRet=m_pQueue->GetFirst(szBuffer,nBufferSize);
    }
    m_Lock.DecRead();
    return nRet;
}
int CNEOMemQueueWithLock::GetFirstLen(void)
{
    int nRet=0;
    m_Lock.AddRead();
    {
        nRet=m_pQueue->GetFirstLength();
    }
    m_Lock.DecRead();
    return nRet;
}
int CNEOMemQueueWithLock::GetTokenCount(void)
{
    int nRet=0;
    m_Lock.AddRead();
    {
        nRet=m_pQueue->GetTokenCount();
    }
    m_Lock.DecRead();
    return nRet;
}
int CNEOMemQueueWithLock::GetAndDeleteFirst(char *szBuffer,int nBufferSize)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        nRet=m_pQueue->GetAndDeleteFirst(szBuffer,nBufferSize);
    }
    m_Lock.DisableWrite();
    return nRet;
}
int CNEOMemQueueWithLock::PopFromFirst(char *szBuffer,int nBufferSize)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        nRet=m_pQueue->PopFromFirst(szBuffer,nBufferSize);
    }
    m_Lock.DisableWrite();
    return nRet;
}
int CNEOMemQueueWithLock::PushToLast(char *szData,int nDataLen)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        nRet=m_pQueue->PushToLast(szData,nDataLen);
    }
    m_Lock.DisableWrite();
    return nRet;
}
void CNEOMemQueueWithLock::CleanAll(void)
{
    m_Lock.EnableWrite();
    {
        m_pQueue->CleanAll();
    }
    m_Lock.DisableWrite();
}
bool CNEOMemQueueWithLock::DeleteFirst(void)
{
    bool bRet=false;
    m_Lock.EnableWrite();
    {
        bRet=m_pQueue->DeleteFirst();
    }
    m_Lock.DisableWrite();
    return bRet;
}
void CNEOMemQueueWithLock::WriteToFile(const char *szFileName)
{
    m_Lock.AddRead();
    {
        m_pQueue->WriteToFile(szFileName);
    }
    m_Lock.DecRead();
}
int CNEOMemQueueWithLock::ReadFromFile(const char *szFileName)
{
    int nRet=0;
    m_Lock.EnableWrite();
    {
        nRet=m_pQueue->ReadFromFile(szFileName);
    }
    m_Lock.DisableWrite();
    return nRet;
}
void CNEOMemQueueWithLock::PrintInside()
{
    m_Lock.AddRead();
    {
        m_pQueue->PrintInside();
    }
    m_Lock.DecRead();
}

//#else 
//#endif
}
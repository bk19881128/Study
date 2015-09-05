//Ҫȥ�����ڴ�ص���


//#ifdef WIN32_NEO
#include "neoindex.h" 

#include "neomemmanager.h"     
#include "neodebug.h"      
#include "neolock.h"         
#include "neosafefunc.h"
#include "neoqueue.h"

namespace NEOLIB {
//��ҪΪ��ʵ�ֱ���ƴ�Ӻ�������������ڴ�أ�
//��̬buffer

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
//�ߴ����ú���
bool CNEODynamicBuffer::SetSize(int nSize) //�����µĴ�С
{
    if(!m_pMemPool)
        return false;
    m_nDataLen=nSize;
    if(0==m_nDataLen)
    {
        if(m_pData)
        {
            m_pMemPool->Free(m_pData);//�ڴ���ͷ�
            m_pData=NULL;             //�ͷź���������ֵ
        }
        return true;                 //��Ҳ�ǲ����ɹ�
    }
    //�����õĻ����ڳ��ȣ�һ����Ϊ0
    if(!m_pData)                    //��ԭָ��Ϊ�գ���ʾû�����ݣ���ҪMalloc
    {
        m_pData=(char *)m_pMemPool->Malloc(m_nDataLen,"CNEODynamicBuffer::m_pData");//������ڴ�鳤��
        if(!m_pData)//�ж������Ƿ�ɹ�
        {
            m_nDataLen=0;
            return false;
        }
        else 
            return true;
    }//if
    else //�Ѿ���һ���ڴ��������
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
/*�ú�������mempool�������Ŀռ䣬���ǻ������ͷ�
  ��Ҫԭ���Ǳ���memcpy ǰ�򿽱������Ĵ���*/
bool CNEODynamicBuffer::InsertSpaceToHead(int nAddBytes)//��ǰ�����հ�
{
    bool bRet=false;
    int nNewSize=0;                   //�µ� �ռ������С
    char *pBuffer=NULL;               //�����ƻ�����ָ��
    if(!m_pMemPool)
        goto CNEODynamicBuffer_InsertSpaceToHead_End;
    nNewSize=m_nDataLen+nAddBytes;    //����µĳ���
    //��ע���������һ����ʱ���������䳤�ȵ���ԭ����+���ӵĳ���
    pBuffer=(char *)m_pMemPool->Malloc(nNewSize,"CNEODynamicBuffer::InsertSpaceToHead();pBuffer");
    if(!pBuffer)                      //����ʧ��
        goto CNEODynamicBuffer_InsertSpaceToHead_End;
    //�绺����Ϊ�գ��򲻿���
    if((m_pData)&&(m_nDataLen))
    {
        //��ԭ�л��������ݿ�������ʱ��������벿�֣��������ӵ��ֽڹ���һ������
        memcpy(pBuffer+nAddBytes,m_pData,m_nDataLen);
    }
    bRet=BinCopyFrom(pBuffer,nNewSize);
    
CNEODynamicBuffer_InsertSpaceToHead_End:
    //�ͷ���ʱ������
    if(pBuffer)
    {
        m_pMemPool->Free(pBuffer);
        pBuffer=NULL;
    }
    return bRet;
}
bool CNEODynamicBuffer::AddSpaceToTail(int nAddBytes)//�ں������հ�
{
    return SetSize(m_nDataLen+nAddBytes);
}
void CNEODynamicBuffer::CutHead(int nBytes)// ��ǰ�����һ������
{
    if(m_nDataLen<=nBytes)
        SetSize(0);
    else
    {
        //�Ӻ���ǰ�ƶ�����
        memcpy(m_pData,m_pData+nBytes,m_nDataLen-nBytes);
        m_nDataLen-=nBytes;
    }
}
void CNEODynamicBuffer::CutTail(int nBytes)//�Ӻ������һ������
{
    if(m_nDataLen<=nBytes)
        SetSize(0);
    else
        m_nDataLen-=nBytes;
}
////////////////////////////////////////
//��ֵת������
bool CNEODynamicBuffer::SetInt(int n)//��һ�������Զ����Ʒ�ʽ�������������������ֽ���
{
    int nSave=htonl(n);
    //���������ػ�����
    return BinCopyFrom((char *)&nSave,sizeof(int));
}


int CNEODynamicBuffer::GetInt(void)//��������ʽ��û���������ֵ
{
    if(!m_pData)
        return 0;
    if((sizeof(int)>(size_t)m_nDataLen))
        return 0;
    return ntohl(*(int *)m_pData);//�����ַ�ʽȡ��ǰ���ֽ�
}
bool CNEODynamicBuffer::SetShort(short n)//��һ���������Զ����Ʒ�ʽ�������������������ֽ���
{
    short sSave=htons(n);
    return BinCopyFrom((char *)&sSave,sizeof(short));
}

short CNEODynamicBuffer::GetShort(void)//�˿ں�
{
    if(!m_pData)
        return 0;
    if(sizeof(short)>(size_t)m_nDataLen)
        return 0;
    return ntohs(*(short *)m_pData);
}
bool CNEODynamicBuffer::SetChar(char n)//��һ���ֽ��Զ����Ʒ�ʽ������������
{
    return BinCopyFrom(&n,sizeof(char));
}
char CNEODynamicBuffer::GetChar(void)//���ֽڷ�ʽ��û���������ֵ
{
    if(!m_pData)
        return 0;
    if(sizeof(char)>(size_t)m_nDataLen)
        return 0;
    return *(char *)m_pData;
}
//////////////////////////////////////////
//����������׷�Ӻ���
//׷�����ݵ���󣬷����µĳ���
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
//�������ݵ���ǰ�棬�����µ����ݳ���
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
//���������ݿ�������
//������һ��Ŀ�껺�������ܴ���Ļ�������������
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
//��һ����Դ�������������ݵ���������
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

//������һ��Buffer���󿽱����ݵ�������
int CNEODynamicBuffer::BinCopyFrom(CNEODynamicBuffer *pBuffer)
{
    return BinCopyFrom(pBuffer->m_pData,pBuffer->m_nDataLen);
}
/////////////////////////////////////////////
//�ı����ݿ�����������
int CNEODynamicBuffer::StrCopyFrom(char *szString)
{
    int n=strlen(szString);
    n++;//������\0��
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
//���ݱȽϺ���
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

//��̬Buffer

CNEOStaticBuffer::CNEOStaticBuffer(CNEOMemPoolWithLock *pMemPool)
    :m_pMemPool(pMemPool),m_nDataLen(0)
{
    //m_pMemPool=pMemPool;
    //m_nDataLen=0;
}
CNEOStaticBuffer::~CNEOStaticBuffer(){}                      
///////////////////////////////
//�ߴ����ú���
bool CNEOStaticBuffer::SetSize(int nSize) //�����µĴ�С
{
    if(NEO_SAFE_BUFFER_MAX_SIZE<nSize)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::SetSize():ERROR!nSize=%d\n",nSize);////////neoqueue.cpp�а���neomem.h,neomem.h �в�������neodebug.h
        return false;
    }
    m_nDataLen=nSize;
    return true;
}
bool CNEOStaticBuffer::InsertSpaceToHead(int nAddBytes)//��ǰ�����հ�
{
    if(0>=m_nDataLen)
    {
        //���û��ԭʼ���ݣ�����Ϊ���û�������С
        m_nDataLen=nAddBytes;
        return true;
    }
    if(nAddBytes>(NEO_SAFE_BUFFER_MAX_SIZE-m_nDataLen))
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::InsertSpaceToHead();ERROR!nAddBytes=%d,m_nDataLen=%d,too big!\n",nAddBytes,m_nDataLen);
        return false;
    }
    {
    //�˴��Ĵ����ţ����޶�����szBuffer�����÷�Χ
        char szBuffer[NEO_SAFE_BUFFER_MAX_SIZE];
        memset(szBuffer,'\0',NEO_SAFE_BUFFER_MAX_SIZE);
        //��һ�Σ���ԭ�����ݿ���������
        memcpy(szBuffer+nAddBytes,m_pData,m_nDataLen);
        m_nDataLen+=nAddBytes;
        //�ڶ��Σ������������ݿ����ر����󻺳���
        memcpy(m_pData,szBuffer,m_nDataLen);
    }
    return true;
}
bool CNEOStaticBuffer::AddSpaceToTail(int nAddBytes)//�ں������հ�
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
void CNEOStaticBuffer::CutHead(int nBytes)// ��ǰ�����һ������
{
    if(0>=m_nDataLen)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutHead();ERROR!m_nDataLen=%d,too small!\n",m_nDataLen);
        m_nDataLen=0;
        return;
    }
    if(nBytes>m_nDataLen)
    {
        //�Ӻ���ǰ�ƶ�����
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutHead();ERROR!m_nDataLen=%d,nBytes=%d,too small!\n",m_nDataLen,nBytes);
        m_nDataLen=0;
        return;
    }
    m_nDataLen-=nBytes;
    //�Ӻ���ǰ����������ԭ������
    memcpy(m_pData,m_pData+nBytes,m_nDataLen);
    return;
}
void CNEOStaticBuffer::CutTail(int nBytes)//�Ӻ������һ������
{
        if(0>=m_nDataLen)
    {
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutTail();ERROR!m_nDataLen=%d,too small!\n",m_nDataLen);
        m_nDataLen=0;
        return;
    }
    if(nBytes>m_nDataLen)
    {
        //�Ӻ���ǰ�ƶ�����
        m_pMemPool->m_pDebug->DebugToFile("CNEOStaticBuffer::CutTail();ERROR!m_nDataLen=%d,nBytes=%d,too small!\n",m_nDataLen,nBytes);
        m_nDataLen=0;
        return;
    }
    m_nDataLen-=nBytes;
    return;
}
////////////////////////////////////////
//��ֵת������
bool CNEOStaticBuffer::SetInt(int n)//��һ�������Զ����Ʒ�ʽ�������������������ֽ���
{
    int nSave=htonl(n);
    //���������ػ�����
    return BinCopyFrom((char *)&nSave,sizeof(int));
}
int CNEOStaticBuffer::GetInt(void)//��������ʽ��û���������ֵ
{
    if(0>=m_nDataLen)
        return 0;
    int *pData=(int *)m_pData;
    int nRet=*pData;
    return ntohl(nRet);//�����ַ�ʽȡ��ǰ���ֽ�
}
bool CNEOStaticBuffer::SetShort(short n)//��һ���������Զ����Ʒ�ʽ�������������������ֽ���
{
    short sSave=htons(n);
    return BinCopyFrom((char *)&sSave,sizeof(short));
}

short CNEOStaticBuffer::GetShort(void)//�˿ں�
{
    if(0>=m_nDataLen)
        return 0;
    short *pData=(short *)m_pData;
    short sRet=*pData;
    return ntohs(sRet);//�����ַ�ʽȡ��ǰ���ֽ�
}
bool CNEOStaticBuffer::SetChar(char n)//��һ���ֽ��Զ����Ʒ�ʽ������������
{
    *m_pData=n;
    m_nDataLen=sizeof(char);
    return true;
}
char CNEOStaticBuffer::GetChar(void)//���ֽڷ�ʽ��û���������ֵ
{
    return *m_pData;
}
//////////////////////////////////////////
//����������׷�Ӻ���
//׷�����ݵ���󣬷����µĳ���
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
//�������ݵ���ǰ�棬�����µ����ݳ���
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
//���������ݿ�������
//������һ��Ŀ�껺�������ܴ���Ļ�������������
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
    //ִ�����ǵĿ����߼�
    memcpy(szBuffer,m_pData,m_nDataLen);
    return m_nDataLen;
}
//��һ����Դ�������������ݵ���������
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
    //��������
    memcpy(m_pData,szData,nDataLen);
    m_nDataLen=nDataLen;
    return m_nDataLen;
}
//������һ��Buffer���󿽱����ݵ�������
int CNEOStaticBuffer::BinCopyFrom(CNEODynamicBuffer *pBuffer)
{
    return BinCopyFrom(pBuffer->m_pData,pBuffer->m_nDataLen);
}
/////////////////////////////////////////////
//�ı����ݿ�����������
int CNEOStaticBuffer::StrCopyFrom(char *szString)
{
    int n=strlen(szString);
    n++;//������\n��
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
//���ݱȽϺ���
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
//������
bool CNEOStaticBuffer::IHaveData(void)
{
    if(0>=m_nDataLen)
        return false;
    return true;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//��������ģ��

CNEOPopBuffer::CNEOPopBuffer(char *szBuffer,           //������ָ��
    int nBufferSize,                    //�������ߴ�
    bool bInitFlag)                //�Ƿ��ʼ����־
{
    m_pHead=NULL;
    m_pBuffer=NULL;
    m_nBufferSize=nBufferSize;
    Set(szBuffer,nBufferSize);
    if(bInitFlag)//��Ҫ��ʼ��
        Clean();//����ն���
}
CNEOPopBuffer::~CNEOPopBuffer(){}
//ʵ��ճ�ϵķ���,�ⲿ����Ļ��������ڲ���������ָ��ҽ�
void CNEOPopBuffer::Set(char *szBuffer,int nBufferSize)
{
    m_pBuffer=szBuffer;
    m_nBufferSize=nBufferSize;
    m_pHead=(SNEOPopBufferHead*)m_pBuffer;
}
//������������������㣬���������ͷ�
void CNEOPopBuffer::Clean(void)
{
    if(m_pHead)
    {
        m_pHead->m_nTokenCount=0;
        m_pHead->m_nAllByteSCount=SNEOPopBufferHeadSize;
    }
}
//�ڲ���Ϣ��ӡ����
void CNEOPopBuffer::PrintInside()
{
    if(!ICanWork())
    {
        NEO_PRINTF("CNEOPopBuffer::PrintInside();ERROR!m_pBuffer=null\n");
        return;
    }
    //���忪ʼ��ָ��
    char *pTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);
    // ����Ԫ�ص�ͷָ��
    SNEOPopBufferTokenHead *pTokenHead=(SNEOPopBufferTokenHead*)pTokenBegin;
    char *pTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pTokenBegin);
    int i=0;
    //Ԥ��ӡ�������е���Ϣ
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
//�ܷ���ȷ�����ı�־����
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
//�������Ĺ��ܣ�׷�ӵ�ĩβ��
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
    {//�������޶�
        char *pTokenBegin=m_pBuffer+m_pHead->m_nAllByteSCount;//��m_nAllByteSCount���������β��ָ��
        SNEOPopBufferTokenHead *pTokenHead=(SNEOPopBufferTokenHead*)pTokenBegin;//ǿ��ָ������ΪTokenͷָ�룻
        char *pTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pTokenBegin);//���Token��������ָ��
        //������Ӷ���
        pTokenHead->m_nDataLen=nDataLength;//���ȸ�ֵ
        memcpy(pTokenData,szData,nDataLength);
        m_pHead->m_nTokenCount++;//Ԫ�ظ�����һ
        m_pHead->m_nAllByteSCount+=NEO_POP_BUFFER_TOKEN_LENGTH(nDataLength);
        nRet=nDataLength;//����ֵ�Ĵ����ݳ���
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
//��õ�ǰ�ڲ�Ԫ�ظ���
int CNEOPopBuffer::GetTokenCount(void)
{
    if(!ICanWork())
        return 0;
    return m_pHead->m_nTokenCount;
}
//������е��ֽ���
int CNEOPopBuffer::GetAllBytes(void)
{
    if(!ICanWork())
        return 0;
    return m_pHead->m_nAllByteSCount;
}
//�ж��ڲ�ʣ��ռ��Ƿ���,�Ƿ��¼����������
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
//��õ�һ��Ԫ�صĳ���
int CNEOPopBuffer::GetFirstTokenLength(void)
{
    if(!ICanWork())
        return 0;
    char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);//�����һ��token��ʼ��
    SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//ǿ��ת��
    return pFirstTokenHead->m_nDataLen;
}
//��ȡ��һ��Ԫ��
int CNEOPopBuffer::GetFirst(char *szBuffer,int nBufferSize)
{
    int nRet=0;
    if(!ICanWork())
        goto CNEOPopBuffer_GetFirst_End;
    //�ж������Ƿ�Ϊ��
    if(!GetTokenCount())
        goto CNEOPopBuffer_GetFirst_End;
    //�ж������Ĳ����Ƿ�Ϸ�
    if(GetFirstTokenLength()>nBufferSize)
        goto CNEOPopBuffer_GetFirst_End;
    {//�������޶�
        char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);//�����һ��token��ʼ��
        SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//ǿ��ת��
        char *pFirstTokenData=NEO_POP_BUFFER_TOKEN_DATA_BEGIN(pFirstTokenBegin);
        memcpy(szBuffer,pFirstTokenData,pFirstTokenHead->m_nDataLen);//�������ݵ�������
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
//ɾ��һ��Ԫ��
bool CNEOPopBuffer::DeleteFirst(void)
{
    bool bRet=false;
    if(!ICanWork())
        goto CNEOPopBuffer_DeleteFirst_End;
    //�ж������Ƿ�Ϊ��
    if(!GetTokenCount())
        goto CNEOPopBuffer_DeleteFirst_End;
    {//�������޶�
        char *pFirstTokenBegin=NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(m_pBuffer);//�����һ��token��ʼ��
        SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//ǿ��ת��
        int nFirstTokenLen=NEO_POP_BUFFER_TOKEN_LENGTH(pFirstTokenHead->m_nDataLen);//��һ��token�ܳ���
        char *pSecondTokenBegin=pFirstTokenBegin+nFirstTokenLen;//����ڶ���token�����
        int nCopyBytesCount=m_pHead->m_nAllByteSCount-SNEOPopBufferHeadSize-nFirstTokenLen;//��Ҫ�ƶ����ֽ���
        memcpy(pFirstTokenBegin,pSecondTokenBegin,nCopyBytesCount);
        m_pHead->m_nAllByteSCount-=nFirstTokenLen;
        m_pHead->m_nTokenCount--;
        bRet=true;
    }
CNEOPopBuffer_DeleteFirst_End:
    return bRet;
}
//�Ӷ����ܵ�����һ��Ԫ��
int CNEOPopBuffer::GetAndDeleteFirst(char *szBuffer,int nBufferSize)
{
    if(!ICanWork())
        return 0;
    int nRet=GetFirst(szBuffer,nBufferSize);//��õ�һ��Ԫ��
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
//ö�ٱ����������ݣ��ύ�ص�����������ɾ������
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
        SNEOPopBufferTokenHead *pFirstTokenHead=(SNEOPopBufferTokenHead*)pFirstTokenBegin;//ǿ��ת��
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
//��̬�ڴ���䣬���в����б��(�����ڴ��)
CNEOMemQueue::CNEOMemQueue(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,
    const char *szAppName,                    // Ӧ�ó����������������
    int nMaxToken  //���token����
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
        DeleteATokenAndHisNext(m_pHead);//�ݹ��ͷ�
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
void CNEOMemQueue::CleanAll(void)              //�������token
{
    if(!ICanWork())
        return;
    while(DeleteFirst()){}       //ѭ��ɾ����һ������֪������Ϊ��
}
int CNEOMemQueue::GetFirstLength(void)        // ��õ�һ��token���ݳ���
{
    int nRet=0;
    if(m_pHead)                  //ICAnWorkΪ��m_pHead��ΪNULL
    {
        nRet=m_pHead->m_nDataLen;
    }
    return nRet;
}
int CNEOMemQueue::GetTokenCount(void)        //���token����
{
    return m_nTokenCount;
}
void CNEOMemQueue::PrintInside(void)         //��ӡ���ж����ڲ�token����
{
    if(!ICanWork())
        return;
    NEO_PRINTF("Queue:Token Count=%d,Head=0x%p,Last=0x%p\n",
        m_nTokenCount,m_pHead,m_pLast);
    if(m_pHead)
        PrintAToken(m_pHead);//�Ӷ���ͷ��ʼ�ݹ�
}
int CNEOMemQueue::AddLast(const char *szData       //����ָ��
    ,int nDataLen,             //���ݳ���
    int nLimit)              //��ֹ�ݹ鳤�ȹ���
{          
    int nRet=0;
    if(!ICanWork())
        goto CNEOMemQueue_AddLast_End;
    if(0>=nLimit)               //Ӧ������ֵ
    {
        // ����������
        if(m_nMaxToken<=m_nTokenCount)//������ʱ����m_nMaxToken��Ϊ�߽�����
            goto CNEOMemQueue_AddLast_End;
    }//if
    else
    {
        //��������
        if(nLimit<=m_nTokenCount) //�����nlimit����ʹ�������������
            goto CNEOMemQueue_AddLast_End;
    }
    if(!m_pHead)
    {
        m_pHead=GetAToken();//��ͷ�ĵ�һ�γ�ʼ��
        if(!m_pHead)
        {
            m_pDebug->DebugToFile("%s::AddLast():malloc m_pHead fail \n",m_szAppName);
            goto CNEOMemQueue_AddLast_End;
        }

    }
    if(m_pLast)//�����������ã�ֱ������
    {
        nRet=AddLastToThisToken(m_pLast,szData,nDataLen);
    }
    else if(m_pHead)//����������ֵ����ͳģʽ����������
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
    if(!m_pHead)                       //��������Ƿ�������
        goto CNEOMemQueue_GetFirst_End;
    if(!m_pHead->m_pBuffer)           //����һ��Token�Ƿ�������
    {
        m_pDebug->DebugToFile("%s::GetFirst():m_pHead->m_pBuffer=NULL\n",m_szAppName);
        goto CNEOMemQueue_GetFirst_End;
    }
    if(m_pHead->m_nDataLen>nBufferSize)//�������Ļ������Ƿ��㹻
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
    SNEOQueueTokenHead *pSecond=NULL;//ָ��ڶ���Token����ʱָ��
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
        if(!m_pHead)//����������ɾ�գ��������������
            m_pLast=NULL;
    }
    else//ɾ��ʧ��
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
    
//��ǰ�浯��һ�����ݣ�
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
//��popbuffer�������������������ݣ�׷�ӵ�����β
int CNEOMemQueue::PushToLast(char *szData,int nDataLen)
{
    int nRet=0;
    CNEOPopBuffer cPopBuffer(szData,nDataLen,false);
    if(!ICanWork())
        goto CNEOMemQueue_PushToLast_End;

    //�˴�����popbuffer�ĺ���ɾ��˵�����ݣ���ʹ�ûص�������������
    //�ص������ڴ˴�����CNEOMemQueue���󣨱�����),�����øö����addlast��
    //���ν���������memqueue
    cPopBuffer.MoveAllData(PushDataCallback,this);
CNEOMemQueue_PushToLast_End:
    return nRet;
}
    //��������д�����
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
//�������ݴӴ��̶���
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
        //��ʼ���� token ����
        n=fread((void *)pTempBuffer,sizeof(char),nDataLen,fp);
        if(nDataLen>n)
        {
            m_pDebug->DebugToFile("%s::ReadFromFile():read data  fail!\n",m_szAppName);
            goto CNEOMemQueue_ReadFromFile_End;
        }
        //����ɹ�
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
        Debug_bin(pToken->m_pBuffer,pToken->m_nDataLen);                         ///��Ҫ����Debug�ļ�
    if(pToken->m_pNext)//�ݹ�
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
    //д�����
    fwrite((void *)&(pToken->m_nDataLen),sizeof(int),1,fp);
    fwrite((void *)pToken->m_pBuffer,sizeof(char),pToken->m_nDataLen,fp);
    if(pToken->m_pNext)//�ݹ鵽��һ��token
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
        //����buffer װ����
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    if(!DeleteFirst())
    {
        m_pDebug->DebugToFile("%s::PopFromFirst4NEOPopBuffer():DeleteFirst fail\n",m_szAppName);
        goto CNEOMemQueue_PopFromFirst4NEOPopBuffer_End;
    }
    if(m_pHead)//�ݹ飬��������
        nRet+=PopFromFirst4NEOPopBuffer(pPopBuffer);
CNEOMemQueue_PopFromFirst4NEOPopBuffer_End:
    return nRet;
}
//���������洢���ݵ��ڴ�
int CNEOMemQueue::AddLastToThisToken(SNEOQueueTokenHead *pToken,const char *szData,int nDataLen)
{
    int nRet=0;
    char szNameBuffer[256];
    if(!ICanWork())
        goto CNEOMemQueue_AddLastToThisToken_End;
    //ָ�벻Ϊ�գ�˵���������ݣ�����nextָ�봦
    if(!pToken->m_pBuffer)
    {
        //�����Token��������Ч���ݣ��򱣴浽�Լ�
        SafePrintf(szNameBuffer,256,"%s::pToken->m_pBuffer",m_szAppName);  //��ʽ��˵������
        pToken->m_pBuffer=(char *)m_pMemPool->Malloc(nDataLen,szNameBuffer);//���ڴ���������ݿ�
        if(!pToken->m_pBuffer)
        {
        //����ʧ��
            m_pDebug->DebugToFile("%s::AddLastToThisToken():Malloc new Token fail!\n",m_szAppName);
            goto CNEOMemQueue_AddLastToThisToken_End;
        }
        memcpy(pToken->m_pBuffer,szData,nDataLen); //����ҵ�����ݵ��ڴ��
        pToken->m_nDataLen=nDataLen;
        nRet=nDataLen;
        m_pLast=pToken;
        goto CNEOMemQueue_AddLastToThisToken_End;      
    }//if
    else
    {
        //nextΪ�գ�����next�������
        if(!pToken->m_pNext)
        {
            //����¼ҵ���ָ��Ϊ�գ�������GetAToken����ͷ
            pToken->m_pNext=GetAToken();
            if(!pToken->m_pNext)
            {
                //����ʧ�ܱ���
                m_pDebug->DebugToFile("%s::AddLastToThisToken():malloc pToken->m_pNext fail!!\n",m_szAppName);
                goto CNEOMemQueue_AddLastToThisToken_End;
            }
        }
        if(pToken->m_pNext)//�ݹ����
            nRet=AddLastToThisToken(pToken->m_pNext,szData,nDataLen);
    }
CNEOMemQueue_AddLastToThisToken_End:
    return nRet;
}
//����һ��tokenͷ
SNEOQueueTokenHead *CNEOMemQueue::GetAToken(void)//����ָ�룬������˭����˭�ͷŵ�ԭ��
{
    SNEOQueueTokenHead *pToken=NULL;
    char *pTokenBuffer=NULL;                 //��ʱָ��
    char szNameBuffer[NEO_APPLICATION_NAME_SIZE];//˵�����ֻ�����
    if(!ICanWork())
        goto CNEOMemQueue_GetAToken_End;
    SafePrintf(szNameBuffer,256,"%s::Token_Head",m_szAppName);

    //��ʼ����Tokenͷ�ڴ��
    pTokenBuffer=(char *)m_pMemPool->Malloc(SNEOQueueTokenHeadSize,szNameBuffer);
    if(!pTokenBuffer)
    {
        m_pDebug->DebugToFile("%s::GetAToken:Malloc new Token fail!\n",m_szAppName);
        goto CNEOMemQueue_GetAToken_End;
    }
    //ǿ��ָ��ת��
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
    if(pToken->m_pNext)//�ݹ�ɾ��
    {
        DeleteATokenAndHisNext(pToken->m_pNext);
        pToken->m_pNext=NULL;
    }
    if(pToken->m_pBuffer)//��ʼɾ��������
    {
        m_pMemPool->Free(pToken->m_pBuffer);//���ڴ滺����ͷ����ݻ���
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
//�ص���������
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
//����ȫ������
CNEOMemQueueWithLock::CNEOMemQueueWithLock(CNEOLowDebug *pDebug,
    CNEOMemPoolWithLock *pMemPool,
    const char *szAppName,                    // Ӧ�ó����������������
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
//////////////////////////////////////////
#include "neodebug.h"
      
//#include <stdarg.h> 
//#include <Windows.h>
//#include <stdio.h>
//#include <time.h>
//#include <stdlib.h>
/*ͷ�ļ�: ����#include <stdarg.h> ������������: ����int _vsnprintf(char *buffer, size_t max_count, const char *format, va_list vArgList); ��������˵��: ����1. char *buffer [out],�����ɵĸ�ʽ�����ַ������������. ����2. size_t max_count [in], buffer�ɽ��ܵ�����ֽ���,��ֹ��������Խ��. ����3. const char *format [in], ��ʽ���ַ��� ����4. va_list vArgList [in], va_list����. va:variable-argument:�ɱ���� �����÷�������vsprintf,ֻ��������max_count������. ��������ֵ˵��: ��������ɹ����ô˺���������д��buffer�е��ַ��ĸ�������������β��'\0'����snprintf��vsnprintf�������ܹ�д����size��С��������β��'0'�����ֽ�������������Ϊ����ԭ�򱻽ضϣ����سɹ�д��buffer���ַ�������������β��'\0'����������㹻���ڴ�ռ�Ļ������ԣ��������ֵ����size���ߴ���size����ʾ�����buffer���ַ����ضϣ����������������������򷵻�һ��������
��VA_LIST���÷��� VA_LIST ����C�����н����������һ��꣩����1�������ں����ﶨ��һ��VA_LIST�͵ı��������������ָ�������ָ�� ������2��Ȼ����VA_START���ʼ�������ն����VA_LIST�����������ĵڶ��� ���������ǵ�һ���ɱ������ǰһ����������һ���̶��Ĳ���������������VA_START ����(ap,v)�Ժ�apָ���һ���ɱ�����ڶ�ջ�ĵ�ַ���� ������3��Ȼ����VA_ARG���ؿɱ�Ĳ�����VA_ARG�ĵڶ�����������Ҫ���صĲ��������͡� ������4�������VA_END������ɱ�����Ļ�ȡ��Ȼ����Ϳ����ں�����ʹ�õڶ��������ˡ� ������������ж���ɱ�����ģ����ε���VA_ARG��ȡ����������*/
//#define WIN_LINUX_vsnprintf _vsnprintf
//#define DEBUG_BUFFER_LENGTH 1024
//#define NEO_DEBUG_FILENAME_LENGTH 256
////////////////////////////////////////////////////////////
//#define PATH_CHAR "\\"//���ڽ��linux��windows·��������Ĳ���
///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
namespace NEOLIB {
int SafePrintf(char *szBuf,int nMaxlength,const char *szFormat,...)
{
    int nListCount=0;
    va_list pArgList;
    if(!szBuf)
        goto SafePrint_END;

    va_start(pArgList,szFormat);
    //���溯���������ַ����������ֵʱ����ֵΪ-1
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,
        nMaxlength-nListCount,szFormat,pArgList);
    va_end(pArgList);

    if(nListCount>(nMaxlength-1))
        nListCount=nMaxlength-1;

    if(nListCount<0)
        nListCount=nMaxlength-1;

    *(szBuf+nListCount)='\0';

    SafePrint_END:
    return nListCount;

}
int GetTimeStamp(char *szBuf,int nMaxlength)
{
    time_t t;
    struct tm *pTM=NULL;
    int nLength=0;
    time(&t);
    pTM=localtime(&t);
    nLength=SafePrintf(szBuf,nMaxlength,"%s",asctime(pTM));//asctime����س�Ҫ���˵�
    szBuf[nLength-1]='\0';
    return nLength;
}
//ͬʱ��ӡ�ַ�����Ļ���ļ�
int OutputFileOrScreen(const char *szFileName,const char *szMode,const char *szFormate,...)
{
    char szBuf[DEBUG_BUFFER_LENGTH] = "\0";//debugʱҪ����DEBUG_BUFFER_LENGTH
    char szTime[256] = "\0";
    int nListCount=0;

    va_list pArgList;
    va_start(pArgList,szFormate);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,DEBUG_BUFFER_LENGTH-nListCount,szFormate,pArgList);
    va_end(pArgList);

    if(nListCount>(DEBUG_BUFFER_LENGTH-1))
        nListCount=DEBUG_BUFFER_LENGTH-1;

    if(nListCount<0)
        nListCount=DEBUG_BUFFER_LENGTH-1;

    *(szBuf+nListCount)='\0';
    GetTimeStamp(szTime,256);
    FILE *fp;
    fp=fopen(szFileName,szMode);
    if(fp)
    {
        nListCount=fprintf(fp,"[%s]%s\r\n",szTime,szBuf);
        NEO_PRINTF("[%s]%s\r\n",szTime,szBuf);
        fclose(fp);
    }
    else
        nListCount=0;
    return nListCount;
}

//��ַ�롪��ַ���16���Ʊ�ʾ��ASC�ַ���ʾ
int Debug_bin_ASC(char *pPrintBuffer,const char *pBuffer,int nLength)
{
    int i=0;
    int nCount=0;
    for(i=0;i<nLength;i++)
    {
       if(32<=*(pBuffer+i))
           nCount+=SafePrintf(pPrintBuffer+nCount,256,"%c",*(pBuffer+i));
       else//������ʾ�ַ���.����
           nCount+=SafePrintf(pPrintBuffer+nCount,256,".");
    }
    return nCount;
}
int Debug_bin_HEX(char *pPrintBuffer,const char *pBuffer,int nLength)
{
    int i=0;
    int j=0;
    int nCount=0;
    for(i=0;i<nLength;i++)
    {
        nCount+=SafePrintf(pPrintBuffer+nCount,256,"%02X",(unsigned char)*(pBuffer+i));
        j++;
        if(4==j)//�ĸ��ֽ���һ��32λָ��,
        {
          j=0;
          nCount+=SafePrintf(pPrintBuffer+nCount,256," ");//���ָ���Ŀո�
        }
        
    }//for
    if(16>nLength)
    {
        for(;i<16;i++)//������С��16��16-i���ֽ��ÿո���
        {
            nCount+=SafePrintf(pPrintBuffer+nCount,256,"  ");//1���ֽ��������ո����
          j++;
          if(4==j)//�ĸ��ֽ���һ��32λָ��,��������ո�
          {
             j=0;
             nCount+=SafePrintf(pPrintBuffer+nCount,256," ");
          }
        }//for
    }
    return nCount;
}

void Debug_bin(const char *pBuffer,int nLength)
{
    int nAddr=0;//��ǰָ���ƫ����
    int nLineCount=0;
    int nBufferCount=nLength;
    int n=0;
    char szLine[256];//�л���
    if(0<nLength)
    {
       while(1)
       {
          n=0;
          n+=SafePrintf(szLine+n,256-n,"%p-",pBuffer+nAddr);//����ǰ�ڴ��ַ���
          nLineCount=16;
          if(nBufferCount<nLineCount)
              nLineCount=nBufferCount;
          n+=Debug_bin_HEX(szLine+n,pBuffer+nAddr,nLineCount);//16���� ÿ��16���ֽ� ����32���ַ�
          n+=Debug_bin_ASC(szLine+n,pBuffer+nAddr,nLineCount);//asc��  ÿ��16���ֽ� ����16���ַ�
          printf("%s\n",szLine);//ÿ�����16���ֽ�
          nAddr+=16;
          nBufferCount-=16;//����ѭ��
          if(0>=nBufferCount) 
              break;
       }//while
       printf("\n");
    }
    else
       printf("Debug_bin error length=%d\n",nLength);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//��ȡ��ֵ�ĺ���
unsigned long SCountSubGetX(SCountSub &CountSub)
{
    return CountSub.m_nEnd-CountSub.m_nBegin;
}
void SCountSubSetBegin(SCountSub &CountSub,unsigned long n)
{
    CountSub.m_nBegin=n;
}
unsigned long SCountSubSetEnd(SCountSub &CountSub,unsigned long n)
{
    CountSub.m_nEnd=n;
    return SCountSubGetX(CountSub);
}
//�����ֵ����
CCountSub::CCountSub()
    {
       m_CountSub.m_nBegin=0;
       m_CountSub.m_nEnd=0;
    }
CCountSub::~CCountSub(){}

unsigned long CCountSub::SetBegin(unsigned long n)
{	
    m_CountSub.m_nBegin = n;
	return m_CountSub.m_nBegin;
}
unsigned long CCountSub::SetEnd(unsigned long n)
{
	m_CountSub.m_nEnd = n;
    return m_CountSub.m_nEnd;
}
unsigned long CCountSub::GetBegin(void)
{
    return m_CountSub.m_nBegin;
}
unsigned long CCountSub::GetEnd(void)
{
    return m_CountSub.m_nEnd;
}
unsigned long CCountSub::GetX(void)
{
    return m_CountSub.m_nEnd-m_CountSub.m_nBegin;
}
void CCountSub::E2B(void)
{
    m_CountSub.m_nBegin=m_CountSub.m_nEnd;
}
void CCountSub::Push(unsigned long ulNew)//����ѭ��
{
    E2B();
    m_CountSub.m_nEnd=ulNew;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
//ƽ��ֵ���㣨ÿ�뷢�����ٴζ�����
CDeltaTime::CDeltaTime()
{
    TouchBegin();
}
CDeltaTime::~CDeltaTime(){}


void CDeltaTime::Reset(void)//Begin=End
{
    m_CountSub.SetEnd(m_CountSub.SetBegin((unsigned long )time(NULL)));
}
void CDeltaTime::TouchBegin(void)
{
    m_CountSub.SetBegin((unsigned long)time(NULL));
}
void CDeltaTime::TouchEnd(void)
{
    m_CountSub.SetEnd((unsigned long)time(NULL));
}
unsigned long CDeltaTime::GetDeltaT(void)//��ò�ֵ
{
    return m_CountSub.GetX();
}
double CDeltaTime::GetOperationsPerSecond(unsigned long ulOperationCount/*�ڼ�Ĳ����ܺ�*/)
{
    double dRet=0.0;
    double dCount=(double)ulOperationCount;
    unsigned long ulDeltaSecond=GetDeltaT();
    double dSeconds=(double)ulDeltaSecond;
    if(0==ulDeltaSecond)
        return dRet;
    return dCount/dSeconds;
}

/////////////////////////////////////
/////////////////////////////////////
////////////////////////////////////
//ͳ��ƽ��ֵ
//SCountSetBegin��ʼͳ��
//SCountSubSetEnd��ε�����ˢ�½���ʱ��
//SCountSum���ͳ��ƽ��ֵ
void SCountReset(SCount &Count)
{
    Count.m_Sum=0;
    Count.m_Sub.m_nBegin=0;
    Count.m_Sub.m_nEnd=0;
}
unsigned long SCountSum(SCount &Count)//ƽ��ֵ��ƽ��ֵ
{
    unsigned long X=SCountSubGetX(Count.m_Sub);
    if(0==Count.m_Sum)
        Count.m_Sum=X;
    else
        Count.m_Sum=(Count.m_Sum+X)/2;
    return Count.m_Sum;
}
unsigned long SCountGetSum(SCount &Count)
{
    return Count.m_Sum;
}
unsigned long SCountGetX(SCount &Count)
{
    return SCountSubGetX(Count.m_Sub);
}
void SCountSetBegin(SCount &Count,unsigned long n)
{
    SCountSubSetBegin(Count.m_Sub,n);
}
unsigned long SCountSetEnd(SCount &Count,unsigned long n)
{
    return SCountSubSetEnd(Count.m_Sub,n);
}
//////////////////////////////////////
//����ƽ��ֵ����

CCount::CCount()
{
    SCountReset();
}
CCount::~CCount(){}

void CCount::SCountReset(void)
{
    NEOLIB::SCountReset(m_Count);
}
unsigned long CCount::SCountSum(void)
{
    return NEOLIB::SCountSum(m_Count);
}
unsigned long CCount::SCountSetSum(unsigned long n)
{
    m_Count.m_Sum=n;
    return n;
}
unsigned long CCount::SCountGetSum(void)
{
    return NEOLIB::SCountGetSum(m_Count);
}
unsigned long CCount::SCountGetX(void)
{
    return NEOLIB::SCountGetX(m_Count);
}
void CCount::SCountSetBegin(unsigned long n)
{
    NEOLIB::SCountSetBegin(m_Count,n);
}
unsigned long CCount::SCountSetEnd(unsigned long n)
{
    return NEOLIB::SCountSetEnd(m_Count,n);
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//��������
inline int GetNot0(void)
{
    srand((unsigned)time(NULL));
    int nRet=rand();
    if(!nRet)
        nRet++;
    return nRet;
}
inline int Get0(void)
{
    srand((unsigned)time(NULL));
    int nRet=rand();
    return nRet^nRet;
}

//��ø�����Χ�ڵ������
inline int GetRandomBeteen(int nBegin,int nEnd)
{
    int n=GetNot0();
    int nBetween=0;
    if(0>nBegin)
        nBegin=-nBegin;
    if(0>nEnd)
        nEnd=-nEnd;
    if(nBegin>nEnd)
    {
        nBetween=nEnd;
        nEnd=nBegin;
        nBegin=nBetween;
    }
    else if(nBegin==nEnd)
        nEnd=nBegin+10;
    nBetween=nEnd-nBegin;
    n=n%nBetween;
    n+=nBegin;
    return n;
}
////�ж�ʱ���Ƿ���
//#define TimeSetNow(t) time(&t)
//inline bool TimelsUp(time_t tLast/*���һ��ʱ��*/,long lMax/*��ʱʱ������*/)
//{
//  time_t tNow;
//  TimeSetNow(tNow);
//  long lDeltaT=(long)tNow-(long)tLast;
//  if(lMax<=lDeltaT)
//      return true;
//  else
//      return false;
//}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//CLowDebug�࣬���ھ��幦�ܵ����

    //ɾ��һ���ļ�
void CNEOLowDebug::DeleteAFile(const char *szFileName)
{
    remove(szFileName);
}
//����·����ȡ�ļ���
char *CNEOLowDebug::GetTrueFileName(char *szBuffer)
{
    char *pRet=szBuffer;
    int nLen=strlen(szBuffer);
    int i=0;
    for(i=nLen-1;i>=0;i--)
    {
        //��������һ���ָ���������Ϊ�ļ���
        if('\\'==*(szBuffer+i))
        {
            pRet=(szBuffer+i+1);
            break;
        }
        if('/'==*(szBuffer+i))
        {
            pRet=(szBuffer+i+1);
            break;
        }

    }
    return pRet;//����ָ�루���ף�
}

//����ַ���DebugToFile���ļ������̨�������ַ�����������\0
int CNEOLowDebug::DebugToFile(const char *szFormat,...)
{
    char szBuff[DEBUG_BUFFER_LENGTH] = "\0";                                //�������
    char szTemp[DEBUG_BUFFER_LENGTH] = "\0";                               //ʱ����û����м�buffer
    char szTime[DEBUG_BUFFER_LENGTH] = "\0";                              //ʱ���buff
    FILE *fp=NULL;
    int nListCount=0;
    va_list pArgList;
    //time_t t;
    //struct tm *pTM=NULL;
    //int nLength=0;
    //����ʱ���
    GetTimeStamp(szTemp,DEBUG_BUFFER_LENGTH);
    SafePrintf(szTime,DEBUG_BUFFER_LENGTH,"[%s]",szTemp);
    ////////////////////////////////////////////////////
    m_Lock.Lock();
    {
        va_start(pArgList,szFormat);
        nListCount+=WIN_LINUX_vsnprintf(szBuff+nListCount,DEBUG_BUFFER_LENGTH-nListCount,szFormat,pArgList);
        va_end(pArgList);
        if(nListCount>(DEBUG_BUFFER_LENGTH-1))
            nListCount=DEBUG_BUFFER_LENGTH-1;
        *(szBuff+nListCount)='\0';
        //������ļ�
        fp=fopen(m_szFileName,"a+");
        if(fp)
        {
            nListCount=fprintf(fp,"%s%s",szTime,szBuff);
            if(m_bePrintToScreenFlag)
            {
                //�������Ļ
            NEO_PRINTF("%s%s",szTime,szBuff);
            if(pInfoOutCallback)
            {
                //�ص��������ϲ����
                char szInfoOut[DEBUG_BUFFER_LENGTH];
                SafePrintf(szInfoOut,DEBUG_BUFFER_LENGTH,"%s%s",szTime,szBuff);
                pInfoOutCallback(szInfoOut,pInfoOutCallbackParam);
            }

            }
            fclose(fp);
        }
        else
        {
            nListCount=0;
        }

    }
    m_Lock.UnLock();
    return nListCount;

}
//���һ���ڴ�ε��ļ���
void CNEOLowDebug::DebugToFileBin(char *pBuffer,int nLength)
{
    m_Lock.Lock();
    {
        OutputFileOrScreen(m_szFileName,"a+",pBuffer,nLength);
        if(m_bePrintToScreenFlag)
            Debug_bin(pBuffer,nLength);
    }
    m_Lock.UnLock();
}

//���캯������������
CNEOLowDebug::CNEOLowDebug (const char *szPathName,                //·����
    const char *szAppName,                           //�ļ���
    _APP_INFO_OUT_CALLBACK pInfoOutCallback,//���������ص�����
    void *pInfoOutCallbackParam,          //�ص���������
    bool bePrintToScreenFlag          //�Ƿ��ӡ����Ļ
    ):pInfoOutCallback(pInfoOutCallback),pInfoOutCallbackParam(pInfoOutCallbackParam),
    m_bePrintToScreenFlag(bePrintToScreenFlag),m_Lock()
{
    if(szAppName)
    {
        FULL_NAME(szPathName,szAppName,m_szFileName,"dbg");
    }
    else
        m_szFileName[0] = '\0';

    DeleteAFile(m_szFileName);
    DebugToFile("CNEOLowDebug:Start!\n");
}

CNEOLowDebug::~CNEOLowDebug()
{
    DebugToFile("CNeoLowDebug:stop\n");
}

}
//#else 
//#define WIN_LINUX_vsnprintf vsprintf
//#define PATH_CHAR "/"
//#endif
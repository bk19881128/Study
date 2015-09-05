//////////////////////////////////////////
#include "neodebug.h"
      
//#include <stdarg.h> 
//#include <Windows.h>
//#include <stdio.h>
//#include <time.h>
//#include <stdlib.h>
/*头文件: 　　#include <stdarg.h> 　　函数声明: 　　int _vsnprintf(char *buffer, size_t max_count, const char *format, va_list vArgList); 　　参数说明: 　　1. char *buffer [out],把生成的格式化的字符串存放在这里. 　　2. size_t max_count [in], buffer可接受的最大字节数,防止产生数组越界. 　　3. const char *format [in], 格式化字符串 　　4. va_list vArgList [in], va_list变量. va:variable-argument:可变参数 　　用法类似于vsprintf,只不过加了max_count的限制. 　　返回值说明: 　　如果成功调用此函数，返回写到buffer中的字符的个数（不包括结尾的'\0'）。snprintf和vsnprintf函数不能够写多于size大小（包括结尾的'0'）的字节数。如果输出因为以上原因被截断，返回成功写入buffer的字符数（不包括结尾的'\0'），如果有足够的内存空间的话。所以，如果返回值等于size或者大于size，表示输出到buffer的字符被截断，如果输出过程中遇到错误，则返回一个负数。
　VA_LIST的用法（ VA_LIST 是在C语言中解决变参问题的一组宏）：（1）首先在函数里定义一具VA_LIST型的变量，这个变量是指向参数的指针 　　（2）然后用VA_START宏初始化变量刚定义的VA_LIST变量，这个宏的第二个 　　参数是第一个可变参数的前一个参数，是一个固定的参数。（如在运行VA_START 　　(ap,v)以后，ap指向第一个可变参数在堆栈的地址。） 　　（3）然后用VA_ARG返回可变的参数，VA_ARG的第二个参数是你要返回的参数的类型。 　　（4）最后用VA_END宏结束可变参数的获取。然后你就可以在函数里使用第二个参数了。 　　如果函数有多个可变参数的，依次调用VA_ARG获取各个参数。*/
//#define WIN_LINUX_vsnprintf _vsnprintf
//#define DEBUG_BUFFER_LENGTH 1024
//#define NEO_DEBUG_FILENAME_LENGTH 256
////////////////////////////////////////////////////////////
//#define PATH_CHAR "\\"//用于解决linux与windows路径间隔符的差异
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
    //下面函数当拷贝字符串超过最大值时返回值为-1
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
    nLength=SafePrintf(szBuf,nMaxlength,"%s",asctime(pTM));//asctime输出回车要过滤掉
    szBuf[nLength-1]='\0';
    return nLength;
}
//同时打印字符到屏幕或文件
int OutputFileOrScreen(const char *szFileName,const char *szMode,const char *szFormate,...)
{
    char szBuf[DEBUG_BUFFER_LENGTH] = "\0";//debug时要定义DEBUG_BUFFER_LENGTH
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

//地址码—地址码的16进制表示—ASC字符表示
int Debug_bin_ASC(char *pPrintBuffer,const char *pBuffer,int nLength)
{
    int i=0;
    int nCount=0;
    for(i=0;i<nLength;i++)
    {
       if(32<=*(pBuffer+i))
           nCount+=SafePrintf(pPrintBuffer+nCount,256,"%c",*(pBuffer+i));
       else//不可显示字符以.代替
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
        if(4==j)//四个字节是一个32位指针,
        {
          j=0;
          nCount+=SafePrintf(pPrintBuffer+nCount,256," ");//输出指针间的空格
        }
        
    }//for
    if(16>nLength)
    {
        for(;i<16;i++)//若长度小于16，16-i个字节用空格补上
        {
            nCount+=SafePrintf(pPrintBuffer+nCount,256,"  ");//1个字节用两个空格代替
          j++;
          if(4==j)//四个字节是一个32位指针,用于输出空格
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
    int nAddr=0;//当前指针的偏移量
    int nLineCount=0;
    int nBufferCount=nLength;
    int n=0;
    char szLine[256];//行缓冲
    if(0<nLength)
    {
       while(1)
       {
          n=0;
          n+=SafePrintf(szLine+n,256-n,"%p-",pBuffer+nAddr);//将当前内存地址输出
          nLineCount=16;
          if(nBufferCount<nLineCount)
              nLineCount=nBufferCount;
          n+=Debug_bin_HEX(szLine+n,pBuffer+nAddr,nLineCount);//16进制 每行16个字节 等于32个字符
          n+=Debug_bin_ASC(szLine+n,pBuffer+nAddr,nLineCount);//asc码  每行16个字节 等于16个字符
          printf("%s\n",szLine);//每行输出16个字节
          nAddr+=16;
          nBufferCount-=16;//帮助循环
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
//获取差值的函数
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
//计算差值的类
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
void CCountSub::Push(unsigned long ulNew)//用于循环
{
    E2B();
    m_CountSub.m_nEnd=ulNew;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
//平均值计算（每秒发生多少次动作）
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
unsigned long CDeltaTime::GetDeltaT(void)//获得差值
{
    return m_CountSub.GetX();
}
double CDeltaTime::GetOperationsPerSecond(unsigned long ulOperationCount/*期间的操作总和*/)
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
//统计平均值
//SCountSetBegin开始统计
//SCountSubSetEnd多次调用来刷新结束时间
//SCountSum获得统计平均值
void SCountReset(SCount &Count)
{
    Count.m_Sum=0;
    Count.m_Sub.m_nBegin=0;
    Count.m_Sub.m_nEnd=0;
}
unsigned long SCountSum(SCount &Count)//平均值的平均值
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
//计算平均值的类

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
//辅助函数
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

//获得给定范围内的随机数
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
////判断时间是否到了
//#define TimeSetNow(t) time(&t)
//inline bool TimelsUp(time_t tLast/*最后一次时间*/,long lMax/*超时时间设置*/)
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
//CLowDebug类，用于具体功能的输出

    //删除一个文件
void CNEOLowDebug::DeleteAFile(const char *szFileName)
{
    remove(szFileName);
}
//过滤路径获取文件名
char *CNEOLowDebug::GetTrueFileName(char *szBuffer)
{
    char *pRet=szBuffer;
    int nLen=strlen(szBuffer);
    int i=0;
    for(i=nLen-1;i>=0;i--)
    {
        //从右数第一个分隔符跳出作为文件名
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
    return pRet;//返回指针（不妥）
}

//输出字符串DebugToFile到文件或控制台，返回字符数，不包括\0
int CNEOLowDebug::DebugToFile(const char *szFormat,...)
{
    char szBuff[DEBUG_BUFFER_LENGTH] = "\0";                                //输出缓冲
    char szTemp[DEBUG_BUFFER_LENGTH] = "\0";                               //时间戳置换的中间buffer
    char szTime[DEBUG_BUFFER_LENGTH] = "\0";                              //时间戳buff
    FILE *fp=NULL;
    int nListCount=0;
    va_list pArgList;
    //time_t t;
    //struct tm *pTM=NULL;
    //int nLength=0;
    //构建时间戳
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
        //输出到文件
        fp=fopen(m_szFileName,"a+");
        if(fp)
        {
            nListCount=fprintf(fp,"%s%s",szTime,szBuff);
            if(m_bePrintToScreenFlag)
            {
                //输出到屏幕
            NEO_PRINTF("%s%s",szTime,szBuff);
            if(pInfoOutCallback)
            {
                //回调函数供上层调用
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
//输出一块内存段到文件。
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

//构造函数与析构函数
CNEOLowDebug::CNEOLowDebug (const char *szPathName,                //路径名
    const char *szAppName,                           //文件名
    _APP_INFO_OUT_CALLBACK pInfoOutCallback,//额外的输出回调函数
    void *pInfoOutCallbackParam,          //回调函数参数
    bool bePrintToScreenFlag          //是否打印到屏幕
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
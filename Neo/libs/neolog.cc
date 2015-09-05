//需要依赖debug类
//内存类
//锁类
//队列

#include "neoindex.h"    

#include "neomemmanager.h"                          
#include "neoqueue.h"      
#include "neolock.h"    
#include "neodebug.h"
#include "neolog.h"

namespace NEOLIB {
////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
CNEOLog::CNEOLog(CNEOLowDebug *pDebug,
        CNEOMemPoolWithLock *pMemPool,        //内存指针池
        const char *szLogPath,                      //日志路径
        const char *szAppName,                      //应用名
        int nHoldFileMax,                 //保存的最大文件数
        bool bSyslogFlag,                //日志级别开关
        bool bDebugFlag,
        bool bDebug2Flag,
        bool bDebug3Flag,
        bool bPrintfToScrFlag          //是否向屏幕打印
        ):m_Lock(),m_pDebug(pDebug),m_pMemPool(pMemPool)
{
    //m_pDebug=pDebug;
    //m_pMemPool=pMemPool;
    //拦截信息
    m_pInfoOutCallback=m_pDebug->pInfoOutCallback;
    m_pInfoOutCallbackParam=m_pDebug->pInfoOutCallbackParam;
    memset(m_szFilePath,0,sizeof(m_szFilePath));
    memset(m_szFileName,0,sizeof(m_szFileName));
    //利用debug输出启动标志
    m_pDebug->DebugToFile("CNEOLog:start~\n");
    //获得文件的基准字符
    FULL_NAME(szLogPath,szAppName,m_szFilePath,"");//在debug文件中定义
    //获得文件目录名的保存文件名
    FULL_NAME(szLogPath,szAppName,m_szFileInfoName,"info");
    //清空当前文件名缓冲
    NEO_CLEAN_CHAR_BUFFER(m_szFileName);//在某个文件中定义
    m_nFileSize=0;
    m_bSysLogFlag=bSyslogFlag;
    m_bDebugFlag=bDebugFlag;
    m_bDebug2Flag=bDebug2Flag;
    m_bDebug3Flag=bDebug3Flag;
    m_bPrintfToScrFlag=bPrintfToScrFlag;
    m_nHoldFileMax=nHoldFileMax;
    m_pFileInfoQueue=new CNEOMemQueue(m_pDebug,m_pMemPool,"CNEOLog::m_pFileInfoQueue");
    if(m_pFileInfoQueue)//申请成功，注册到内存池
    {
       m_pMemPool->Register(m_pFileInfoQueue,"CNEOLog::m_pFileInfoQueue");
    }
    m_pFileInfoQueue->ReadFromFile(m_szFileInfoName);//读入上次保留的文件名信息
    MakeFileName();                                  // 定制文件名
}
CNEOLog::~CNEOLog()
{
    if(m_pFileInfoQueue)
    {
       m_pFileInfoQueue->WriteToFile(m_szFileInfoName);
       m_pMemPool->UnRegister(m_pFileInfoQueue);    //反注册指针
       delete m_pFileInfoQueue;
       m_pFileInfoQueue=NULL;
    }
    m_pDebug->DebugToFile("CNEOLog:stop~\n");
}
int CNEOLog::_Printf(const char *szFormat,...)
 {
    char szTime[LOG_ITEM_LENGTH_MAX] = {"\0"};
    char szTemp[LOG_ITEM_LENGTH_MAX] = {"\0"};
    char szBuf[LOG_ITEM_LENGTH_MAX] = {"\0"};
    int nMaxLength=LOG_ITEM_LENGTH_MAX;
    int nListCount=0;
    time_t t;
    struct tm *pTM=NULL;
    int nLength=0;
    time(&t);
    pTM=localtime(&t);
    nLength=SafePrintf(szTemp,LOG_ITEM_LENGTH_MAX,"%s",asctime(pTM));
    szTemp[nLength-1]='\0';
    SafePrintf(szTime,LOG_ITEM_LENGTH_MAX,"[%s]",szTemp);
    va_list pArgList;
    va_start(pArgList,szFormat);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,nMaxLength-nListCount,szFormat,pArgList);
    va_end(pArgList);
    if(nListCount>(nMaxLength-1))
        nListCount=nMaxLength-1;
    *(szBuf+nListCount)='\0';
    //得到当前的文件名
    GetFileName();
    //将信息输出到文件
    nListCount=OutputFileOrScreen(m_szFileName,"a+","%s%s",szTime,szBuf);
    if(m_bPrintfToScrFlag)
    {
       NEO_PRINTF("%s%s",szTime,szBuf);
    }
    if(m_pInfoOutCallback)
    {
       char szInfoOut[APP_INFO_OIT_STRING_MAX];
       SafePrintf(szInfoOut,APP_INFO_OIT_STRING_MAX,"%s%s",szTime,szBuf);
       m_pInfoOutCallback(szInfoOut,m_pInfoOutCallbackParam);
    }
    m_nFileSize+=nListCount;
    return nListCount;
}
void CNEOLog::DeleteFirstFile(void)
{
    char szFirstFile[FILENAME_STRING_LENGTH]="\0";  //文件名缓冲
    int nFirstFileNameLen=0;                   //文件名长度
    //从文件名数组中弹出第一个数组
    nFirstFileNameLen=m_pFileInfoQueue->GetAndDeleteFirst(szFirstFile,FILENAME_STRING_LENGTH);
    if(0>=nFirstFileNameLen)
        goto CNEOLog_DeleteFirstFile_End;
    CNEOLowDebug::DeleteAFile(szFirstFile);   //真实的删除文件
CNEOLog_DeleteFirstFile_End:
    return;
}
void CNEOLog::FixFileInfo(void)
{
    //int nAddLastRet=0;
    while(m_pFileInfoQueue->GetTokenCount()>=m_nHoldFileMax)
    {
       DeleteFirstFile();
    }
}
void CNEOLog::MakeFileName(void)
{
    char szTemp[LOG_ITEM_LENGTH_MAX] = "\0";
    MakeATimeString(szTemp,LOG_ITEM_LENGTH_MAX);  //获得时间戳字符串
    FixFileInfo();                                //维护文件总个数（默认72个）
    int nLen=SafePrintf(m_szFileName,(FILENAME_STRING_LENGTH*2),"%s_%s.log",m_szFilePath,szTemp);//生成日志文件名
    nLen++;
    //将新的文件名添加到队列
    int nAddLastRet=m_pFileInfoQueue->AddLast(m_szFileName,nLen);
    if(0>=nAddLastRet)
    {
       //为防止队列满了，删除对头的三个名字
        DeleteFirstFile();
        DeleteFirstFile();
        DeleteFirstFile();
        //重新添加
        nAddLastRet=m_pFileInfoQueue->AddLast(m_szFileName,nLen);
    }
    m_nFileSize=0;                               //新建文件，长度为0
    //新建文件，在文件头打印一些信息
    time(&m_tFileNameMake);
    {
       bool bPrintToScr=m_bPrintfToScrFlag;
       m_bPrintfToScrFlag=false;
       _Printf("NEO.base lib log file %s\n",m_szFileName);
       _Printf("____________________________________\n");
       m_bPrintfToScrFlag=bPrintToScr;//输出完毕，恢复标志
    }

}
void CNEOLog::GetFileName(void)
{
    time_t tNow;
    unsigned long ulDeltaT=0;  //变化量
    if('\0'==m_szFileName[0])  //如果第一次启动，文件名位空
    {
       MakeFileName();
       goto CNEOLog_GetFileName_End;
    }
    time(&tNow);
    ulDeltaT=(unsigned long)tNow-m_tFileNameMake;  //计算变化量
    if(LOG_FILE_CHANGE_NAME_PRE_SECONDS<=ulDeltaT)
    {
       MakeFileName();                            //时间超过一小时，返回
       goto CNEOLog_GetFileName_End;
    }
    if(LOG_FILE_SIZE_MAX<=m_nFileSize)
    {
       MakeFileName();                            
       goto CNEOLog_GetFileName_End;
    }
CNEOLog_GetFileName_End:
    return;
}
int CNEOLog::MakeATimeString(char *szBuffer,int nBufferSize)
{
    int i=0;
    time_t t;
    struct tm *pTM=NULL;
    int nLength=0;
    if(LOG_TIME_STRING_MAX>nBufferSize)
        goto CNEOLog_MakeATimeString_End;
    time(&t);
    pTM=localtime(&t);//生成时间自动带一个;;\0
    nLength=SafePrintf(szBuffer,LOG_ITEM_LENGTH_MAX,"%s",asctime(pTM));
    szBuffer[nLength-1]='\0';
    //过滤空格
    for(i=0;i<nLength;i++)
    {
       if(' '==szBuffer[i])
           szBuffer[i]='_';
       if(':'==szBuffer[i])
           szBuffer[i]='_';
    }
CNEOLog_MakeATimeString_End:
    return nLength;
}
void CNEOLog::_XGDebugForBin(const char *pBuffer,int nLength)
{
    m_Lock.Lock();
    {
       GetFileName();
       OutputFileOrScreen(m_szFileName,"a+",pBuffer,nLength);
       Debug_bin(pBuffer,nLength);
    }
    m_Lock.UnLock();
}
int CNEOLog::_XGSysLog(const char *szFormat,...)
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
    if(m_bSysLogFlag)
    {
       m_Lock.Lock();
       {
          _Printf("%s\n",szBuf);
       }
       m_Lock.UnLock();
    }
    return nListCount;
}
int CNEOLog::_XGDebug(const char *szFormat,...)
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
    if(m_bDebugFlag)
    {
       m_Lock.Lock();
       {
          _Printf("%s\n",szBuf);
       }
       m_Lock.UnLock();
    }
    return nListCount;
}
int CNEOLog::_XGDebug2(const char *szFormat,...)
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
    if(m_bDebug2Flag)
    {
       m_Lock.Lock();
       {
          _Printf("%s",szBuf);
       }
       m_Lock.UnLock();
    }
    return nListCount;
}
int CNEOLog::_XGDebug3(const char *szFormat,...)
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
    if(m_bDebug3Flag)
    {
       m_Lock.Lock();
       {
          _Printf("%s",szBuf);
       }
       m_Lock.UnLock();
    }
    return nListCount;
}
//#else
//#endif
}
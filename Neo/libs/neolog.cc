//��Ҫ����debug��
//�ڴ���
//����
//����

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
        CNEOMemPoolWithLock *pMemPool,        //�ڴ�ָ���
        const char *szLogPath,                      //��־·��
        const char *szAppName,                      //Ӧ����
        int nHoldFileMax,                 //���������ļ���
        bool bSyslogFlag,                //��־���𿪹�
        bool bDebugFlag,
        bool bDebug2Flag,
        bool bDebug3Flag,
        bool bPrintfToScrFlag          //�Ƿ�����Ļ��ӡ
        ):m_Lock(),m_pDebug(pDebug),m_pMemPool(pMemPool)
{
    //m_pDebug=pDebug;
    //m_pMemPool=pMemPool;
    //������Ϣ
    m_pInfoOutCallback=m_pDebug->pInfoOutCallback;
    m_pInfoOutCallbackParam=m_pDebug->pInfoOutCallbackParam;
    memset(m_szFilePath,0,sizeof(m_szFilePath));
    memset(m_szFileName,0,sizeof(m_szFileName));
    //����debug���������־
    m_pDebug->DebugToFile("CNEOLog:start~\n");
    //����ļ��Ļ�׼�ַ�
    FULL_NAME(szLogPath,szAppName,m_szFilePath,"");//��debug�ļ��ж���
    //����ļ�Ŀ¼���ı����ļ���
    FULL_NAME(szLogPath,szAppName,m_szFileInfoName,"info");
    //��յ�ǰ�ļ�������
    NEO_CLEAN_CHAR_BUFFER(m_szFileName);//��ĳ���ļ��ж���
    m_nFileSize=0;
    m_bSysLogFlag=bSyslogFlag;
    m_bDebugFlag=bDebugFlag;
    m_bDebug2Flag=bDebug2Flag;
    m_bDebug3Flag=bDebug3Flag;
    m_bPrintfToScrFlag=bPrintfToScrFlag;
    m_nHoldFileMax=nHoldFileMax;
    m_pFileInfoQueue=new CNEOMemQueue(m_pDebug,m_pMemPool,"CNEOLog::m_pFileInfoQueue");
    if(m_pFileInfoQueue)//����ɹ���ע�ᵽ�ڴ��
    {
       m_pMemPool->Register(m_pFileInfoQueue,"CNEOLog::m_pFileInfoQueue");
    }
    m_pFileInfoQueue->ReadFromFile(m_szFileInfoName);//�����ϴα������ļ�����Ϣ
    MakeFileName();                                  // �����ļ���
}
CNEOLog::~CNEOLog()
{
    if(m_pFileInfoQueue)
    {
       m_pFileInfoQueue->WriteToFile(m_szFileInfoName);
       m_pMemPool->UnRegister(m_pFileInfoQueue);    //��ע��ָ��
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
    //�õ���ǰ���ļ���
    GetFileName();
    //����Ϣ������ļ�
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
    char szFirstFile[FILENAME_STRING_LENGTH]="\0";  //�ļ�������
    int nFirstFileNameLen=0;                   //�ļ�������
    //���ļ��������е�����һ������
    nFirstFileNameLen=m_pFileInfoQueue->GetAndDeleteFirst(szFirstFile,FILENAME_STRING_LENGTH);
    if(0>=nFirstFileNameLen)
        goto CNEOLog_DeleteFirstFile_End;
    CNEOLowDebug::DeleteAFile(szFirstFile);   //��ʵ��ɾ���ļ�
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
    MakeATimeString(szTemp,LOG_ITEM_LENGTH_MAX);  //���ʱ����ַ���
    FixFileInfo();                                //ά���ļ��ܸ�����Ĭ��72����
    int nLen=SafePrintf(m_szFileName,(FILENAME_STRING_LENGTH*2),"%s_%s.log",m_szFilePath,szTemp);//������־�ļ���
    nLen++;
    //���µ��ļ�����ӵ�����
    int nAddLastRet=m_pFileInfoQueue->AddLast(m_szFileName,nLen);
    if(0>=nAddLastRet)
    {
       //Ϊ��ֹ�������ˣ�ɾ����ͷ����������
        DeleteFirstFile();
        DeleteFirstFile();
        DeleteFirstFile();
        //�������
        nAddLastRet=m_pFileInfoQueue->AddLast(m_szFileName,nLen);
    }
    m_nFileSize=0;                               //�½��ļ�������Ϊ0
    //�½��ļ������ļ�ͷ��ӡһЩ��Ϣ
    time(&m_tFileNameMake);
    {
       bool bPrintToScr=m_bPrintfToScrFlag;
       m_bPrintfToScrFlag=false;
       _Printf("NEO.base lib log file %s\n",m_szFileName);
       _Printf("____________________________________\n");
       m_bPrintfToScrFlag=bPrintToScr;//�����ϣ��ָ���־
    }

}
void CNEOLog::GetFileName(void)
{
    time_t tNow;
    unsigned long ulDeltaT=0;  //�仯��
    if('\0'==m_szFileName[0])  //�����һ���������ļ���λ��
    {
       MakeFileName();
       goto CNEOLog_GetFileName_End;
    }
    time(&tNow);
    ulDeltaT=(unsigned long)tNow-m_tFileNameMake;  //����仯��
    if(LOG_FILE_CHANGE_NAME_PRE_SECONDS<=ulDeltaT)
    {
       MakeFileName();                            //ʱ�䳬��һСʱ������
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
    pTM=localtime(&t);//����ʱ���Զ���һ��;;\0
    nLength=SafePrintf(szBuffer,LOG_ITEM_LENGTH_MAX,"%s",asctime(pTM));
    szBuffer[nLength-1]='\0';
    //���˿ո�
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
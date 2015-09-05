

#include "neoindex.h"
#include "neosafefunc.h"

void neosafestrncpy(char *pD,const char *pS,int nDestSize)
{
    int nLen=strlen(pS)+1;
    if(nLen>nDestSize)
        nLen=nDestSize;
    memcpy(pD,pS,nLen);
    *(pD+nLen-1)='\0';
}

int neosafesprintf(char *szBuf,int nDestSize,const char *szFormat,...)
{
    int nListCount=0;
    va_list pArgList;
    va_start(pArgList,szFormat);
    nListCount+=WIN_LINUX_vsnprintf(szBuf+nListCount,nDestSize+nListCount,szFormat,pArgList);
    va_end(pArgList);
    *(szBuf+nDestSize-1)='\0';
    return strlen(szBuf);
}
void SafeStrcpy(char *pDest,const char *pSource,int nCount)
{
    int nLen=(int)strlen(pSource)+1;
    if(!pDest)
        goto SafeStrcpy_End;
    if(!pSource)
        goto SafeStrcpy_End;
    if(nLen>nCount)
        nLen=nCount;
    memcpy(pDest,pSource,nLen);
    *(pDest+nLen-1)='\0';
SafeStrcpy_End:
    return;
}
//#else 

//#endif

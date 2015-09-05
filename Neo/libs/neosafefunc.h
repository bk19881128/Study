#ifndef NEOSAFEFUNC

#define NEOSAFEFUNC

///////////////////////////////////////////////export func
 void neosafestrncpy(char *pD,const char *pS,int nDestSize);
 int neosafesprintf(char *szBuf,int nDestSize,const char *szFormat,...);
 void SafeStrcpy(char *pDest,const char *pSource,int nCount);

#endif
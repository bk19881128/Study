#ifndef NEOMEM 

#define NEOMEM 
#include "neoindex.h"  

#include "neolock.h"      
#include "neodebug.h"        
#include "neosafefunc.h"
namespace NEOLIB {
///////////////////////////////////////////////export func
// void neosafestrncpy(char *pD,char *pS,int nDestSize);
// int neosafesprintf(char *szBuf,int nDestSize,char *szFormat,...);
//�������
//�������Ԫ��
typedef struct _CHAIN_TOKEN_
{
	struct _CHAIN_TOKEN_ *m_pNext;
	char *m_pBuffer;
}SChainToken;
//�б��ͷ
typedef struct _NEO_MEM_BLOCK_HEAD_
{
	unsigned long m_ulBlockSize;//�ڴ��Ĵ�С��byte��
	struct _NEO_MEM_BLOCK_HEAD_ *m_pNext;
}SNeoMemroryBlockHead;
const unsigned long SNeoMemroryBlockHeadSize=sizeof(SNeoMemroryBlockHead);
////////////////////////////////////////////////////////////////////////////
////�Զ������ģ��
//#define NEO_MEM_BLOCK_SIZE(nDataLen) (nDataLen+SNeoMemroryBlockHeadSize)
//#define NEO_MEM_BLOCK_DATA_SIZE(nBlockSize) (nBlockSize-SNeoMemroryBlockHeadSize)
////�����ͷŵ�ָ�룬������ʵ���ڴ��ڵ�ַ
//#define NEO_MEM_BLOCK_HEAD(pData) ((SNeoMemroryBlockHead*)(((char*)pData)-SNeoMemroryBlockHeadSize))
////�����ݿ�ĵ�ַ
//#define NEO_MEM_BLOCK_DATA(pHead) (((char *)pHead)+SNeoMemroryBlockHeadSize)
////��С����ڴ��
//#define NEO_MEM_STACK_BLOCK_MIN 16
//#define NEO_MEM_STACK_BLOCK_MAX (1024*1024)
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////�ڴ�ע��ģ��
//#ifndef NEO_MEM_BLOCK_INFO_MAX_SIZE
//#define NEO_MEM_BLOCK_INFO_MAX_SIZE 124
//#endif
typedef struct _NEO_MEM_REGISTER_
{
	void *m_pPoint;                            //����ָ��
	char m_szInfo[NEO_MEM_BLOCK_INFO_MAX_SIZE];//˵������
}SNEOMemRegister;
const unsigned long SNEOMemRegisterSize=sizeof(SNEOMemRegister);
////�����ָ�����
//#ifndef NEO_MEM_REGISTER_MAX
//#define NEO_MEM_REGISTER_MAX 10000
//#endif
////�ַ�����յĺ�
//#define NEO_CLEAN_CHAR_BUFFER(p) (*((char *)p)='\0')
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//socketע��ģ��
typedef struct _NEO_SOCKET_REGISTER_
{
	WIN_LINUX_SOCKET m_nSocket;
	char m_szInfo[NEO_MEM_BLOCK_INFO_MAX_SIZE];
}SNEOSocketRegister;
const unsigned long SNEOSocketRegisterSize=sizeof(SNEOSocketRegister);
//�ڴ��Ԫ�ع���,��Ҫ����֮���Ĺ���
class  CNEOMemStackToken
{
public:
	CNEOMemStackToken(int nBlockSize/*Ҫ����Ļ����ڴ���С*/,CNEOLowDebug *pDebug);
	~CNEOMemStackToken();
private:
	SNeoMemroryBlockHead *m_pFirstSon;//����ͷ
	CNEOMemStackToken *m_pNext;//�ֵܽڵ�
	CMultiReadSingleWriteLock m_Lock;//����������������
	//�ڴ��ߴ�
	unsigned long m_ulBlockSize;//ֻ����������������
	CMRSWint m_nBlockOutSide;//�����ȥ���ڴ���
	CMRSWint m_nBlockInSide;//�����Ŀ����ڴ���
	CNEOLowDebug *m_pDebug;
public:
	void *Malloc(int nSize,/*ͳ������Ա�debug����*/CMRSWint &nAllBlockCount,CMRSWint &nMemUsed);
	bool Free(void *pPoint,bool bCloseFlag);
    void PrintStack(void);
private:
	//ϵͳ�Ƴ�ʱ�ݹ�����������ڴ��,�ݹ�������֮��
	void DestroySon(SNeoMemroryBlockHead *pSon);
};
//�ۺϣ�ʵ����֦���Ĺ���
class  CNEOMemStack
{
public:
	CNEOMemStack(CNEOLowDebug *pDebug);
	~CNEOMemStack();
private:
	CNEOMemStackToken *m_pHead;//��֦������һ���ڵ�
	//CMRSWint m_pMaxPoint;//ͳ�Ƶ�ǰ�õ�������ڴ�ָ��
	CMutexTemplate<void *> m_pMaxPoint;
	CMRSWint m_nAllBlockCount;//ͳ�Ƶ�ǰ�õ��������ڴ��
	CMRSWint m_nMemoryUse;//ͳ�Ƶ�ǰʹ�õ��ڴ����ֽ���
	CMRSWbool m_CloseFlag;//�رձ�ǩ
public:
	//�ض���ָ��ָ����ڴ浥Ԫ�Ŀռ��С��������ָ����¿ռ�ĳߴ�
    //����ɹ���������Ч����ָ�룬ָ���ض���Ĵ�С֮���¿ռ䣬ʧ�ܷ���null
	//���Խ��Ϳռ�����ݱ��ݵ��¿ռ䣬���ⲻһ��ȡ�����¿ռ�Ĵ�С
	void *ReMalloc(void *pPoint,//��ָ��
		int nNewSize,//�µ����ݳߴ�
		bool bCopyOldDataFlag=true);
	//Malloc��free�������ڴ����Ԫ�ķ���
	void *Malloc(int nSize);
	bool Free(void *pPoint);
	//�ڲ���Ϣ�������
	void PrintStack(void);
	void PrintInfo(void);
	//���ùرձ�־��Ϊ��ʱ����ϵͳfree
	void SetCloseFlag(bool bFlag=true);
	CNEOLowDebug *m_pDebug;
};
//�ڴ�ָ��ע��ģ��
class  CMemRegister
{
public:
	CMemRegister(CNEOLowDebug *pDebug);
	~CMemRegister();
private:
	CNEOLowDebug *m_pDebug;
	CMutexLock m_Lock;//�̰߳�ȫ��
	SNEOMemRegister m_RegisterArray[NEO_MEM_REGISTER_MAX];
	//��ǰ�������ʹ�ö��ٵ�Ԫ
	int m_nUseMax;
	//ͳ������ָ��
	void *m_pMaxPoint;
	//ͳ�Ƶ�ǰ����ʹ�õ�ָ����
	int m_nPointCount;
private:
	//�ڲ����ߣ���������
	void RegisterCopy(SNEOMemRegister *pDest//Ŀ�Ŀ���ָ��
		,void *pPoint                       //��������ָ��
		,const char *szInfo);                      //������������
public:
	//���һ��ָ�뼰��˵��
	void Add(void *pPoint,const char *szInfo);
	//ɾ��һ��ָ�루�ڴ汻�ͷţ�ʧЧ��
	void Del(void *pPoint);
	//remalloc��ʱ�����ָ��
	void Modeify(void *pOld,void *pNew);
	//��ӡ��Ϣ
	void PrintInfo(void);
};
////socketע��ģ��
//bool SocketIsOK(WIN_LINUX_SOCKET nSocket)
//{
//	//if(0>nSocket) return false;
//	//if(nSocket>65535)return false
//	if(WIN_LINUX_InvalidSocket==nSocket)
//		return false;
//	return true;
//}
class  CSocketRegister
{
public:
	CSocketRegister(CNEOLowDebug *pDebug);
	~CSocketRegister();
private:
	CNEOLowDebug *m_pDebug;
	CMutexLock m_Lock;
	SNEOSocketRegister m_RegisterArray[NEO_MEM_REGISTER_MAX];
	//�������m_nSocketUseCount
	int m_nUseMax;
	WIN_LINUX_SOCKET m_nMaxSocket;//ע���������socket
	int m_nSocketUseCount;//���õ�socket����
public:
	void Add(WIN_LINUX_SOCKET s,char *szInfo=NULL);
	bool Del(WIN_LINUX_SOCKET s);
	void PrintInfo(void);
};
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//�ڴ����
class  CNEOMemPoolWithLock
{
public:
	CNEOMemPoolWithLock(CNEOLowDebug *pDebug,
		bool bOpenResgisterFlag=true);
	~CNEOMemPoolWithLock();
	//ָ�����
private:
	CMemRegister *m_pRegister;
public:
	void Register(void *pPoint,const char *szInfo);
	void UnRegister(void *pPoint);
	//socket   ����
private:
	CSocketRegister *m_pSocketRegister;
public:
	void RegisterSocket(WIN_LINUX_SOCKET s,char *szInfo=NULL);
	//��ע��ӹر�
	void CloseSocket(WIN_LINUX_SOCKET &s);
	//��������
public:
	//���ñ�־�������е�free��ֱ��free, ���ڱ�����stack��
	void SetCloseFlag(bool bFlag=true);
	//���·���ָ��Ŀռ䣬Ĭ�Ͽ���ԭʼ���ݵ��¿ռ�
	void *ReMalloc(void *pPoint,int nNewSize,bool bCopyOldDataFlag=true);
	//����һ���ڴ�
	void *Malloc(int nSize,const char *szInfo=NULL);
	void Free(void *pBlock);
	//��ʾ�����ڴ���������
	void PrintTree(void);
	//�ؼ���Ϣ��ʾ
	void PrintInfo(void);
public:
	CNEOMemStack *m_pMemPool;//�ڴ�ջ����
	CNEOLowDebug *m_pDebug;
};
}
#endif
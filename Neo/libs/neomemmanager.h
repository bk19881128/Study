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
//类的申明
//基本类表元素
typedef struct _CHAIN_TOKEN_
{
	struct _CHAIN_TOKEN_ *m_pNext;
	char *m_pBuffer;
}SChainToken;
//列表的头
typedef struct _NEO_MEM_BLOCK_HEAD_
{
	unsigned long m_ulBlockSize;//内存块的大小（byte）
	struct _NEO_MEM_BLOCK_HEAD_ *m_pNext;
}SNeoMemroryBlockHead;
const unsigned long SNeoMemroryBlockHeadSize=sizeof(SNeoMemroryBlockHead);
////////////////////////////////////////////////////////////////////////////
////自定义计算模块
//#define NEO_MEM_BLOCK_SIZE(nDataLen) (nDataLen+SNeoMemroryBlockHeadSize)
//#define NEO_MEM_BLOCK_DATA_SIZE(nBlockSize) (nBlockSize-SNeoMemroryBlockHeadSize)
////根据释放的指针，逆求真实的内存内地址
//#define NEO_MEM_BLOCK_HEAD(pData) ((SNeoMemroryBlockHead*)(((char*)pData)-SNeoMemroryBlockHeadSize))
////求数据块的地址
//#define NEO_MEM_BLOCK_DATA(pHead) (((char *)pHead)+SNeoMemroryBlockHeadSize)
////最小最大内存块
//#define NEO_MEM_STACK_BLOCK_MIN 16
//#define NEO_MEM_STACK_BLOCK_MAX (1024*1024)
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////内存注册模块
//#ifndef NEO_MEM_BLOCK_INFO_MAX_SIZE
//#define NEO_MEM_BLOCK_INFO_MAX_SIZE 124
//#endif
typedef struct _NEO_MEM_REGISTER_
{
	void *m_pPoint;                            //保留指针
	char m_szInfo[NEO_MEM_BLOCK_INFO_MAX_SIZE];//说明文字
}SNEOMemRegister;
const unsigned long SNEOMemRegisterSize=sizeof(SNEOMemRegister);
////管理的指针个数
//#ifndef NEO_MEM_REGISTER_MAX
//#define NEO_MEM_REGISTER_MAX 10000
//#endif
////字符串清空的宏
//#define NEO_CLEAN_CHAR_BUFFER(p) (*((char *)p)='\0')
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//socket注册模块
typedef struct _NEO_SOCKET_REGISTER_
{
	WIN_LINUX_SOCKET m_nSocket;
	char m_szInfo[NEO_MEM_BLOCK_INFO_MAX_SIZE];
}SNEOSocketRegister;
const unsigned long SNEOSocketRegisterSize=sizeof(SNEOSocketRegister);
//内存块元素管理,主要是右之树的管理
class  CNEOMemStackToken
{
public:
	CNEOMemStackToken(int nBlockSize/*要管理的基本内存块大小*/,CNEOLowDebug *pDebug);
	~CNEOMemStackToken();
private:
	SNeoMemroryBlockHead *m_pFirstSon;//链表头
	CNEOMemStackToken *m_pNext;//兄弟节点
	CMultiReadSingleWriteLock m_Lock;//保护上面两个变量
	//内存块尺寸
	unsigned long m_ulBlockSize;//只读的所以无锁保护
	CMRSWint m_nBlockOutSide;//分配出去的内存数
	CMRSWint m_nBlockInSide;//保留的空闲内存数
	CNEOLowDebug *m_pDebug;
public:
	void *Malloc(int nSize,/*统计设计以便debug调用*/CMRSWint &nAllBlockCount,CMRSWint &nMemUsed);
	bool Free(void *pPoint,bool bCloseFlag);
    void PrintStack(void);
private:
	//系统推出时递归销毁申请的内存块,递归销毁右之树
	void DestroySon(SNeoMemroryBlockHead *pSon);
};
//聚合，实现左枝树的管理
class  CNEOMemStack
{
public:
	CNEOMemStack(CNEOLowDebug *pDebug);
	~CNEOMemStack();
private:
	CNEOMemStackToken *m_pHead;//左枝树的以一个节点
	//CMRSWint m_pMaxPoint;//统计当前用到的最大内存指针
	CMutexTemplate<void *> m_pMaxPoint;
	CMRSWint m_nAllBlockCount;//统计当前用到的所有内存块
	CMRSWint m_nMemoryUse;//统计当前使用的内存总字节数
	CMRSWbool m_CloseFlag;//关闭标签
public:
	//重定义指针指向的内存单元的空间大小，给出旧指针和新空间的尺寸
    //如果成功，返回有效地新指针，指向重定义的大小之后新空间，失败返回null
	//可以将就空间的数据备份到新空间，但这不一定取决于新空间的大小
	void *ReMalloc(void *pPoint,//旧指针
		int nNewSize,//新的数据尺寸
		bool bCopyOldDataFlag=true);
	//Malloc和free，调用内存管理单元的方法
	void *Malloc(int nSize);
	bool Free(void *pPoint);
	//内部信息输出函数
	void PrintStack(void);
	void PrintInfo(void);
	//设置关闭标志，为真时调用系统free
	void SetCloseFlag(bool bFlag=true);
	CNEOLowDebug *m_pDebug;
};
//内存指针注册模块
class  CMemRegister
{
public:
	CMemRegister(CNEOLowDebug *pDebug);
	~CMemRegister();
private:
	CNEOLowDebug *m_pDebug;
	CMutexLock m_Lock;//线程安全锁
	SNEOMemRegister m_RegisterArray[NEO_MEM_REGISTER_MAX];
	//当前数组最大使用多少单元
	int m_nUseMax;
	//统计最大的指针
	void *m_pMaxPoint;
	//统计当前正在使用的指针数
	int m_nPointCount;
private:
	//内部工具，拷贝数据
	void RegisterCopy(SNEOMemRegister *pDest//目的拷贝指针
		,void *pPoint                       //待拷贝的指针
		,const char *szInfo);                      //待拷贝的描述
public:
	//添加一个指针及其说明
	void Add(void *pPoint,const char *szInfo);
	//删除一个指针（内存被释放，失效）
	void Del(void *pPoint);
	//remalloc的时候更新指针
	void Modeify(void *pOld,void *pNew);
	//打印信息
	void PrintInfo(void);
};
////socket注册模块
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
	//管理变量m_nSocketUseCount
	int m_nUseMax;
	WIN_LINUX_SOCKET m_nMaxSocket;//注册过的最大的socket
	int m_nSocketUseCount;//在用的socket总数
public:
	void Add(WIN_LINUX_SOCKET s,char *szInfo=NULL);
	bool Del(WIN_LINUX_SOCKET s);
	void PrintInfo(void);
};
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//内存池类
class  CNEOMemPoolWithLock
{
public:
	CNEOMemPoolWithLock(CNEOLowDebug *pDebug,
		bool bOpenResgisterFlag=true);
	~CNEOMemPoolWithLock();
	//指针管理
private:
	CMemRegister *m_pRegister;
public:
	void Register(void *pPoint,const char *szInfo);
	void UnRegister(void *pPoint);
	//socket   管理
private:
	CSocketRegister *m_pSocketRegister;
public:
	void RegisterSocket(WIN_LINUX_SOCKET s,char *szInfo=NULL);
	//反注册加关闭
	void CloseSocket(WIN_LINUX_SOCKET &s);
	//公共管理
public:
	//设置标志，让所有的free都直接free, 不在保留在stack中
	void SetCloseFlag(bool bFlag=true);
	//重新分配指针的空间，默认拷贝原始数据到新空间
	void *ReMalloc(void *pPoint,int nNewSize,bool bCopyOldDataFlag=true);
	//分配一块内存
	void *Malloc(int nSize,const char *szInfo=NULL);
	void Free(void *pBlock);
	//显示整棵内存树的内容
	void PrintTree(void);
	//关键信息显示
	void PrintInfo(void);
public:
	CNEOMemStack *m_pMemPool;//内存栈对象
	CNEOLowDebug *m_pDebug;
};
}
#endif
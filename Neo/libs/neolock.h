#ifndef NEOLOCK

#define NEOLOCK
#include "neoindex.h"

namespace NEOLIB {

template<class mType>
struct MVALUE
{
	mType m_nValue;
	MUTEX m_MyLock;
};

template<class cmType>
class  CMutexTemplate
{
public:
	CMutexTemplate(cmType nValue);
	~CMutexTemplate(void);
public:
	cmType Get(void);
	cmType Set(cmType nValue);
	cmType Add(int nValue=1);
	cmType Dec(int nValue=1);
public:
	MVALUE<cmType> m_cmValue;
};

template<class cmType>
CMutexTemplate<cmType>::CMutexTemplate(cmType nValue)
{

	MUTEXINIT(&m_cmValue.m_MyLock);
    m_cmValue.m_nValue=nValue;
}

template<class cmType>
CMutexTemplate<cmType>::~CMutexTemplate(void)
{
	MUTEXLOCK(&m_cmValue.m_MyLock);
    MUTEXUNLOCK(&m_cmValue.m_MyLock);
    /////////////////////////////////
    MUTEXDESTROY(&m_cmValue.m_MyLock);
}

template <class cmType>
cmType CMutexTemplate<cmType>::Get(void)
{
	cmType nValue;
    MUTEXLOCK(&m_cmValue.m_MyLock);
    nValue=m_cmValue.m_nValue;
    MUTEXUNLOCK(&m_cmValue.m_MyLock);
    return nValue;
}

template <class cmType>
cmType CMutexTemplate<cmType>::Set(cmType nValue)
{
	MUTEXLOCK(&m_cmValue.m_MyLock);
    m_cmValue.m_nValue=nValue;
    MUTEXUNLOCK(&m_cmValue.m_MyLock);
    return nValue;
}

template <class cmType>
cmType CMutexTemplate<cmType>::Add(int nValue)
{
	cmType nRet;
    MUTEXLOCK(&m_cmValue.m_MyLock);
    m_cmValue.m_nValue+=nValue;
    nRet=m_cmValue.m_nValue;
    MUTEXUNLOCK(&m_cmValue.m_MyLock);
    return nRet;
}

template <class cmType>
cmType CMutexTemplate<cmType>::Dec(int nValue)
{
	int nRet;
    MUTEXLOCK(&m_cmValue.m_MyLock);
    m_cmValue.m_nValue-=nValue;
    nRet=m_cmValue.m_nValue;
    MUTEXUNLOCK(&m_cmValue.m_MyLock);
    return nRet;
}


//c语言的加锁结构体
typedef struct _MINT_
{
	int m_nValue;
	MUTEX m_MyLock;
}MINT,MBOOL;
//对整形和bool型数据的加锁
//初始化哪一个线程安全变量，同时可以返回设置值
 int MvarInit(MINT &(mValue),int nValue=0);
 void MvarDestroy(MINT &(mValue));
//设置一个线程变量值
 int MvarSet(MINT &(mValue),int nValue);
//得到线程安全变量的值
 int MvarGet(MINT &(mValue));
//线程安全变量做加法和解法
 int MvarAdd(MINT &(mValue),int nValue=1);
 int MvarDec(MINT &(mValue),int nValue=1);

//单写多读锁
typedef struct _NEO_MULTI_READ_SINGLE_WRITE_LOCK_
{
	int m_nReadCount;                              //读计数器
	bool m_bWriteFlag;                             //写标记
	MUTEX m_Lock;                                  //内部锁，保证多线程安全
}SNeoMultiReadSingleWriteLock;
const unsigned long  SNeoMultiReadSingleWriteLockSize=sizeof(SNeoMultiReadSingleWriteLock);

 void MRSWLock_Create(SNeoMultiReadSingleWriteLock *pLock);
 void MRSWLock_Destroy(SNeoMultiReadSingleWriteLock *pLock);
//锁状态获知
 bool MRSWLock_GetWrite(SNeoMultiReadSingleWriteLock *pLock);
 int MRSWLock_GetRead(SNeoMultiReadSingleWriteLock *pLock);
//进入写操作
 void MRSWLock_EnterWrite(SNeoMultiReadSingleWriteLock *pLock);
//退出写
 void MRSWLock_DisableWrite(SNeoMultiReadSingleWriteLock *pLock);
//进入读
 int MRSWLock_AddRead(SNeoMultiReadSingleWriteLock *pLock);
//退出读
 int MRSWLock_DecRead(SNeoMultiReadSingleWriteLock *pLock);
//读转写
 void MRSWLock_ReadToWrite(SNeoMultiReadSingleWriteLock *pLock);
/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//基本加锁类
class  CMutexLock
{
public:
	CMutexLock(void);
	~CMutexLock(void);

public:
	void Lock();
	void UnLock();
private:
	MUTEX m_Lock;
};
//以上函数的c++类实现
class  CMint
{
public:
	CMint(int nValue=0);
	~CMint(void);
public:
	int Get(void);
	int Set(int nValue);
	int Add(int nValue=1);
	int Dec(int nValue=1);
public:
	MINT m_nValue;
};
//对bool型变量的加锁
class  CMbool
{
public:
	CMbool(int nValue=0);
	~CMbool(void);
public:
	int Get(void);
	int Set(int nValue);
public:
	MBOOL m_nValue;
};
//以上函数对应的c++封装
class  CMultiReadSingleWriteLock
{
public:
	CMultiReadSingleWriteLock();
	~CMultiReadSingleWriteLock();
public:
	void EnableWrite(void);
	void DisableWrite(void);
	void ReadToWrite(void);
	void DecRead(void);
	void AddRead(void);
	bool GetWrite(void);
	int GetRead();
private:
	SNeoMultiReadSingleWriteLock m_Lock;
};
//用以上的锁保护整形和bool型变量的类
class  CMRSWint
{
public:
	CMRSWint();
	~CMRSWint();
public:
	int Get(void);
	int Set(int nValue);
	int Add(int nValue=1);
	int Dec(int nValue=1);
private:
	int m_nValue;
	CMultiReadSingleWriteLock m_Lock;
};
class  CMRSWbool
{
public:
	CMRSWbool();
	~CMRSWbool();
public:
	int Get(void);
	int Set(bool bFlag);
private:
	CMRSWint m_nValue;
};
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//不可重入锁
class  CNonReentrant
{
public:
	CNonReentrant();
public:
	bool Set(bool bRunFlag);
private:
	CMutexLock m_Lock;             //flag
	bool m_bAlreadyRunFlag;        //内部的变量值
};
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//线程控制锁
class  CThreadManager
{
private:
	CMRSWbool m_bThreadContinue;//线程持续的标志
	CMRSWint m_nThreadCount;    //线程计数器
	CMRSWint m_nThreadID;       //内嵌的debug友好功能
public:
	CThreadManager();
	~CThreadManager();
	void Open(void);
	void CloseAll(void);
	int AddThread(void);
	void DecAThread(void);
	//查询线程维持变量的值
	bool ThreadContinue(void);
	//获得计数器的值
	int GetThreadCount(void);
	//分配一个线程ID供debug用
	int GetID(void);
};

}

#endif
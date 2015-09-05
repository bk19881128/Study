#ifndef NEODEBUG  

#define NEODEBUG  
#include "neoindex.h"
#include "neolock.h"

namespace NEOLIB {
///////////////////////////////////////////////export fun
typedef struct COUNT_SUB//差值的数据结构
{
    unsigned long m_nBegin;
    unsigned long m_nEnd;
}SCountSub;
typedef struct _COUNT_//统计平均值模块
{
    SCountSub m_Sub;//差值
    unsigned long m_Sum;//统计平均值
}SCount;
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//字符输出
 int SafePrintf(char *szBuf,int nMaxlength,const char *szFormat,...);
//获得时间戳
 int GetTimeStamp(char *szBuf,int nMaxlength);
//打印到文件或屏幕
 int OutputFileOrScreen(const char *szFileName,const char *szMode,const char *szFormate,...);
//以二进制输出数据
 int Debug_bin_ASC(char *pPrintBuffer,const char *pBuffer,int nLength);
//以16进制输出数据
 int Debug_bin_HEX(char *pPrintBuffer,const char *pBuffer,int nLength);
//输出主入口函数
 void Debug_bin(const char *pBuffer,int nLength);
///////////////////////////////////////////////////////////////
//获取差值的函数
 unsigned long SCountSubGetX(SCountSub &CountSub);
 void SCountSubSetBegin(SCountSub &CountSub,unsigned long n);
 unsigned long SCountSubSetEnd(SCountSub &CountSub,unsigned long n);
///////////////////////////////////////////////////////////////
//统计平均值模块
 void SCountReset(SCount &Count);//初始化
 unsigned long SCountSum(SCount &Count);//计算统计平均值
 unsigned long SCountGetSum(SCount &Count);//返回sum值
 unsigned long SCountGetX(SCount &Count);//返回当前的差值
 void SCountSetBegin(SCount &Count,unsigned long n);
 unsigned long SCountSetEnd(SCount &Count,unsigned long n);
////////////////////////////////////////////////////////////////
//辅助功能函数
//获得非零随机数
 inline int GetNot0(void);
 inline int Get0(void);
//获得给定范围内的随机数
 inline int GetRandomBeteen(int nBegin,int nEnd);
//判断时间是否到了
 //inline bool TimelsUp(long tLast,long lMax);
//计算差值的类
class  CCountSub
{
public:
    CCountSub();
    ~CCountSub();
public:
    SCountSub m_CountSub;
    unsigned long SetBegin(unsigned long n);
    unsigned long SetEnd(unsigned long n);
    unsigned long GetBegin(void);
    unsigned long GetEnd(void);
    unsigned long GetX(void);
    void E2B(void);
    void Push(unsigned long ulNew);
};

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
//平均值计算（每秒发生多少次动作）
class  CDeltaTime
{
public:
    CDeltaTime();
    ~CDeltaTime();
public:
    CCountSub m_CountSub;
    void Reset(void);
    void TouchBegin(void);
    void TouchEnd(void);
    unsigned long GetDeltaT(void);
    double GetOperationsPerSecond(unsigned long ulOperationCount/*期间的操作总和*/);
};
//////////////////////////////////////
//计算平均值的类
class  CCount
{
public:
    CCount();
    ~CCount();
public:
    SCount m_Count;
    void SCountReset(void);
    unsigned long SCountSum(void);
    unsigned long SCountSetSum(unsigned long n);
    unsigned long SCountGetSum(void);
    unsigned long SCountGetX(void);
    void SCountSetBegin(unsigned long n);
    unsigned long SCountSetEnd(unsigned long n);
};
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//CLowDebug类，用于具体功能的输出
class  CNEOLowDebug
{
public:
    //删除一个文件
    static void DeleteAFile(const char *szFileName);
    //过滤路径获取文件名
    static char *GetTrueFileName(char *szBuffer);
public:
    //输出字符串DebugToFile到文件或控制台，返回字符数，不包括\0
    int DebugToFile(const char *szFormat,...);
    //输出一块内存段到文件。
    void DebugToFileBin(char *pBuffer,int nLength);
public:
    //构造函数与析构函数
    CNEOLowDebug (const char *szPathName,                //路径名
        const char *szAppName,                           //文件名
        _APP_INFO_OUT_CALLBACK pInfoOutCallback=NULL,//额外的输出回调函数
        void *pInfoOutCallbackParam=NULL,          //回调函数参数
        bool bePrintToScreenFlag=false           //是否打印到屏幕
        );
    ~CNEOLowDebug();
public:
    //回调函数定义
    //在需要的时候，抓取输出信息，打入自己的输出队列
    _APP_INFO_OUT_CALLBACK pInfoOutCallback;
    //给调用者提供的指针
    void *pInfoOutCallbackParam;
private:
    bool m_bePrintToScreenFlag;                  //内部保留的输出标记
    char m_szFileName[NEO_DEBUG_FILENAME_LENGTH];//拼接好的路径名+文件名
    CMutexLock m_Lock;                           //线程安全锁
};

}
#endif
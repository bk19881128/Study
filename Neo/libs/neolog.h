#ifndef NEOLOG

#define NEOLOG
#include "neoindex.h"

namespace NEOLIB {

class CNEOMemPoolWithLock;
class CNEOLowDebug;
class CMutexLock;
class CNEOMemQueue;

////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//#define LOG_FILE_SIZE_MAX (1*1024*1024*1024)   //日志文件最大1g
//#define LOG_ITEM_LENGTH_MAX (2*1024)           //单条log最大2K
//#define LOG_FILE_CHANGE_NAME_PRE_SECONDS (60*60)//日志文件一小时更换一次姓名
//#define LOG_FILE_INFO_BUFFER_SIZE (256*1024)   //日志目录最大长度 超出删除
//#define LOG_FILE_DEFAULT_HOLD 72               //一般保留三天的数据
//#define LOG_TIME_STRING_MAX 128                //时间戳字符串长度
//#define FILENAME_STRING_LENGTH 256             //文件名长度
//#define APP_INFO_OIT_STRING_MAX 256           
//////////////////////////////////////////////////////////////
//日志类
class  CNEOLog
{
public:
	CNEOLog(CNEOLowDebug *pDebug,
		CNEOMemPoolWithLock *pMemPool,        //内存指针池
		const char *szLogPath,                      //日志路径
		const char *szAppName,                      //应用名
		int nHoldFileMax=LOG_FILE_DEFAULT_HOLD,//保存的最大文件数
		bool bSyslogFlag=true,                //日志级别开关
		bool bDebugFlag=true,
		bool bDebug2Flag=false,
		bool bDebug3Flag=false,
		bool bPrintfToScrFlag=true           //是否向屏幕打印
		);
	~CNEOLog();
private://内部私有变量
	CMutexLock m_Lock;                      //线程安全锁
	char m_szFilePath[FILENAME_STRING_LENGTH];//文件路径
	char m_szFileName[FILENAME_STRING_LENGTH*2];//文件名
	unsigned long m_nFileSize;                //当前文件大小
	time_t m_tFileNameMake;                   //定制文件名的时间戳
	int m_nHoldFileMax;                       //保留文件的数量，由构造函数传入
	_APP_INFO_OUT_CALLBACK m_pInfoOutCallback;// 应用程序拦截输出回调函数
	void *m_pInfoOutCallbackParam;            //透传的回调函数参数
	bool m_bPrintfToScrFlag;                  //是否打印到屏幕
	char m_szFileInfoName[FILENAME_STRING_LENGTH];//文件名
	CNEOLowDebug *m_pDebug;
	CNEOMemPoolWithLock *m_pMemPool;          //内存池指针
	CNEOMemQueue *m_pFileInfoQueue;           //文件明队列
private:
	//内部工具函数
	int _Printf(const char *szFormat,...);        //核心打印输出模块
	void DeleteFirstFile(void);             //删除最老的文件
	void FixFileInfo(void);                 //修订文件名目录队列
	void MakeFileName(void);                //根据时间和文件大小，定制文件名
	void GetFileName(void);                 //获得当前可用的文件名
public:
	static  int MakeATimeString(char *szBuffer,int nBufferSize);//定制时间戳字符串
	void _XGDebugForBin(const char *pBuffer,int nLength);  //二进制输出
	int _XGSysLog(const char *szFormat,...);               //syslog输出
	int _XGDebug(const char *szFormat,...);                //debug 输出
	int _XGDebug2(const char *szFormat,...);   
	int _XGDebug3(const char *szFormat,...);   
public:
	bool m_bSysLogFlag;
	bool m_bDebugFlag;
	bool m_bDebug2Flag;
	bool m_bDebug3Flag;
};

}
#endif
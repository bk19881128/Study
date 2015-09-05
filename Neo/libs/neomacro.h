#ifndef NEOMACRO

#define NEOMACRO

//类库的宏定义

//判断时间是否到了
#define TimeSetNow(t) time(&t)

inline bool TimelsUp(time_t tLast,long lMax)///*最后一次时间*//*超时时间设置*/
{
	time_t tNow;
	TimeSetNow(tNow);
	long lDeltaT=(long)tNow-(long)tLast;
	if(lMax<=lDeltaT)
		return true;
	else
		return false;
}

template <class Parm>
inline void NEOExchange(Parm &p1, Parm &p2)
{
	Parm temp;
	temp = p1;
	p1 = p2;
	p2 = temp;
}
/*根据结构体元素获取结构体首地址指针*/
/*获取成员在结构体中的偏移量*/
#define offset_of(type,member) ((size_t)&((type *)0)->member)
/*ptr为member的有效地址
  type为结构体的类型名，是
  member为type中的元素*/
//当前指针位置减去从头到尾的偏移量就是包含该指针的结构体的首指针
#define container_of(ptr,type,member)\
	({ \
		const typeof(((type *)0)->member) *_mptr = (ptr);\
		(type *)((char *)_mptr - offset_of(type,member));\
	})
	
//debug
#define DEBUG_BUFFER_LENGTH 1024
#define NEO_DEBUG_FILENAME_LENGTH 256
#ifdef WIN32
	#define PATH_CHAR "\\"//用于解决linux与windows路径间隔符的差异
#else
	#define PATH_CHAR "/"
#endif

#ifdef NEO_DEBUG//debug开关用于判定是否是debug
#define NEO_PRINTF printf
#else
#define NEO_PRINTF /\/printf //表明不是debug
#endif

//构建文件名的宏
#define FILENAME_STRING_LENGTH 256//文件名的统一长度
#define FULL_NAME(path,name,fullname,ext_name/*扩展名*/)\
{\
	if(strlen(path))\
    {\
	   if(strlen(ext_name))\
			SafePrintf(fullname,\
			FILENAME_STRING_LENGTH,\
			"%s%s%s.%s",\
			path,\
			PATH_CHAR,\
			name,\
			ext_name);\
	else\
	    SafePrintf(fullname,\
		FILENAME_STRING_LENGTH,\
		"%s%s.%s",\
		path,\
		PATH_CHAR,\
		name);\
    }\
	else\
   {\
        if(strlen(ext_name))\
			SafePrintf(fullname,\
			FILENAME_STRING_LENGTH,\
			"%s.%s",\
			path,\
			PATH_CHAR,\
			name,\
			ext_name);\
		else\
			SafePrintf(fullname,\
			FILENAME_STRING_LENGTH,\
			"%s",\
			name);\
    }\
}
//lock
//锁变量重定义
#ifdef WIN32
#define MUTEX CRITICAL_SECTION
#define MUTEXINIT(m) InitializeCriticalSection(m)
#define MUTEXLOCK(m) EnterCriticalSection(m)
#define MUTEXUNLOCK(m) LeaveCriticalSection(m)
#define MUTEXDESTROY(m) DeleteCriticalSection(m)

#else
#define MUTEX pthread_mutex_t
#define MUTEXINIT(m) pthread_mutex_init(m,NULL)
#define MUTEXLOCK(m) pthread_mutex_lock(m)
#define MUTEXUNLOCK(m) pthread_mutex_unlock(m)
#define MUTEXDESTROY(m) pthread_mutex_destroy(m)

#endif

inline void NEOMinSleep(void)
{
#ifdef WIN32
	Sleep(1);
#else
	struct timespec sleeptm;
	sleeptm.tv_sec=0;
	sleeptm.tv_nsec=1000;//1000ns=1us
	if(nanosleep(&sleeptm,NULL)==-1)
		usleep(1);
#endif 
}
///////////////////////////////////////////////////////////////
//mem manager
//////////////////////////////////////////////////////////////////////////
//自定义计算模块
#define NEO_MEM_BLOCK_SIZE(nDataLen)        (nDataLen+SNeoMemroryBlockHeadSize)
#define NEO_MEM_BLOCK_DATA_SIZE(nBlockSize) (nBlockSize-SNeoMemroryBlockHeadSize)
//根据释放的指针，逆求真实的内存内地址
#define NEO_MEM_BLOCK_HEAD(pData)       ((SNeoMemroryBlockHead*)(((char*)pData)-SNeoMemroryBlockHeadSize))
//求数据块的地址
#define NEO_MEM_BLOCK_DATA(pHead)       (((char *)pHead)+SNeoMemroryBlockHeadSize)
//最小最大内存块
#define NEO_MEM_STACK_BLOCK_MIN 16
#define NEO_MEM_STACK_BLOCK_MAX         (1024*1024)
////////////////////////////////////////////////////////////////////
//内存注册模块
#ifndef NEO_MEM_BLOCK_INFO_MAX_SIZE
#define NEO_MEM_BLOCK_INFO_MAX_SIZE     124
#endif
//管理的指针个数
#ifndef NEO_MEM_REGISTER_MAX
#define NEO_MEM_REGISTER_MAX            10000
#endif
//字符串清空的宏
#define NEO_CLEAN_CHAR_BUFFER(p)        ((*(char *)p)='\0')
///////////////////////////////////////////////////////////////////////
//thread
//各种常量设计
#define OPEN_THREAD_DELAY               250         //连续开启线程的最小时间间隔
#define WHILE_THREAD_COUNT              10         //最多开启10个空闲线程
#define DEFAULT_THREAD_SLEEP            500      //通常建议的线程睡眠时间
#define THREAD_POOL_EXIT_CODE           10000   //线程池退出代码
//线程上限数
#ifdef _ARM_                          //嵌入式arm系统
#define THIS_POOLTHREAD_MAX              30
#else
#ifdef WIN32
#define THIS_POOLTHREAD_MAX              2000
#else
#define THIS_POOLTHREAD_MAX              300
#endif
#endif
///////////////////////////////////////////////////////////////
//线程状态
#define TPOOL_THREAD_STATE_NOT_RUN       0    //线程未运行
#define TPOOL_THREAD_STATE_IDLE          1    //线程空闲
#define TPOOL_THREAD_STATE_BUSY          2    //线程运行
///////////////////////////////////////////////////////////////
//注册返回结果定义
#define _THREADPOOL_CAN_NOT_USE          2    //线程池未初始化，无法工作
#define _THREADPOOL_OVERFLOW             -1      //线程池溢出标志，无法注册
#define _THREADPOOL_PLEASE_WAIT          0    //线程池没有备用线程，请等待
#define _THREADPOOL_OK                   1    //线程池注册成功
//////////////////////////////////////////////////////////////////////////
#define TASK_POOL_TOKEN_MAX              (16*1024)      //最多并发任务数
#define DEFAULT_THREAD_MAX               (30)             //默认最大线程数
#define NEO_TASK_RUN_MAX_TASK            16              //最多步动作
///////////////////////////////////////////////////////////////////////////////
//queue
#define NEO_BUFFER_STRING_MAX            1024
#define NEO_SAFE_BUFFER_MAX_SIZE         (132*1024)
//求元素的长度
#define NEO_POP_BUFFER_TOKEN_LENGTH(n)   (n+SNEOPopBufferTokenHeadSize)
//元素的数据开始位置
#define NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(p) \
	(((char*)p)+SNEOPopBufferHeadSize)
//求数据开始位置的宏
#define NEO_POP_BUFFER_TOKEN_DATA_BEGIN(p)\
	(((char*)p)+SNEOPopBufferTokenHeadSize)
/////////////////////////////////////////////////////////////
#define NEO_APPLICATION_NAME_SIZE        256
#define NEO_CHAIN_TOKEN_MAX              1024
/////////////////////////////////////////////////////////////////////////////////
//log
#define LOG_FILE_SIZE_MAX                (1*1024*1024*1024)   //日志文件最大1g
#define LOG_ITEM_LENGTH_MAX              (2*1024)           //单条log最大2K
#define LOG_FILE_CHANGE_NAME_PRE_SECONDS (60*60)//日志文件一小时更换一次姓名
#define LOG_FILE_INFO_BUFFER_SIZE        (256*1024)   //日志目录最大长度 超出删除
#define LOG_FILE_DEFAULT_HOLD            72               //一般保留三天的数据
#define LOG_TIME_STRING_MAX              128                //时间戳字符串长度
#define FILENAME_STRING_LENGTH           256             //文件名长度
#define APP_INFO_OIT_STRING_MAX          256           
////////////////////////////////////////////////////////////////////////////////////
//base lib
#define MAIN_LOOP_DELAY                  2
#define NEO_APP_LOG_PATH_NAME_SIZE       256/////////////////////???
#define NEO_APP_TEMP_PATH_NAME_SIZE      256////////////////////???


//回调函数原型

typedef void (*_APP_INFO_OUT_CALLBACK)(char *szInfo,void *pCallParam);
#endif
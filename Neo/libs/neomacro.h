#ifndef NEOMACRO

#define NEOMACRO

//���ĺ궨��

//�ж�ʱ���Ƿ���
#define TimeSetNow(t) time(&t)

inline bool TimelsUp(time_t tLast,long lMax)///*���һ��ʱ��*//*��ʱʱ������*/
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
/*���ݽṹ��Ԫ�ػ�ȡ�ṹ���׵�ַָ��*/
/*��ȡ��Ա�ڽṹ���е�ƫ����*/
#define offset_of(type,member) ((size_t)&((type *)0)->member)
/*ptrΪmember����Ч��ַ
  typeΪ�ṹ�������������
  memberΪtype�е�Ԫ��*/
//��ǰָ��λ�ü�ȥ��ͷ��β��ƫ�������ǰ�����ָ��Ľṹ�����ָ��
#define container_of(ptr,type,member)\
	({ \
		const typeof(((type *)0)->member) *_mptr = (ptr);\
		(type *)((char *)_mptr - offset_of(type,member));\
	})
	
//debug
#define DEBUG_BUFFER_LENGTH 1024
#define NEO_DEBUG_FILENAME_LENGTH 256
#ifdef WIN32
	#define PATH_CHAR "\\"//���ڽ��linux��windows·��������Ĳ���
#else
	#define PATH_CHAR "/"
#endif

#ifdef NEO_DEBUG//debug���������ж��Ƿ���debug
#define NEO_PRINTF printf
#else
#define NEO_PRINTF /\/printf //��������debug
#endif

//�����ļ����ĺ�
#define FILENAME_STRING_LENGTH 256//�ļ�����ͳһ����
#define FULL_NAME(path,name,fullname,ext_name/*��չ��*/)\
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
//�������ض���
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
//�Զ������ģ��
#define NEO_MEM_BLOCK_SIZE(nDataLen)        (nDataLen+SNeoMemroryBlockHeadSize)
#define NEO_MEM_BLOCK_DATA_SIZE(nBlockSize) (nBlockSize-SNeoMemroryBlockHeadSize)
//�����ͷŵ�ָ�룬������ʵ���ڴ��ڵ�ַ
#define NEO_MEM_BLOCK_HEAD(pData)       ((SNeoMemroryBlockHead*)(((char*)pData)-SNeoMemroryBlockHeadSize))
//�����ݿ�ĵ�ַ
#define NEO_MEM_BLOCK_DATA(pHead)       (((char *)pHead)+SNeoMemroryBlockHeadSize)
//��С����ڴ��
#define NEO_MEM_STACK_BLOCK_MIN 16
#define NEO_MEM_STACK_BLOCK_MAX         (1024*1024)
////////////////////////////////////////////////////////////////////
//�ڴ�ע��ģ��
#ifndef NEO_MEM_BLOCK_INFO_MAX_SIZE
#define NEO_MEM_BLOCK_INFO_MAX_SIZE     124
#endif
//�����ָ�����
#ifndef NEO_MEM_REGISTER_MAX
#define NEO_MEM_REGISTER_MAX            10000
#endif
//�ַ�����յĺ�
#define NEO_CLEAN_CHAR_BUFFER(p)        ((*(char *)p)='\0')
///////////////////////////////////////////////////////////////////////
//thread
//���ֳ������
#define OPEN_THREAD_DELAY               250         //���������̵߳���Сʱ����
#define WHILE_THREAD_COUNT              10         //��࿪��10�������߳�
#define DEFAULT_THREAD_SLEEP            500      //ͨ��������߳�˯��ʱ��
#define THREAD_POOL_EXIT_CODE           10000   //�̳߳��˳�����
//�߳�������
#ifdef _ARM_                          //Ƕ��ʽarmϵͳ
#define THIS_POOLTHREAD_MAX              30
#else
#ifdef WIN32
#define THIS_POOLTHREAD_MAX              2000
#else
#define THIS_POOLTHREAD_MAX              300
#endif
#endif
///////////////////////////////////////////////////////////////
//�߳�״̬
#define TPOOL_THREAD_STATE_NOT_RUN       0    //�߳�δ����
#define TPOOL_THREAD_STATE_IDLE          1    //�߳̿���
#define TPOOL_THREAD_STATE_BUSY          2    //�߳�����
///////////////////////////////////////////////////////////////
//ע�᷵�ؽ������
#define _THREADPOOL_CAN_NOT_USE          2    //�̳߳�δ��ʼ�����޷�����
#define _THREADPOOL_OVERFLOW             -1      //�̳߳������־���޷�ע��
#define _THREADPOOL_PLEASE_WAIT          0    //�̳߳�û�б����̣߳���ȴ�
#define _THREADPOOL_OK                   1    //�̳߳�ע��ɹ�
//////////////////////////////////////////////////////////////////////////
#define TASK_POOL_TOKEN_MAX              (16*1024)      //��ಢ��������
#define DEFAULT_THREAD_MAX               (30)             //Ĭ������߳���
#define NEO_TASK_RUN_MAX_TASK            16              //��ಽ����
///////////////////////////////////////////////////////////////////////////////
//queue
#define NEO_BUFFER_STRING_MAX            1024
#define NEO_SAFE_BUFFER_MAX_SIZE         (132*1024)
//��Ԫ�صĳ���
#define NEO_POP_BUFFER_TOKEN_LENGTH(n)   (n+SNEOPopBufferTokenHeadSize)
//Ԫ�ص����ݿ�ʼλ��
#define NEO_POP_BUFFER_FIRST_TOKEN_BEGIN(p) \
	(((char*)p)+SNEOPopBufferHeadSize)
//�����ݿ�ʼλ�õĺ�
#define NEO_POP_BUFFER_TOKEN_DATA_BEGIN(p)\
	(((char*)p)+SNEOPopBufferTokenHeadSize)
/////////////////////////////////////////////////////////////
#define NEO_APPLICATION_NAME_SIZE        256
#define NEO_CHAIN_TOKEN_MAX              1024
/////////////////////////////////////////////////////////////////////////////////
//log
#define LOG_FILE_SIZE_MAX                (1*1024*1024*1024)   //��־�ļ����1g
#define LOG_ITEM_LENGTH_MAX              (2*1024)           //����log���2K
#define LOG_FILE_CHANGE_NAME_PRE_SECONDS (60*60)//��־�ļ�һСʱ����һ������
#define LOG_FILE_INFO_BUFFER_SIZE        (256*1024)   //��־Ŀ¼��󳤶� ����ɾ��
#define LOG_FILE_DEFAULT_HOLD            72               //һ�㱣�����������
#define LOG_TIME_STRING_MAX              128                //ʱ����ַ�������
#define FILENAME_STRING_LENGTH           256             //�ļ�������
#define APP_INFO_OIT_STRING_MAX          256           
////////////////////////////////////////////////////////////////////////////////////
//base lib
#define MAIN_LOOP_DELAY                  2
#define NEO_APP_LOG_PATH_NAME_SIZE       256/////////////////////???
#define NEO_APP_TEMP_PATH_NAME_SIZE      256////////////////////???


//�ص�����ԭ��

typedef void (*_APP_INFO_OUT_CALLBACK)(char *szInfo,void *pCallParam);
#endif
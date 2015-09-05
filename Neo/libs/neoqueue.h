#ifndef NEOQUEUE 

#define NEOQUEUE 

namespace NEOLIB {
//pop buffer数据结构
//队列的头
typedef struct _NEO_POP_BUFFER_HEAD_
{
	int m_nTokenCount;             //内部包含的元素个数
	int m_nAllByteSCount;          //使用的字节总数
}SNEOPopBufferHead;
const unsigned long SNEOPopBufferHeadSize=sizeof(SNEOPopBufferHead);

//队列中每个元素的头
typedef struct _NEO_POP_BUFFER_TOKEN_HEAD_
{
	int m_nDataLen;              //标示该单元的数据长度
}SNEOPopBufferTokenHead;
const unsigned long SNEOPopBufferTokenHeadSize=sizeof(SNEOPopBufferTokenHead);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//mem queue 的数据结构
typedef struct _NEO_QUEUE_TOKEN_HEAD_
{
	int m_nDataLen;                    //存储的业务数据长度
	char *m_pBuffer;                   //指向业务数据块的指针
	struct _NEO_QUEUE_TOKEN_HEAD_ *m_pNext;
}SNEOQueueTokenHead;
const unsigned long SNEOQueueTokenHeadSize=sizeof(SNEOQueueTokenHead);
///////////////////////////////////////////////////////////////
//#define NEO_APPLICATION_NAME_SIZE 256
//#define NEO_CHAIN_TOKEN_MAX 1024
//主要为了实现报文拼接和信令构建（依赖内存池）
//动态buffer
class  CNEODynamicBuffer
{ 
public:
	CNEODynamicBuffer(CNEOMemPoolWithLock *pMemPool);
	~CNEODynamicBuffer();
public:
	CNEOMemPoolWithLock *m_pMemPool;//内存池指针
	char *m_pData;//动态内存缓冲区指针
	int m_nDataLen;//内存缓冲区长度
public:
	///////////////////////////////
	//尺寸设置函数
	bool SetSize(int nSize); //设置新的大小
	bool InsertSpaceToHead(int nAddBytes);//在前面插入空白
	bool AddSpaceToTail(int nAddBytes);//在后面插入空白
	void CutHead(int nBytes);// 从前面剪掉一段数据;
	void CutTail(int nBytes);//从后面剪掉一段数据
    ////////////////////////////////////////
	//数值转换函数
	bool SetInt(int n);//讲一个整数以二进制方式拷贝到缓冲区带网络字节序
	int GetInt(void);//以整数方式获得缓冲区的数值
	bool SetShort(short n);//讲一个短整数以二进制方式拷贝到缓冲区带网络字节序

	short GetShort(void);//端口号
	bool SetChar(char n);//将一个字节以二进制方式拷贝到缓冲区
	char GetChar(void);//以字节方式获得缓冲区的数值
	//////////////////////////////////////////
	//二进制数据追加函数
	//追加数据到最后，返回新的长度
	int AddData(char *szData,int nDataLen);
	//插入数据的最前面，返回新的数据长度
	int InsertDataToHead(char *szData,int nDataLen);
	/////////////////////////////////////////
	//二进制数据拷贝函数
	//拷贝到一块目标缓冲区域，受传入的缓冲区长度限制
	int BinCopyTo(char *szBuffer,int nBufferSize);
	//从一块来源缓冲区拷贝数据到本对象中
	int BinCopyFrom(char *szData,int nDataLen);
	//从另外一个Buffer对象拷贝数据到本对象
	int BinCopyFrom(CNEODynamicBuffer *pBuffer);
	/////////////////////////////////////////////
	//文本数据拷贝构建函数
	int StrCopyFrom(char *szString);
	int Printf(const char *szFormat,...);
	/////////////////////////////////////////////
	//数据比较函数
	int memcmp(char *szData,int nDataLen);
	int strcmp(char *szString);
	/**/
	char* GetBuffer(void);
	int GetBufferLength(void);
};
//静态Buffer
class  CNEOStaticBuffer
{
public:
	/*CNEOStaticBuffer(CNEOBaseLibrary *pBaseLib);*/
	CNEOStaticBuffer(CNEOMemPoolWithLock *pMemPool);
	~CNEOStaticBuffer();
public:
	CNEOMemPoolWithLock *m_pMemPool;       //内存池指针
	char m_pData[NEO_SAFE_BUFFER_MAX_SIZE];//静态数组
	int m_nDataLen;                        //内存缓冲区长度
public:
	///////////////////////////////
	//尺寸设置函数
	bool SetSize(int nSize); //设置新的大小
	bool InsertSpaceToHead(int nAddBytes);//在前面插入空白
	bool AddSpaceToTail(int nAddBytes);//在后面插入空白
	void CutHead(int nBytes);// 从前面剪掉一段数据
	void CutTail(int nBytes);//从后面剪掉一段数据
    ////////////////////////////////////////
	//数值转换函数
	bool SetInt(int n);//讲一个整数以二进制方式拷贝到缓冲区带网络字节序
	int GetInt(void);//以整数方式获得缓冲区的数值
	bool SetShort(short n);//讲一个短整数以二进制方式拷贝到缓冲区带网络字节序

	short GetShort(void);//端口号
	bool SetChar(char n);//将一个字节以二进制方式拷贝到缓冲区
	char GetChar(void);//以字节方式获得缓冲区的数值
	//////////////////////////////////////////
	//二进制数据追加函数
	//追加数据到最后，返回新的长度
	int AddData(char *szData,int nDataLen);
	//插入数据的最前面，返回新的数据长度
	int InsertDataToHead(char *szData,int nDataLen);
	/////////////////////////////////////////
	//二进制数据拷贝函数
	//拷贝到一块目标缓冲区域，受传入的缓冲区长度限制
	int BinCopyTo(char *szBuffer,int nBufferSize);
	//从一块来源缓冲区拷贝数据到本对象中
	int BinCopyFrom(char *szData,int nDataLen);
	//从另外一个Buffer对象拷贝数据到本对象
	int BinCopyFrom(CNEODynamicBuffer *pBuffer);
	/////////////////////////////////////////////
	//文本数据拷贝构建函数
	int StrCopyFrom(char *szString);
	int Printf(char *szFormat,...);
	/////////////////////////////////////////////
	//数据比较函数
	int memcmp(char *szData,int nDataLen);
	int strcmp(char *szString);
	////////////////////////////////////////////
	//服务函数
	bool IHaveData(void);

	char* GetBuffer(void);
	int GetBufferLength(void);
};
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//用于通信双方的操作系统一样的情况（效率偏低）
//回调函数构型
typedef bool(*_NEO_ENUM_DATA_CALLBACK)(
	char *szData,//数据指针
	int nDataLen//数据长度
	,void* pCallParam);//代传的参数
//static bool EnumDataCallback(char *szData,int nDataLen,void *pCallParam);
//基本队列模型
class  CNEOPopBuffer
{
public:
	CNEOPopBuffer(char *szBuffer,           //缓冲区指针
		int nBufferSize,                    //缓冲区尺寸
		bool bInitFlag=true);                //是否初始化标志
	~CNEOPopBuffer();
public:
	char *m_pBuffer;                       //最关键的内部缓冲区指针
	int m_nBufferSize;                     //内部缓冲区的长度
private:
	SNEOPopBufferHead *m_pHead;            //队列头指针
public:
	//实现粘合的方法,外部传入的缓冲区域内部缓冲区的指针挂接
	void Set(char *szBuffer,int nBufferSize);
	//清空整个区域，数据清零，缓冲区不释放
	void Clean(void);
	//内部信息打印函数
	void PrintInside();
	//能否正确工作的标志函数
	bool ICanWork(void);
	//队列最经典的功能，追加到末尾，
	int AddLast(const char *szData,int nDataLength);
	int AddLast(CNEODynamicBuffer *pBuffer);
	int AddLast(CNEOStaticBuffer *pBuffer);
	//获得当前内部元素个数
	int GetTokenCount(void);
	//获得所有的字节数
	int GetAllBytes(void);
	//判断内部剩余空间是否够用,是否够新加入的数据用
	bool ICanSave(int nDataLen);
	//获得第一个元素的长度
	int GetFirstTokenLength(void);
	//获取第一个元素
	int GetFirst(char *szBuffer,int nBufferSize);
	int GetFirst(CNEODynamicBuffer *pBufferh);/////////////////////////////////////////////////////////////////???
	int GetFirst(CNEOStaticBuffer *pBuffer);
	//删除一个元素
	bool DeleteFirst(void);
	//从队列总弹出第一个元素
	int GetAndDeleteFirst(char *szBuffer,int nBufferSize);
	int GetAndDeleteFirst(CNEODynamicBuffer *pBufferh);///////////////////////////////////////////////////////////////??
	int GetAndDeleteFirst(CNEOStaticBuffer *pBuffer);
	//枚举遍历所有数据，提交回调函数处理，并删除数据
	int MoveAllData(_NEO_ENUM_DATA_CALLBACK pCallBack,void *pCallParam);
};

//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//动态内存分配，队列采用列表的(依赖内存池)
class  CNEOMemQueue
{
private:
	int m_nMaxToken;                        //最大的Token限制
	int m_nTokenCount;                      //队列中有效地Token个数

	SNEOQueueTokenHead *m_pHead;            //队列头指针
	SNEOQueueTokenHead *m_pLast;            //加速因子，队列尾指针
	CNEOLowDebug *m_pDebug;
	CNEOMemPoolWithLock *m_pMemPool;        
	char m_szAppName[NEO_APPLICATION_NAME_SIZE];
public:
	CNEOMemQueue(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,
		const char *szAppName,                    // 应用程序名，代表队列名
		int nMaxToken=NEO_CHAIN_TOKEN_MAX  //最大token上限
		);
	~CNEOMemQueue();
public:
	bool ICanWork(void);
	void CleanAll(void);              //清除所有token;
	int GetFirstLength(void);        // 获得第一个token数据长度
	int GetTokenCount(void);        //获得token总数
	void PrintInside(void);         //打印搜有队列内部token数据
	int AddLast(const char *szData       //数据指针
		,int nDataLen,             //数据长度
		int nLimit=-1);              //防止递归长度过神
	int GetFirst(char *szBuffer,int nBufferSize);
	int GetFirst(CNEOStaticBuffer *pBuffer);
	int GetFirst(CNEODynamicBuffer *pBuffer);
	bool DeleteFirst(void);
	int GetAndDeleteFirst(char *szBuffer,int nBufferSize);
	int GetAndDeleteFirst(CNEOStaticBuffer *pBuffer);////////////////////////未完//////////////////////////////////////??????????
	int GetAndDeleteFirst(CNEODynamicBuffer *pBuffer);

	//从前面弹出一批数据，
	int PopFromFirst(char *szBuffer,int nBufferSize);
	//从popbuffer的数据区弹出所有数据，追加到队列尾
	int PushToLast(char *szData,int nDataLen);

     //队列数据写入磁盘
	void WriteToFile(const char *szFileName);
	//队列数据从磁盘读出
	int ReadFromFile(const char *szFileName);
private:
	void PrintAToken(SNEOQueueTokenHead *pToken);
	void WriteATokenToFile(SNEOQueueTokenHead *pToken,FILE *fp);
	int PopFromFirst4NEOPopBuffer(CNEOPopBuffer *pPopBuffer);
	//申请用来存储数据的内存
	int AddLastToThisToken(SNEOQueueTokenHead *pToken,const char *szData,int nDataLen);
	//申请一个token头
	SNEOQueueTokenHead *GetAToken(void);//返回指针，不遵守谁申请谁释放的原则
	bool DeleteATokenAndHisNext(SNEOQueueTokenHead *pToken);
	//回调函数申明
	static bool PushDataCallback(char *szData,int nDataLen,void *pCallParam);
};

//带安全锁的类
class  CNEOMemQueueWithLock
{
public:
	CNEOMemQueueWithLock(CNEOLowDebug *pDebug,
		CNEOMemPoolWithLock *pMemPool,
		const char *szAppName,                    // 应用程序名，代表队列名
		int nMaxToken=NEO_CHAIN_TOKEN_MAX
		);
	~CNEOMemQueueWithLock();
public:
	bool ICanWork(void);
	int AddLast(const char *szData,int nDataLen);
	int GetFirst(CNEODynamicBuffer *pBuffer);
	int GetFirst(char *szBuffer,int nBufferSize);
	int GetFirstLen(void);
	int GetTokenCount(void);
	int GetAndDeleteFirst(char *szBuffer,int nBufferSize);
	int PopFromFirst(char *szBuffer,int nBufferSize);
	int PushToLast(char *szData,int nDataLen);
	void CleanAll(void);
	bool DeleteFirst(void);
	void WriteToFile(const char *szFileName);
	int ReadFromFile(const char *szFileName);
	void PrintInside();
private:
	CNEOMemPoolWithLock *m_pMemPool;
	CNEOMemQueue *m_pQueue;
	CMultiReadSingleWriteLock m_Lock;
	char m_szAppName[NEO_APPLICATION_NAME_SIZE];
};

}
#endif
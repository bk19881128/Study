#ifndef NEOQUEUE 

#define NEOQUEUE 

namespace NEOLIB {
//pop buffer���ݽṹ
//���е�ͷ
typedef struct _NEO_POP_BUFFER_HEAD_
{
	int m_nTokenCount;             //�ڲ�������Ԫ�ظ���
	int m_nAllByteSCount;          //ʹ�õ��ֽ�����
}SNEOPopBufferHead;
const unsigned long SNEOPopBufferHeadSize=sizeof(SNEOPopBufferHead);

//������ÿ��Ԫ�ص�ͷ
typedef struct _NEO_POP_BUFFER_TOKEN_HEAD_
{
	int m_nDataLen;              //��ʾ�õ�Ԫ�����ݳ���
}SNEOPopBufferTokenHead;
const unsigned long SNEOPopBufferTokenHeadSize=sizeof(SNEOPopBufferTokenHead);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//mem queue �����ݽṹ
typedef struct _NEO_QUEUE_TOKEN_HEAD_
{
	int m_nDataLen;                    //�洢��ҵ�����ݳ���
	char *m_pBuffer;                   //ָ��ҵ�����ݿ��ָ��
	struct _NEO_QUEUE_TOKEN_HEAD_ *m_pNext;
}SNEOQueueTokenHead;
const unsigned long SNEOQueueTokenHeadSize=sizeof(SNEOQueueTokenHead);
///////////////////////////////////////////////////////////////
//#define NEO_APPLICATION_NAME_SIZE 256
//#define NEO_CHAIN_TOKEN_MAX 1024
//��ҪΪ��ʵ�ֱ���ƴ�Ӻ�������������ڴ�أ�
//��̬buffer
class  CNEODynamicBuffer
{ 
public:
	CNEODynamicBuffer(CNEOMemPoolWithLock *pMemPool);
	~CNEODynamicBuffer();
public:
	CNEOMemPoolWithLock *m_pMemPool;//�ڴ��ָ��
	char *m_pData;//��̬�ڴ滺����ָ��
	int m_nDataLen;//�ڴ滺��������
public:
	///////////////////////////////
	//�ߴ����ú���
	bool SetSize(int nSize); //�����µĴ�С
	bool InsertSpaceToHead(int nAddBytes);//��ǰ�����հ�
	bool AddSpaceToTail(int nAddBytes);//�ں������հ�
	void CutHead(int nBytes);// ��ǰ�����һ������;
	void CutTail(int nBytes);//�Ӻ������һ������
    ////////////////////////////////////////
	//��ֵת������
	bool SetInt(int n);//��һ�������Զ����Ʒ�ʽ�������������������ֽ���
	int GetInt(void);//��������ʽ��û���������ֵ
	bool SetShort(short n);//��һ���������Զ����Ʒ�ʽ�������������������ֽ���

	short GetShort(void);//�˿ں�
	bool SetChar(char n);//��һ���ֽ��Զ����Ʒ�ʽ������������
	char GetChar(void);//���ֽڷ�ʽ��û���������ֵ
	//////////////////////////////////////////
	//����������׷�Ӻ���
	//׷�����ݵ���󣬷����µĳ���
	int AddData(char *szData,int nDataLen);
	//�������ݵ���ǰ�棬�����µ����ݳ���
	int InsertDataToHead(char *szData,int nDataLen);
	/////////////////////////////////////////
	//���������ݿ�������
	//������һ��Ŀ�껺�������ܴ���Ļ�������������
	int BinCopyTo(char *szBuffer,int nBufferSize);
	//��һ����Դ�������������ݵ���������
	int BinCopyFrom(char *szData,int nDataLen);
	//������һ��Buffer���󿽱����ݵ�������
	int BinCopyFrom(CNEODynamicBuffer *pBuffer);
	/////////////////////////////////////////////
	//�ı����ݿ�����������
	int StrCopyFrom(char *szString);
	int Printf(const char *szFormat,...);
	/////////////////////////////////////////////
	//���ݱȽϺ���
	int memcmp(char *szData,int nDataLen);
	int strcmp(char *szString);
	/**/
	char* GetBuffer(void);
	int GetBufferLength(void);
};
//��̬Buffer
class  CNEOStaticBuffer
{
public:
	/*CNEOStaticBuffer(CNEOBaseLibrary *pBaseLib);*/
	CNEOStaticBuffer(CNEOMemPoolWithLock *pMemPool);
	~CNEOStaticBuffer();
public:
	CNEOMemPoolWithLock *m_pMemPool;       //�ڴ��ָ��
	char m_pData[NEO_SAFE_BUFFER_MAX_SIZE];//��̬����
	int m_nDataLen;                        //�ڴ滺��������
public:
	///////////////////////////////
	//�ߴ����ú���
	bool SetSize(int nSize); //�����µĴ�С
	bool InsertSpaceToHead(int nAddBytes);//��ǰ�����հ�
	bool AddSpaceToTail(int nAddBytes);//�ں������հ�
	void CutHead(int nBytes);// ��ǰ�����һ������
	void CutTail(int nBytes);//�Ӻ������һ������
    ////////////////////////////////////////
	//��ֵת������
	bool SetInt(int n);//��һ�������Զ����Ʒ�ʽ�������������������ֽ���
	int GetInt(void);//��������ʽ��û���������ֵ
	bool SetShort(short n);//��һ���������Զ����Ʒ�ʽ�������������������ֽ���

	short GetShort(void);//�˿ں�
	bool SetChar(char n);//��һ���ֽ��Զ����Ʒ�ʽ������������
	char GetChar(void);//���ֽڷ�ʽ��û���������ֵ
	//////////////////////////////////////////
	//����������׷�Ӻ���
	//׷�����ݵ���󣬷����µĳ���
	int AddData(char *szData,int nDataLen);
	//�������ݵ���ǰ�棬�����µ����ݳ���
	int InsertDataToHead(char *szData,int nDataLen);
	/////////////////////////////////////////
	//���������ݿ�������
	//������һ��Ŀ�껺�������ܴ���Ļ�������������
	int BinCopyTo(char *szBuffer,int nBufferSize);
	//��һ����Դ�������������ݵ���������
	int BinCopyFrom(char *szData,int nDataLen);
	//������һ��Buffer���󿽱����ݵ�������
	int BinCopyFrom(CNEODynamicBuffer *pBuffer);
	/////////////////////////////////////////////
	//�ı����ݿ�����������
	int StrCopyFrom(char *szString);
	int Printf(char *szFormat,...);
	/////////////////////////////////////////////
	//���ݱȽϺ���
	int memcmp(char *szData,int nDataLen);
	int strcmp(char *szString);
	////////////////////////////////////////////
	//������
	bool IHaveData(void);

	char* GetBuffer(void);
	int GetBufferLength(void);
};
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//����ͨ��˫���Ĳ���ϵͳһ���������Ч��ƫ�ͣ�
//�ص���������
typedef bool(*_NEO_ENUM_DATA_CALLBACK)(
	char *szData,//����ָ��
	int nDataLen//���ݳ���
	,void* pCallParam);//�����Ĳ���
//static bool EnumDataCallback(char *szData,int nDataLen,void *pCallParam);
//��������ģ��
class  CNEOPopBuffer
{
public:
	CNEOPopBuffer(char *szBuffer,           //������ָ��
		int nBufferSize,                    //�������ߴ�
		bool bInitFlag=true);                //�Ƿ��ʼ����־
	~CNEOPopBuffer();
public:
	char *m_pBuffer;                       //��ؼ����ڲ�������ָ��
	int m_nBufferSize;                     //�ڲ��������ĳ���
private:
	SNEOPopBufferHead *m_pHead;            //����ͷָ��
public:
	//ʵ��ճ�ϵķ���,�ⲿ����Ļ��������ڲ���������ָ��ҽ�
	void Set(char *szBuffer,int nBufferSize);
	//������������������㣬���������ͷ�
	void Clean(void);
	//�ڲ���Ϣ��ӡ����
	void PrintInside();
	//�ܷ���ȷ�����ı�־����
	bool ICanWork(void);
	//�������Ĺ��ܣ�׷�ӵ�ĩβ��
	int AddLast(const char *szData,int nDataLength);
	int AddLast(CNEODynamicBuffer *pBuffer);
	int AddLast(CNEOStaticBuffer *pBuffer);
	//��õ�ǰ�ڲ�Ԫ�ظ���
	int GetTokenCount(void);
	//������е��ֽ���
	int GetAllBytes(void);
	//�ж��ڲ�ʣ��ռ��Ƿ���,�Ƿ��¼����������
	bool ICanSave(int nDataLen);
	//��õ�һ��Ԫ�صĳ���
	int GetFirstTokenLength(void);
	//��ȡ��һ��Ԫ��
	int GetFirst(char *szBuffer,int nBufferSize);
	int GetFirst(CNEODynamicBuffer *pBufferh);/////////////////////////////////////////////////////////////////???
	int GetFirst(CNEOStaticBuffer *pBuffer);
	//ɾ��һ��Ԫ��
	bool DeleteFirst(void);
	//�Ӷ����ܵ�����һ��Ԫ��
	int GetAndDeleteFirst(char *szBuffer,int nBufferSize);
	int GetAndDeleteFirst(CNEODynamicBuffer *pBufferh);///////////////////////////////////////////////////////////////??
	int GetAndDeleteFirst(CNEOStaticBuffer *pBuffer);
	//ö�ٱ����������ݣ��ύ�ص�����������ɾ������
	int MoveAllData(_NEO_ENUM_DATA_CALLBACK pCallBack,void *pCallParam);
};

//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//��̬�ڴ���䣬���в����б��(�����ڴ��)
class  CNEOMemQueue
{
private:
	int m_nMaxToken;                        //����Token����
	int m_nTokenCount;                      //��������Ч��Token����

	SNEOQueueTokenHead *m_pHead;            //����ͷָ��
	SNEOQueueTokenHead *m_pLast;            //�������ӣ�����βָ��
	CNEOLowDebug *m_pDebug;
	CNEOMemPoolWithLock *m_pMemPool;        
	char m_szAppName[NEO_APPLICATION_NAME_SIZE];
public:
	CNEOMemQueue(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,
		const char *szAppName,                    // Ӧ�ó����������������
		int nMaxToken=NEO_CHAIN_TOKEN_MAX  //���token����
		);
	~CNEOMemQueue();
public:
	bool ICanWork(void);
	void CleanAll(void);              //�������token;
	int GetFirstLength(void);        // ��õ�һ��token���ݳ���
	int GetTokenCount(void);        //���token����
	void PrintInside(void);         //��ӡ���ж����ڲ�token����
	int AddLast(const char *szData       //����ָ��
		,int nDataLen,             //���ݳ���
		int nLimit=-1);              //��ֹ�ݹ鳤�ȹ���
	int GetFirst(char *szBuffer,int nBufferSize);
	int GetFirst(CNEOStaticBuffer *pBuffer);
	int GetFirst(CNEODynamicBuffer *pBuffer);
	bool DeleteFirst(void);
	int GetAndDeleteFirst(char *szBuffer,int nBufferSize);
	int GetAndDeleteFirst(CNEOStaticBuffer *pBuffer);////////////////////////δ��//////////////////////////////////////??????????
	int GetAndDeleteFirst(CNEODynamicBuffer *pBuffer);

	//��ǰ�浯��һ�����ݣ�
	int PopFromFirst(char *szBuffer,int nBufferSize);
	//��popbuffer�������������������ݣ�׷�ӵ�����β
	int PushToLast(char *szData,int nDataLen);

     //��������д�����
	void WriteToFile(const char *szFileName);
	//�������ݴӴ��̶���
	int ReadFromFile(const char *szFileName);
private:
	void PrintAToken(SNEOQueueTokenHead *pToken);
	void WriteATokenToFile(SNEOQueueTokenHead *pToken,FILE *fp);
	int PopFromFirst4NEOPopBuffer(CNEOPopBuffer *pPopBuffer);
	//���������洢���ݵ��ڴ�
	int AddLastToThisToken(SNEOQueueTokenHead *pToken,const char *szData,int nDataLen);
	//����һ��tokenͷ
	SNEOQueueTokenHead *GetAToken(void);//����ָ�룬������˭����˭�ͷŵ�ԭ��
	bool DeleteATokenAndHisNext(SNEOQueueTokenHead *pToken);
	//�ص���������
	static bool PushDataCallback(char *szData,int nDataLen,void *pCallParam);
};

//����ȫ������
class  CNEOMemQueueWithLock
{
public:
	CNEOMemQueueWithLock(CNEOLowDebug *pDebug,
		CNEOMemPoolWithLock *pMemPool,
		const char *szAppName,                    // Ӧ�ó����������������
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
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
//#define LOG_FILE_SIZE_MAX (1*1024*1024*1024)   //��־�ļ����1g
//#define LOG_ITEM_LENGTH_MAX (2*1024)           //����log���2K
//#define LOG_FILE_CHANGE_NAME_PRE_SECONDS (60*60)//��־�ļ�һСʱ����һ������
//#define LOG_FILE_INFO_BUFFER_SIZE (256*1024)   //��־Ŀ¼��󳤶� ����ɾ��
//#define LOG_FILE_DEFAULT_HOLD 72               //һ�㱣�����������
//#define LOG_TIME_STRING_MAX 128                //ʱ����ַ�������
//#define FILENAME_STRING_LENGTH 256             //�ļ�������
//#define APP_INFO_OIT_STRING_MAX 256           
//////////////////////////////////////////////////////////////
//��־��
class  CNEOLog
{
public:
	CNEOLog(CNEOLowDebug *pDebug,
		CNEOMemPoolWithLock *pMemPool,        //�ڴ�ָ���
		const char *szLogPath,                      //��־·��
		const char *szAppName,                      //Ӧ����
		int nHoldFileMax=LOG_FILE_DEFAULT_HOLD,//���������ļ���
		bool bSyslogFlag=true,                //��־���𿪹�
		bool bDebugFlag=true,
		bool bDebug2Flag=false,
		bool bDebug3Flag=false,
		bool bPrintfToScrFlag=true           //�Ƿ�����Ļ��ӡ
		);
	~CNEOLog();
private://�ڲ�˽�б���
	CMutexLock m_Lock;                      //�̰߳�ȫ��
	char m_szFilePath[FILENAME_STRING_LENGTH];//�ļ�·��
	char m_szFileName[FILENAME_STRING_LENGTH*2];//�ļ���
	unsigned long m_nFileSize;                //��ǰ�ļ���С
	time_t m_tFileNameMake;                   //�����ļ�����ʱ���
	int m_nHoldFileMax;                       //�����ļ����������ɹ��캯������
	_APP_INFO_OUT_CALLBACK m_pInfoOutCallback;// Ӧ�ó�����������ص�����
	void *m_pInfoOutCallbackParam;            //͸���Ļص���������
	bool m_bPrintfToScrFlag;                  //�Ƿ��ӡ����Ļ
	char m_szFileInfoName[FILENAME_STRING_LENGTH];//�ļ���
	CNEOLowDebug *m_pDebug;
	CNEOMemPoolWithLock *m_pMemPool;          //�ڴ��ָ��
	CNEOMemQueue *m_pFileInfoQueue;           //�ļ�������
private:
	//�ڲ����ߺ���
	int _Printf(const char *szFormat,...);        //���Ĵ�ӡ���ģ��
	void DeleteFirstFile(void);             //ɾ�����ϵ��ļ�
	void FixFileInfo(void);                 //�޶��ļ���Ŀ¼����
	void MakeFileName(void);                //����ʱ����ļ���С�������ļ���
	void GetFileName(void);                 //��õ�ǰ���õ��ļ���
public:
	static  int MakeATimeString(char *szBuffer,int nBufferSize);//����ʱ����ַ���
	void _XGDebugForBin(const char *pBuffer,int nLength);  //���������
	int _XGSysLog(const char *szFormat,...);               //syslog���
	int _XGDebug(const char *szFormat,...);                //debug ���
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
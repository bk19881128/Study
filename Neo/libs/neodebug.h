#ifndef NEODEBUG  

#define NEODEBUG  
#include "neoindex.h"
#include "neolock.h"

namespace NEOLIB {
///////////////////////////////////////////////export fun
typedef struct COUNT_SUB//��ֵ�����ݽṹ
{
    unsigned long m_nBegin;
    unsigned long m_nEnd;
}SCountSub;
typedef struct _COUNT_//ͳ��ƽ��ֵģ��
{
    SCountSub m_Sub;//��ֵ
    unsigned long m_Sum;//ͳ��ƽ��ֵ
}SCount;
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//�ַ����
 int SafePrintf(char *szBuf,int nMaxlength,const char *szFormat,...);
//���ʱ���
 int GetTimeStamp(char *szBuf,int nMaxlength);
//��ӡ���ļ�����Ļ
 int OutputFileOrScreen(const char *szFileName,const char *szMode,const char *szFormate,...);
//�Զ������������
 int Debug_bin_ASC(char *pPrintBuffer,const char *pBuffer,int nLength);
//��16�����������
 int Debug_bin_HEX(char *pPrintBuffer,const char *pBuffer,int nLength);
//�������ں���
 void Debug_bin(const char *pBuffer,int nLength);
///////////////////////////////////////////////////////////////
//��ȡ��ֵ�ĺ���
 unsigned long SCountSubGetX(SCountSub &CountSub);
 void SCountSubSetBegin(SCountSub &CountSub,unsigned long n);
 unsigned long SCountSubSetEnd(SCountSub &CountSub,unsigned long n);
///////////////////////////////////////////////////////////////
//ͳ��ƽ��ֵģ��
 void SCountReset(SCount &Count);//��ʼ��
 unsigned long SCountSum(SCount &Count);//����ͳ��ƽ��ֵ
 unsigned long SCountGetSum(SCount &Count);//����sumֵ
 unsigned long SCountGetX(SCount &Count);//���ص�ǰ�Ĳ�ֵ
 void SCountSetBegin(SCount &Count,unsigned long n);
 unsigned long SCountSetEnd(SCount &Count,unsigned long n);
////////////////////////////////////////////////////////////////
//�������ܺ���
//��÷��������
 inline int GetNot0(void);
 inline int Get0(void);
//��ø�����Χ�ڵ������
 inline int GetRandomBeteen(int nBegin,int nEnd);
//�ж�ʱ���Ƿ���
 //inline bool TimelsUp(long tLast,long lMax);
//�����ֵ����
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
//ƽ��ֵ���㣨ÿ�뷢�����ٴζ�����
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
    double GetOperationsPerSecond(unsigned long ulOperationCount/*�ڼ�Ĳ����ܺ�*/);
};
//////////////////////////////////////
//����ƽ��ֵ����
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
//CLowDebug�࣬���ھ��幦�ܵ����
class  CNEOLowDebug
{
public:
    //ɾ��һ���ļ�
    static void DeleteAFile(const char *szFileName);
    //����·����ȡ�ļ���
    static char *GetTrueFileName(char *szBuffer);
public:
    //����ַ���DebugToFile���ļ������̨�������ַ�����������\0
    int DebugToFile(const char *szFormat,...);
    //���һ���ڴ�ε��ļ���
    void DebugToFileBin(char *pBuffer,int nLength);
public:
    //���캯������������
    CNEOLowDebug (const char *szPathName,                //·����
        const char *szAppName,                           //�ļ���
        _APP_INFO_OUT_CALLBACK pInfoOutCallback=NULL,//���������ص�����
        void *pInfoOutCallbackParam=NULL,          //�ص���������
        bool bePrintToScreenFlag=false           //�Ƿ��ӡ����Ļ
        );
    ~CNEOLowDebug();
public:
    //�ص���������
    //����Ҫ��ʱ��ץȡ�����Ϣ�������Լ����������
    _APP_INFO_OUT_CALLBACK pInfoOutCallback;
    //���������ṩ��ָ��
    void *pInfoOutCallbackParam;
private:
    bool m_bePrintToScreenFlag;                  //�ڲ�������������
    char m_szFileName[NEO_DEBUG_FILENAME_LENGTH];//ƴ�Ӻõ�·����+�ļ���
    CMutexLock m_Lock;                           //�̰߳�ȫ��
};

}
#endif
#ifndef NEO_SORT
#define NEO_SORT
#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////
/*����Ϊ�����㷨������ģ��T��ȷ�������㷨��key���ͣ�flag=0��ʾ�����������У������ʾ����*/
template <typename T>
inline void Exchange(T *pA, T *pB)
{
	T temp = 0;
	temp = *pA;
	*pA = *pB;
	*pB = temp;
}

template <typename T>
void Insert_Sort(T *pInput, unsigned int uiLength, int flag = 0);
template <typename T>
void Merge_Sort(T *pInput, unsigned int uiBeg, unsigned int uiEnd, int flag = 0);
/*uiIndex��ʾ��ǰ������Ԫ�ص�����
uiLength ��ʾ�ѵĴ�С,�Ѵ�����0��ʼ*/
template <typename T>
void Update_Heap(T *pInput, unsigned int uiIndex, unsigned int uiLength, int flag=0);
template <typename T>
void Build_Heap(T *pInput, unsigned int uiLength, int flag=0);
template <typename T>
void Heap_Sort(T *pInput, unsigned int uiLength, int flag=0);
template <typename T>
void Select_Sort(T *pInput, unsigned int uiLength, int flag = 0);
template <typename T>
void Bubble_Sort(T *pInput, unsigned int uiLength, int flag = 0);
template <typename T>
void Count_Sort(T *pInput, T *pOutput,unsigned int uiLength, unsigned int uiMax, int flag = 0);

#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
#endif
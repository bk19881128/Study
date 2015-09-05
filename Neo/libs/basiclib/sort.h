#ifndef NEO_SORT
#define NEO_SORT
#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////
/*本节为排序算法，采用模板T来确定排序算法的key类型，flag=0表示按照升序排列，非零表示降序*/
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
/*uiIndex表示当前调整的元素的索引
uiLength 表示堆的大小,堆从索引0开始*/
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
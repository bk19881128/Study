#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
using namespace std;
#endif

#include "neoindex.h"
#include "neomacro.h"
#include "sort.h"

////////////////////////////////////////////////////////////////////////////
template <typename T> void Insert_Sort(INOUT T *pInput, IN unsigned int uiLength, int flag = 0)
{
    if (uiLength <= 0)
    {
        return;
    }
    for (int i = 1; i <= uiLength; i++)
    {
        T temp = pInput[i];
        for (int j = i-1; j >=0 ; j--)
        {
            if( 0 == flag )
            {
                if ( temp < pInput[j] )
                {
                    pInput[j+1] = pInput[j];
                }
            }
            else
            {
                if ( temp > pInput[j] )
                {
                    pInput[j+1] = pInput[j];
                }
            }
        }//for
        pInput[j+1] = temp;
    }//for
    return;
}
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*Merge sort*/
 template <typename T> static void Merge(INOUT T *pInput, IN unsigned int uiBeg, IN unsigned int uiMid, IN unsigned int uiEnd, IN int flag)
{
    unsigned int n1 = uiMid - uiBeg + 1;
    unsigned int n2 = uiEnd -uiMid;

    T zLeft[n1] = {0};
    T zRight[n2] = {0};
    unsigned int j=k=0;

    for (int i=0; i<n1; i++)
    {
        zLeft[i] = pInput[i];
    }

    for (int i=0; i<n2; i++)
    {
        zRight[i] = pInput[uiMid+1+i];
    }

    for (int i=uiBeg; i<=uiEnd; i++)
    {
        if (flag==0)
        {
            if(zLeft[j]<=B[k])
            {
                pInput[i] = zLeft[j++];
            }
            else
            {
                pInput[i] = zRight[k++];
            }
        }
        else
        {
            if(zLeft[j]>=zRight[k])
            {
                pInput[i] = zLeft[j++];
            }
            else
            {
                pInput[i] = zRight[k++];
            }
        }
    }
}
template <typename T> void Merge_Sort(INOUT T *pInput, IN unsigned int uiBeg, IN unsigned int uiEnd, IN int flag = 0)
{
    if (uiEnd <= uiBeg)
    {
        return;
    }
     
    unsigned int uiMid = (uiBeg + uiEnd)/2;

    Merge_Sort(pInput, uiBeg, uiMid, flag);
    Merge_Sort(pInput, uiMid+1, uiEnd, flag);
    Merge(pInput, uiBeg, uiMid, uiEnd, flag);

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*快速排序*/

template <typename T> static T PickAPrivoit()
{
    return;
}

template <typename T> static T Partition(INOUT T *pInput,IN unsigned int uiBeg,IN unsigned int uiEnd,IN int flag=0)
{
    T privoit =  PickAPrivoit();
    int iMid= uiBeg-1;//i记录小于privoit值的最新位置,j记录
    
    for (int i=uiBeg; i<=uiEnd; i++)
    {
        if( pInput[i]<privoit )
        {
            ++iMid;
            Exchange(&pInput[iMid+1], &pInput[i]);
        }
    }
    return iMid;
}

template <typename T> void Quick_Sort(INOUT T *pInput, IN unsigned int uiBeg,IN unsigned int uiEnd,IN int flag=0)
{
    if ( uiBeg >= uiEnd )
    {
        return;
    }

    unsigned int uiMid = Partition(pInput, uiBeg, uiEnd, flag);
    Quick_Sort(pInput, uiBeg, uiMid, flag);
    Quick_Sort(pInput, uiMid+1, uiEnd, flag);

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*堆排序*/

template <typename T> void Update_Heap(T *pInput, unsigned int uiIndex, unsigned int uiLength, int flag=0) 
{
    unsigned int l = 2*uiIndex;
    unsigned int r = 2*uiIndex+1;
    unsigned int uiChoose = uiIndex;

    if(flag == 0)
    {
        if (l<uiLength && pIndex[l]<pIndex[uiIndex])
        {
            uiChoose = l;
        }
    
        if (r<uiLength && pIndex[r]<pIndex[uiIndex])
        {
            uiChoose = r;
        }
    }
    else
    {
        if (l<uiLength && pIndex[l]>pIndex[uiIndex])
        {
            uiChoose = l;
        }
    
        if (r<uiLength && pIndex[r]>pIndex[uiIndex])
        {
            uiChoose = r;
        }
    }

    if(uiChoose != uiIndex)
    {
        Exchange(&pInput[uiIndex], &pInput[uiChoose]);
        Update_Heap(pInput, uiChoose, uiLength, flag);
    }
}

template <typename T> void Build_Heap(T *pInput, unsigned int uiLength, int flag=0)
{
    for (unsigned int i=uiLength/2; i>=0; i--)
    {
        Update_Heap(pInput, i, uiLength, flag);
    }
}
/*建小顶堆,排序结果为降序,大顶堆相反*/
template <typename T> void Heap_Sort(T *pInput, unsigned int uiLength, int flag=0)
{
    Build_Heap(pInput, uiLength, !flag);

    for (int i=uiLength; i>=0; i--)
    {
        Exchange(&pInput[1], &pInput[uiLength]);
        uiLength--;
        Update_Heap(pInput, 1, uiLength, !flag);
    }
}

template <typename T> void Select_Sort(T *pInput, unsigned int uiLength, int flag = 0)
{
    if (uiLength<=0)
    {
        return;
    }
    unsigned int uiIndex = 0;
    for (int i=0; i<uiLength; i++)
    {
        T xPolar = pInput[i];
        for (int j=i; j<uiLength; j++)
        {
            if (flag==0)
            {
                if (xPolar>pInput[j])
                {
                    xPolar = pInput[j];
                    uiIndex = j;
                }
            }
            else
            {
                if (xPolar<pInput[j])
                {
                    xPolar = pInput[j];
                    uiIndex = j;
                }
            }//if
        }//for

        if(i!=uiIndex)
        {
            Exchange(&pInput[uiIndex], &pInput[i]);
        }
    }//for
    return;
}

template <typename T> void Bubble_Sort(T *pInput, unsigned int uiLength, int flag = 0)
{
    bool bChange = true;

    for (int i=uiLength-1, bChange=true; i>=1 && bChange; i--)
    {
        bChange = false;
        for (int j=0;j<i;j++)
        {   
            if (flag==0)
            {
                if (pInput[j]>pInput[j+1])
                {
                    Exchange(&pInput[j], &pInput[j+1]);
                    bChange = true;
                }
            }
            else
            {
                if (pInput[j]<pInput[j+1])
                {
                    Exchange(&pInput[j], &pInput[j+1]);
                    bChange = true;
                }
            }
        }//for
    }//for
    return;
}
/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
template <typename T> void Count_Sort(T *pInput, T *pOutput,unsigned int uiLength, unsigned int uiMax, int flag = 0)
{
    T xCounter[uiMax] = {0};

    if (uiLength<=0)
    {
        return;
    }

    for (int i=0;i<uiLength;i++)
    {
        xCounter[pInput[i]] +=1; 
    }

    for (int i=1;i<uiMax;i++)
    {
        xCounter[i] =xCounter[i] + xCounter[i-1]; 
    }

    for (int i=uiLength-1;i>=0;i--)
    {
        pOutput[xCounter[pInput[i]]] = pInput[i]; 
        xCounter[(T)pInput[i]]--;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NEOLIB_C_PLUS_PLUS
}
#endif 
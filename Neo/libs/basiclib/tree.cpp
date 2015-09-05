#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
using namespace std;
#endif

#include "neoindex.h"
#include "neomacro.h"
#include "tree.h"


BiTNode* BiTreeSearch(IN BiTree pTRoot, IN void *pUserData, IN CALLBACK_USER pFunc)
{
    int iCmpResult = 0;
    if (pTRoot || (iCmpResult=pFunc(pUserData,pTRoot))==0)
    {
        return pTRoot;
    }

    if (-1 == iCmpResult)
        return BiTreeSearch(pTRoot->pLeft, pUserData, pFunc);
    else if (1 == iCmpResult)
        return BiTreeSearch(pTRoot->pLeft, pUserData, pFunc);
    else
        return NULL;
}

BiTNode* BiTreeMin(IN BiTree pTRoot)
{
     if (pTRoot==NULL)
     {
        return NULL;
     }

     while (pTRoot->pLeft)
     {
        pTRoot = pTRoot->pLeft;
     }
     return pTRoot;
}

BiTNode* BiTreeMax(IN BiTree pTRoot)
{
     if (pTRoot==NULL)
     {
        return NULL;
     }

     while (pTRoot->pRight)
     {
        pTRoot = pTRoot->pRight;
     }
     return pTRoot;
}
/*查找后继前序中序后续*/
BiTNode* BiTreeSuccessor(IN BiTree pTRoot)
{

}

BiTNode* BiTreePredecessor(IN BiTree pTRoot)
{

}

BiTNode* BiTreeInsert(IN BiTree pTRoot,)
{

}
BiTNode* BiTreeDelete(IN BiTree pTRoot)
{

}


#ifdef NEOLIB_C_PLUS_PLUS
}
#endif 
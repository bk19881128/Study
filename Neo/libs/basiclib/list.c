#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
#endif
//#include <stddef.h>
//#include<stdlib.h>
//#include <string.h>


#include "neoindex.h"
#include "neomacro.h"
#include "list.h"


/*单项链表*/
void InitLNodeList(LHead *pstHead)
{
	pstHead->stHead.next = NULL;
}

void DestroyLNodeList(LHead *pstHead)
{

}

/*在指定的节点后插入新节点，若指定节点为空在末尾插入*/
void InsertLNode(LNode *pstDestNode, LNode *pstSourceNode)
{
	
	pstSourceNode->next = pstDestNode->next;
	pstDestNode->next = pstSourceNode;

	return;
}

void DeleteLNode(LNode *pstDestNode)
{
	LNode *pstTmp = NULL;

    pstTmp = pstDestNode->next;
    pstDestNode->next = pstTmp->next;

	return;
}

/*双向链表*/
void InitDNodeList(DHead *pstHead)
{
	pstHead->stHead.next = NULL;
	pstHead->stHead.pre = NULL;
}

void DestroyDNodeList(DHead *pstHead)
{

}

/*在指定的节点后面插入*/
void InsertDNode(DNode *pstDestNode, DNode *pstSourceNode)
{
	pstSourceNode->next = pstDestNode->next;
	pstDestNode->next->pre = pstSourceNode;
	pstDestNode->next = pstSourceNode;
	pstSourceNode->pre = pstDestNode;

}

void DeleteDNode(DNode *pstDestNode)
{
	pstDestNode->next->next->pre =  pstDestNode;
	pstDestNode->next = pstDestNode->next->next;
}

/*双重指针的双向链表*/
void InitDuNodeList(DuHead *pstHead)
{
	pstHead->stHead.next = NULL;
	pstHead->stHead.pre = NULL;
}

void DestroyDuNodeList(DuHead *pstHead)
{

}

void InsertDuNode(DuNode *pstDestNode, DuNode *pstSourceNode)
{
	pstSourceNode->next = pstDestNode->next;
	pstDestNode->next->pre = &pstSourceNode->next;//双重指针前向指向节点的地址==父节点中next指针的地址
	pstDestNode->next = pstSourceNode;
	pstSourceNode->pre = &pstDestNode->next;

}

void DeleteDuNode(DuNode *pstDestNode)
{
	pstDestNode->next->next->pre = &pstDestNode->next;
	pstDestNode->next = pstDestNode->next->next;
}

/*hash表*/

int CreateHashTable(IN unsigned long ulSize, IN IN CALLBACK_HASH pHashFunc, INOUT HashTable *pstHashTable)
{
	HashTable stHashTable = {0};
	
	pstHashTable->ulSize = ulSize;
	pstHashTable->pFunc = pHashFunc;
	pstHashTable->pstHashTbl = (HashNode *)malloc(ulSize * sizeof(HashNode));
	if( NULL == pstHashTable->pstHashTbl)
	{
		return ERR_ALLOC_MEM_FAILED;
	}
	
	pstHashTable = &stHashTable;
	return ERR_SUCCESS;
}

int DestroyHashTable(IN HashTable *pstHashTable, IN void (* pfFree)(void *))
{
	if (NULL == pstHashTable)
	{
		return ERR_SUCCESS;
	}

	pfFree((void *)pstHashTable);
	if (NULL == pstHashTable)
	{
		return ERR_SUCCESS;
	}

	return ERR_FAILED;
}

HashNode *GetDataInHashTable(IN HashTable *pstHashTable,IN CALLBACK_CMP pFunc, IN void *pstData)
{
	HashNode *pstHashNode = NULL;
	unsigned long ulIndex = 0;
	if (NULL == pstHashTable)
	{
		return NULL;
	}
	ulIndex = pstHashTable->pFunc(pstData);
	pstHashNode = pstHashTable->pstHashTbl+ulIndex-1;
	if (NULL == pstHashNode)
	{
		return NULL;
	}
	
	while(pstHashNode)
	{
		if (pFunc(pstData))
		{
			return pstHashNode;
		}

		pstHashNode = container_of(pstHashNode->pstNode->next,HashNode,pstNode);//////////////???container_of用法对吗？
	}

	return pstHashNode;
}

int InsertDataToHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData)
{
	/*unsigned long ulIndex = 0;
	HashNode *pstHashNode =NULL;
	if (NULL == pstHashTable)
	{
		return ERR_NULLPOINT;
	}

	if (NULL != GetDataInHashTable(pstHashTable, pFunc, pstData))
	{
		return ERR_AREADY_EXIST;
	}

	ulIndex = pstHashTable->pFunc(pstData);
	pstHashNode = pstHashTable->pstHashTbl+ulIndex-1;

	return ERR_SUCCESS;*/
}

int DeleteDataFromHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData)
{

}

void TraverseHashTable(IN HashTable *pstHashTable, IN void (*pfVisit)(void *))
{
	HashNode *pstHashNode = NULL;
	for (unsigned long i = 0; i < pstHashTable->ulSize;i++)
	{
		pstHashNode = pstHashTable->pstHashTbl+i;
		while(pstHashNode)
		{	
			pfVisit(pstHashNode);
			pstHashNode = container_of(pstHashNode->pstNode->next,HashNode,pstNode);//？用法不对
		}
	}
}

#ifdef NEOLIB_C_PLUS_PLUS
}
#endif 
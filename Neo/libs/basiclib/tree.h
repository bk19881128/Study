#ifndef NEO_TREE
#define NEO_TREE
#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////
/*
              XStruct--------
					|       |   ________
					|BiTNode|-->|pLeft  |
					|       |   |pRight |
                    |       |   |pParent|
					--------
*/
//pLeft == &BiNode

typedef struct BiTNode
{
	struct BiTNode *pLeft, *pRight, *pParent;
}BiTNode, *BiTree;

/*���ĺ����ֵܱ�ʾ��*/
typedef struct CSNode
{
	struct BiTNode *pChild, *pNextSibling;
}CSNode, *CSTree;


/*����������*/
//�û��Զ��庯�������ڸ������ڵ�ָ���ҵ���Ӧ�����ݲ���*pUserData�Ƚϣ����Ϊ0������Ϊ1��С��Ϊ-1
typedef int (*CALLBACK_USER) (IN void *pUserData, IN BiTNode *pTNode);
BiTNode* BiTreeSearch(IN BiTree pTRoot,IN void *pUserData, IN CALLBACK_USER pFunc);
BiTNode* BiTreeMin(IN BiTree pTRoot);
BiTNode* BiTreeMax(IN BiTree pTRoot);
/*���Һ��*/
BiTNode* BiTreeSuccessor(IN BiTree pTRoot);
BiTNode* BiTreePredecessor(IN BiTree pTRoot);
BiTNode* BiTreeInsert(IN BiTree pTRoot);
BiTNode* BiTreeDelete(IN BiTree pTRoot);

#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
#endif
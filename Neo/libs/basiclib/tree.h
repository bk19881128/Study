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

/*树的孩子兄弟表示法*/
typedef struct CSNode
{
	struct BiTNode *pChild, *pNextSibling;
}CSNode, *CSTree;


/*二叉搜索树*/
//用户自定义函数，用于根据树节点指针找到相应的数据并与*pUserData比较，相等为0，大于为1，小于为-1
typedef int (*CALLBACK_USER) (IN void *pUserData, IN BiTNode *pTNode);
BiTNode* BiTreeSearch(IN BiTree pTRoot,IN void *pUserData, IN CALLBACK_USER pFunc);
BiTNode* BiTreeMin(IN BiTree pTRoot);
BiTNode* BiTreeMax(IN BiTree pTRoot);
/*查找后继*/
BiTNode* BiTreeSuccessor(IN BiTree pTRoot);
BiTNode* BiTreePredecessor(IN BiTree pTRoot);
BiTNode* BiTreeInsert(IN BiTree pTRoot);
BiTNode* BiTreeDelete(IN BiTree pTRoot);

#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
#endif
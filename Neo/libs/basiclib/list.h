#ifndef NEO_LIST
#define NEO_LIST
#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
#endif
/*该文件定义了链表相关的结构体和函数*/

/*单向链表*/
typedef struct LNode {
	struct LNode *next;
}LNode;

typedef struct LHead {
	struct LNode stHead;
}LHead;

void InitLNodeList(LHead *pstHead);
void DestroyLNodeList(LHead *pstHead);
void InsertLNode(LNode *pstDestNode, LNode *pstSourceNode);
void DeleteLNode(LNode *pstDestNode);


/*带锁的单向列表*/
/*双向列表*/
typedef struct DNode {
	struct DNode *next;
	struct DNode *pre;
}DNode;

typedef struct DHead {
	struct DNode stHead;
}DHead;

void InitDNodeList(DHead *pstHead);
void DestroyDNodeList(DHead *pstHead);
void InsertDNode(DNode *pstDestNode, DNode *pstSourceNode);
void DeleteDNode(DNode *pstDestNode);

/*带锁的双向链表*/
/*二重指针的双向链表
                       ________                ________                  ________
                                 | next  |     -------->     | next |     ------>  | next |  ----> NULL
                NULL <----       | prev  |                   | prev |              | prev |
                                 -----------               ------------              ------------
                                           \              /          \             /   
                                             ___________               ___________
                                             | header1 |               | header2 |
*/
typedef struct DuNode {
	struct DuNode *next;
	struct DuNode **pre;
}DuNode;

typedef struct DuHead {
	struct DuNode stHead;
}DuHead;

void InitDuNodeList(DuHead *pstHead);
void DestroyDuNodeList(DuHead *pstHead);
void InsertDuNode(DuNode *pstDestNode, DuNode *pstSourceNode);
void DeleteDuNode(DuNode *pstDestNode);


/*hash链表*/
/*以链接法解决冲突*/
/*hash函数*/
typedef unsigned long (* CALLBACK_HASH) (void *pUserData);
typedef bool (* CALLBACK_CMP) (void *pUserData);
typedef struct HashNode {
	DuNode *pstNode;//实际存储信息的链表
	unsigned long ulSize;
	CALLBACK_HASH pFunc;
}HashNode;

typedef struct HashTable {
	unsigned long ulSize;
	CALLBACK_HASH pFunc;
	HashNode *pstHashTbl;//头指针是一个连续地址空间，存放各个链表的表头
}HashTable;

int CreateHashTable(IN unsigned long ulSize, IN CALLBACK_HASH pHashFunc,INOUT HashTable *pstHashTable);
int DestroyHashTable(IN HashTable *pstHashTable, IN void (* pfFree)(void *));
HashNode *GetDataInHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData);
void TraverseHashTable(IN HashTable *pstHashTable, IN (void *)pfVisit(void *));
int InsertDataToHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData);
int DeleteDataFromHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData);
/*带锁的hash表*/
#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
#endif

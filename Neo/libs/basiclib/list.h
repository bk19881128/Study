#ifndef NEO_LIST
#define NEO_LIST
#ifdef NEOLIB_C_PLUS_PLUS
extern "C" {
#endif
/*���ļ�������������صĽṹ��ͺ���*/

/*��������*/
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


/*�����ĵ����б�*/
/*˫���б�*/
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

/*������˫������*/
/*����ָ���˫������
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


/*hash����*/
/*�����ӷ������ͻ*/
/*hash����*/
typedef unsigned long (* CALLBACK_HASH) (void *pUserData);
typedef bool (* CALLBACK_CMP) (void *pUserData);
typedef struct HashNode {
	DuNode *pstNode;//ʵ�ʴ洢��Ϣ������
	unsigned long ulSize;
	CALLBACK_HASH pFunc;
}HashNode;

typedef struct HashTable {
	unsigned long ulSize;
	CALLBACK_HASH pFunc;
	HashNode *pstHashTbl;//ͷָ����һ��������ַ�ռ䣬��Ÿ�������ı�ͷ
}HashTable;

int CreateHashTable(IN unsigned long ulSize, IN CALLBACK_HASH pHashFunc,INOUT HashTable *pstHashTable);
int DestroyHashTable(IN HashTable *pstHashTable, IN void (* pfFree)(void *));
HashNode *GetDataInHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData);
void TraverseHashTable(IN HashTable *pstHashTable, IN (void *)pfVisit(void *));
int InsertDataToHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData);
int DeleteDataFromHashTable(IN HashTable *pstHashTable, IN CALLBACK_CMP pFunc, IN void *pstData);
/*������hash��*/
#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
#endif

#include <iostream>

using namespace std;

//we suppose the field data of the tree is int
typedef struct BinaryTreeNode {
	int node_value;
	struct BinaryTreeNode *left;
	struct BinaryTreeNode *right;
}BTNode, *BTree;

BTNode* CreateBTree(BTNode* root);

void PrintBTree(BTNode* root);

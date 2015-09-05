#include "tree_common.h"
#include "../../Utility/common.h"

BTNode* CreateBTree(BTNode* root) {
	string input;
	//if the data node is not an number, the node is null
	cout << "please input an number:" << endl;
	cin >> input;
	if(isnum(input)) {
		root = (BTNode*)malloc(sizeof(BTNode));
		if(!root) {
			cout << "malloc fail" << endl;
			return root;
		}
		else {
			root->node_value = stringToInt(input);
			root->left = CreateBTree(root->left);
			root->right = CreateBTree(root->right);
		}
	}
	else {
		root = NULL;
	}
	
	return root;
}

void PrintBTree(BTNode* root) {
	if(!root)
		return;
	PrintBTree(root->left);
	cout << root->node_value << endl;
	PrintBTree(root->right);
}

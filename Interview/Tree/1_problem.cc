/*
There is a binary tree, the node is ether positive integer or negative interger.
To find a subtree to make the sum of its nodes is the max.
*/

#include "tree_common.h"

int result;
bool maxSet;

void postOrderVisit(BTNode* root) {
	if( root == NULL ) {
		return;
	}
	if( root->left != NULL ) {
		postOrderVisit(root->left);
		root->node_value += root->left->node_value;
	}
        if( root->right != NULL ) {
                postOrderVisit(root->right);
                root->node_value += root->right->node_value;
        }
	if( maxSet ) {
		if(root->node_value > result)
			result = root->node_value;
	}
	else {
		result = root->node_value;
		maxSet = true;
	}
}

int main() {
        BTNode *root = NULL;
        root = CreateBTree(root);
	postOrderVisit(root);
        cout << endl;
        PrintBTree(root);
}

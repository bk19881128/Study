/*
To judge if a tree is a subtree of another tree
*/

#include "tree_common.h"

bool isPart(BTNode *root1,  BTNode *root2) {
	if (root2 == NULL)
		return true;
	if (root1 == NULL)
		return false;
	if (root1->node_value != root2->node_value)
		return false;

	return isPart(root1->left, root2->left) && isPart(root1->right, root2->right);
}

bool isTreePart(BTNode *root1,  BTNode *root2) {
	bool result = false;
	if ( root1 != NULL && root2 != NULL) {
		if(root1->node_value == root2->node_value)
			result = isPart(root1, root2);
		if (!result)
			result = isTreePart(root1->left, root2);
		if (!result)
			result = isTreePart(root1->right, root2);
	}
	return result;
}

int main() {
	BTNode *root1 = NULL, *root2 = NULL;
	bool result;
	
	cout << "please input tree one" << endl;
	root1 = CreateBTree(root1);
	cout << "please input tree two" << endl;
	root2 = CreateBTree(root2);
	result = isTreePart(root1, root2);
	if (result) {
		cout << "tree two is the subtree of tree one" << endl;
	}
	else {
		cout << "tree two is not the subtree of tree one" << endl;
	}
}

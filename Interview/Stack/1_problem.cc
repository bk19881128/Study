/*
Realize a stack class supported the function to return the minimum value.
*/

#include "getMinStack.h"
#include <iostream>

using namespace std;

int main() {

	getMinStack stack1;

	//construct a stack
	cout << "construct stack" << endl;
	stack1.push(3);
	stack1.push(5);
	stack1.push(2);
	stack1.push(1);
	stack1.push(4);
	stack1.push(6);

	cout << "the minimum of the stack:" << endl;
	cout << stack1.getmin() << endl;

	cout << "pop the stack: " << endl;
	cout << stack1.pop() << endl;

	cout << "the minimum of the stack:" << endl;
	cout << stack1.getmin() << endl;
}

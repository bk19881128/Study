/*
use recursion to reverse a stack.
*/

#include <stack>
#include <iostream>

using namespace std;

int return_and_remove_the_stack_bottom(stack<int> &s) {
	int value = s.top();
	s.pop();
	if (s.empty())
		return value;
	else {
		int last = return_and_remove_the_stack_bottom(s);
		s.push(value);
		return last;
	}
}

void my_reserve_stack(stack<int> &s) {
	if(s.empty())
		return;
	int value = return_and_remove_the_stack_bottom(s);
	my_reserve_stack(s);
	s.push(value);

}

int main() {


	stack<int> stack1;
	cout << "before reserve the stack is " << endl;
	cout << "6 4 1 2 5 3" << endl;

	stack1.push(3);
	stack1.push(5);
	stack1.push(2);
	stack1.push(1);
	stack1.push(4);
	stack1.push(6);

	my_reserve_stack(stack1);

	cout << "after reserve the stack is " << endl;
	while(!stack1.empty()) {
		cout << stack1.top() << " ";
		stack1.pop();		
	}
	cout << endl;

}

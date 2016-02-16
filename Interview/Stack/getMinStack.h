#ifndef _BKGETMINSTACK_H_
#define _BKGETMINSTACK_H_

#include <stack>
#include <stdexcept>
using namespace std;

class getMinStack {
public:
	void push(int newNum);
	int pop();
	int getmin();

private:
	stack<int> stackData;
	stack<int> stackMin;
};

void getMinStack::push(int newNum) {
	if(stackMin.empty()) {
		stackData.push(newNum);
		stackMin.push(newNum);
	}
	else{
		stackData.push(newNum);
		if(newNum <= getmin())
			stackMin.push(newNum);
		else
			stackMin.push(getmin());
	}
}

int getMinStack::pop() {
	stackMin.pop();
	int value = stackData.top();
	stackData.pop();
	return value;
}

int getMinStack::getmin() {
	return stackMin.top();
}

#endif

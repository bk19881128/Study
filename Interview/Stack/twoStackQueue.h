#ifndef _BKTWOSTACKQUEUE_H_
#define _BKTWOSTACKQUEUE_H_

#include <stack>
#include <stdexcept>
using namespace std;

class twoStackQueue {
public:
	void add(int newNum);
	int poll();
	int peek();

private:
	stack<int> stack1;
	stack<int> stack2;
};

void twoStackQueue::add(int newNum) {
	stack1.push(newNum);
}

int twoStackQueue::peek() {
	int value;
	if (stack1.empty() && stack2.empty())
		throw runtime_error("Queue is empty");
	if (stack2.empty()) {
		while(!stack1.empty()) {
			stack2.push(stack1.top());
			stack1.pop();
		}
	}
	value = stack2.top();
	stack2.pop();
	return value;
}

int twoStackQueue::poll() {
	int value;
	if (stack1.empty() && stack2.empty())
		throw runtime_error("Queue is empty");
	if (stack2.empty()) {
		while(!stack1.empty()) {
			stack2.push(stack1.top());
			stack1.pop();
		}
	}
	value = stack2.top();
	return value;
}

#endif

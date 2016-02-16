/*
use two stacks to realize a queue.
*/

#include "twoStackQueue.h"
#include <iostream>

using namespace std;

int main() {

	twoStackQueue queue1;

	//construct a stack
	cout << "construct queue" << endl;
	queue1.add(3);
	queue1.add(5);
	queue1.add(2);
	queue1.add(1);
	queue1.add(4);
	queue1.add(6);

	cout << "remove the top of the queue:" << endl;
	cout << queue1.peek() << endl;

	cout << "get the top of the queue: " << endl;
	cout << queue1.poll() << endl;

	cout << "remove the top of the queue:" << endl;
	cout << queue1.peek() << endl;

	cout << "get the top of the queue: " << endl;
	cout << queue1.poll() << endl;

	twoStackQueue queue2;
	
	cout << "remove the top of the queue:" << endl;
	cout << queue2.peek() << endl;
}

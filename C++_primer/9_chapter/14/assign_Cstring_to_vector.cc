#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>

using namespace std;

int main()
{
	list<const char*> list1 = {"hello", "C++", "World"};
	vector<string> vector_string;
	vector_string.assign(list1.cbegin(), list1.cend());
	auto iter1 = vector_string.cbegin();
	auto iter2 = vector_string.cend();
	while(iter1 != iter2) {
		cout << *iter1 << " ";
		++iter1;
	}
	cout << endl;

	return 0;
}	

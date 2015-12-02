#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>

using namespace std;

int main()
{

	//10.14
	auto f1 = [](int i, int j){return i+j;};
	auto m = f1(2,4);
	cout << m << endl;

	//10.15
	int i = 5;
	auto f2 = [i](int j){return i+j;};
	m = f2(4);
	cout << m << endl;

	return 0;
}	

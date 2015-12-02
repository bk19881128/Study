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

	int sz = 10;
	auto f1 = [&sz](){return (sz > 0) ? --sz : sz;};
	int m = 0;
	
	for(int i  = 0; i < 15; i++) {
		m = f1();
		cout << m << endl;
	}

	return 0;
}	

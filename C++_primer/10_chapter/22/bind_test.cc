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
using std::placeholders::_1;

bool check_size(const string &s, string::size_type sz)
{
	return s.size() >= sz;
}

int main()
{

	vector<string> v1 = {"adbc", "asdfasdf", "makevalefa", "sdadsf"};

	//10.22
	auto iter = find_if(v1.cbegin(), v1.cend(), bind(check_size, _1, 6));
	cout << *iter << endl;

	//10.24
	auto count = count_if(v1.cbegin(), v1.cend(), bind(check_size, _1, 6));
	cout << count << " words is larger then 6" << endl;


	return 0;
}	


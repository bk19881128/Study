#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <map>
#include <utility>
#include <cstddef>
#include <sstream>
using namespace std;

int main()
{
	vector<int> v1 = {1,2,3};
	vector<int> v2 = {2,3,4};
	string s1("abc");
	string s2("bcd");
	map<string, vector<int>> m({make_pair(s1,v1), make_pair(s2,v2)});

	map<string, vector<int>>::iterator p = m.find("bcd");
	auto iter = (*p).second.begin();
	while(iter != (*p).second.end()) {
		cout << *iter << " ";
		iter++;
	}
	cout << endl;

	return 0;
}	

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

	map<string, vector<int>> p;
	string s("my");
	vector<int> v = {0,2,3,5};

	p.insert(make_pair(s,v));
	
	auto map_iter = p.begin();
	while(map_iter != p.end()) {
		cout << (*map_iter).first << endl;
		auto iter = (*map_iter).second.begin();
		while(iter != (*map_iter).second.end()) {
			cout << *iter << " ";
			iter++;
		}
		cout << endl;
		map_iter++;

	}

	return 0;
}	

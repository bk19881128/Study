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
	map<string, vector<string>> family_list;
	string line;

	while(getline(cin, line)) {
		istringstream iss(line);
		pair<string, vector<string>> p;
		string record;
		if(iss >> record)
			p.first = record;
		while(iss >> record)
			p.second.push_back(record);

			family_list.insert(p);
	}
	
	for(const auto &w : family_list) {
		cout << "family last name: " << w.first << endl;
		cout << "have children: " << endl;
		for(auto iter = w.second.begin(); iter != w.second.end(); iter++)
			cout << *iter << " ";
		cout << endl;
	}

	return 0;
}	

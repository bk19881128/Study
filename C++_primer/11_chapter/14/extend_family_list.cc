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
	multimap<string, vector<pair<string, string>>> family_list;
	string line;

	while(getline(cin, line)) {
		istringstream iss(line);
		pair<string, vector<pair<string,string>>> p;
		string record;
		if(iss >> record)
			p.first = record;
		while(iss >> record) {
			pair<string,string> m;
			m.first = record;
			if(iss >> record)
				m.second = record;
			p.second.push_back(m);
		}

			family_list.insert(p);
	}
	
	for(const auto &w : family_list) {
		cout << "family last name: " << w.first << endl;
		cout << "have children: " << endl;
		for(auto iter = w.second.begin(); iter != w.second.end(); iter++)
			cout << (*iter).first << " " << (*iter).second << " ";
		cout << endl;
	}


	return 0;
}	

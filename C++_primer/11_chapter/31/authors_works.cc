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
//11.38
#include <tr1/unordered_map>
using std::tr1::unordered_map;
using namespace std;

int main()
{
	unordered_map<string, vector<string>> authors_works;
	string line;

	while(getline(cin, line)) {
		istringstream iss(line);
		pair<string, vector<string>> p;
		string record;
		if(iss >> record)
			p.first = record;
		while(iss >> record)
			p.second.push_back(record);

		sort(p.second.begin(), p.second.end());

		authors_works.insert(p);
	}
	
	//print
	for(const auto &w : authors_works) {
		cout << "author name: " << w.first << endl;
		cout << "work name: ";
		for(auto iter = w.second.begin(); iter != w.second.end(); iter++)
			cout << *iter << " ";
		cout << endl;
	}

	cout << endl;

	auto r1 = authors_works.find("Baokun");
	if(r1 != authors_works.end())
		authors_works.erase(r1);
        //print
        for(const auto &w : authors_works) {
                cout << "author name: " << w.first << endl;
                cout << "work name: ";
                for(auto iter = w.second.begin(); iter != w.second.end(); iter++)
                        cout << *iter << " ";
                cout << endl;
        }

        cout << endl;

        auto r2 = authors_works.find("Adsa");
        if(r2 != authors_works.end())
		authors_works.erase(r2);
        //print
        for(const auto &w : authors_works) {
                cout << "author name: " << w.first << endl;
                cout << "work name: ";
                for(auto iter = w.second.begin(); iter != w.second.end(); iter++)
                        cout << *iter << " ";
                cout << endl;
        }


	return 0;
}	

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
#include <set>
using namespace std;

int main()
{
	multiset<string> c = {"e", "f", "g"};
	vector<string> v = {"a", "b", "c", "d"};

	copy(v.begin(), v.end(), inserter(c, c.end()));
	auto multiset_iter = c.begin();
	while(multiset_iter != c.end()) {
		cout << *multiset_iter << " ";
		multiset_iter++;
	}
	cout << endl;

        multiset<string> c1 = {"e", "f", "g"};
        vector<string> v1 = {"a", "b", "c", "d"};

        copy(c1.begin(), c1.end(), inserter(v1, v1.end()));
        auto multiset_iter1 = v1.begin();
        while(multiset_iter1 != v1.end()) {
                cout << *multiset_iter1 << " ";
                multiset_iter1++;
        }
        cout << endl;

       multiset<string> c2 = {"e", "f", "g"};
        vector<string> v2 = {"a", "b", "c", "d"};

        copy(c2.begin(), c2.end(), back_inserter(v2));
        auto multiset_iter2 = v2.begin();
        while(multiset_iter2 != v2.end()) {
                cout << *multiset_iter2 << " ";
                multiset_iter2++;
        }
        cout << endl;

	return 0;
}	

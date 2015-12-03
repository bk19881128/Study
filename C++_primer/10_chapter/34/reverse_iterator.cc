#include <iostream>
#include <stdexcept> //runtime_error
#include <list>
#include <deque>
#include <vector>
#include <forward_list>
#include <string>
#include <stack>
#include <algorithm>
#include <iterator>
#include <fstream>

using namespace std;

int main()
{

	vector<int> v1 = {1,2,3,4,5,6,7,8,9};

	for(auto iter = v1.crbegin(); iter != v1.crend(); iter++)
		cout << *iter << " ";
	cout << endl;

        for(auto iter = --v1.cend(); iter != v1.cbegin(); iter--)
                cout << *iter << " ";
        cout << *v1.cbegin() << endl;

	list<int> l1;
	for(auto iter = v1.begin() + 2; iter != v1.begin() + 7; iter++)
		l1.push_back(*iter);

        for(auto iter = l1.cbegin(); iter != l1.cend(); iter++)
                cout << *iter << " ";
        cout << endl;


	return 0;
}	
